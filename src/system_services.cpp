#include "roassist/system_services.h"

#include <QDir>
#include <QFile>
#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QStorageInfo>

namespace {

constexpr qint64 LOW_DISK_SPACE_BYTES = 2LL * 1024 * 1024 * 1024;

bool envFlagEnabled(const char *name) {
  const QByteArray value = qgetenv(name).trimmed().toLower();
  return value == "1" || value == "true" || value == "yes";
}

QString envString(const char *name) {
  return QString::fromUtf8(qgetenv(name));
}

QString runAndReadAll(const QString &program, const QStringList &arguments,
                      int timeoutMs = 1500) {
  QProcess process;
  process.start(program, arguments);
  if (!process.waitForFinished(timeoutMs)) {
    process.kill();
    process.waitForFinished(300);
  }
  return QString::fromUtf8(process.readAllStandardOutput()) +
         QString::fromUtf8(process.readAllStandardError());
}

int runAndReadAllWithExitCode(const QString &program,
                              const QStringList &arguments, QString *output,
                              int timeoutMs = 2000) {
  QProcess process;
  process.start(program, arguments);
  if (!process.waitForFinished(timeoutMs)) {
    process.kill();
    process.waitForFinished(300);
  }
  if (output) {
    *output = QString::fromUtf8(process.readAllStandardOutput()) +
              QString::fromUtf8(process.readAllStandardError());
  }
  return process.exitCode();
}

} // namespace

namespace RoAssist {

bool SystemUpdateService::commandExists(const QString &command) {
  const QByteArray overrideName =
      "RO_ASSIST_TEST_" + command.toUtf8().toUpper().replace('-', '_') +
      "_AVAILABLE";
  if (!qgetenv(overrideName.constData()).isEmpty())
    return envFlagEnabled(overrideName.constData());

  return !QStandardPaths::findExecutable(command).isEmpty();
}

UpdatePlan SystemUpdateService::buildUpdatePlan(bool flatpakAvailable,
                                                bool snapAvailable) {
  UpdatePlan plan;
  plan.flatpakAvailable = flatpakAvailable;
  plan.snapAvailable = snapAvailable;
  plan.steps = {
      {UpdateStepKind::SystemPackages,
       {QStringLiteral("System packages"), QStringLiteral("pkexec"),
        {QStringLiteral("dnf"), QStringLiteral("upgrade"),
         QStringLiteral("-y")}},
       true},
      {UpdateStepKind::FlatpakApplications,
       {QStringLiteral("Flatpak applications"), QStringLiteral("flatpak"),
        {QStringLiteral("update"), QStringLiteral("-y")}},
       flatpakAvailable},
      {UpdateStepKind::SnapPackages,
       {QStringLiteral("Snap packages"), QStringLiteral("pkexec"),
        {QStringLiteral("snap"), QStringLiteral("refresh")}},
       snapAvailable},
  };
  return plan;
}

QVector<ProcessCommand> SystemUpdateService::buildUpdateCommands(
    bool flatpakAvailable, bool snapAvailable) {
  QVector<ProcessCommand> commands;
  const UpdatePlan plan = buildUpdatePlan(flatpakAvailable, snapAvailable);
  for (const UpdateStep &step : plan.steps) {
    if (step.available)
      commands.push_back(step.command);
  }
  return commands;
}

QString SystemUpdateService::commandPreview(const ProcessCommand &command) {
  QStringList tokens{command.program};
  tokens.append(command.arguments);
  return tokens.join(QLatin1Char(' '));
}

SystemRiskSnapshot SystemRiskService::collect() {
  SystemRiskSnapshot snapshot;

  const QString lspciOutput =
      qEnvironmentVariableIsSet("RO_ASSIST_TEST_LSPCI")
          ? envString("RO_ASSIST_TEST_LSPCI")
          : runAndReadAll(QStringLiteral("lspci"), {});
  const QString lsmodOutput =
      qEnvironmentVariableIsSet("RO_ASSIST_TEST_LSMOD")
          ? envString("RO_ASSIST_TEST_LSMOD")
          : runAndReadAll(QStringLiteral("lsmod"), {});

  snapshot.nvidiaGpuDetected = detectsNvidiaGpu(lspciOutput);
  snapshot.nouveauLoaded = detectsNouveauModule(lsmodOutput);
  snapshot.roControlAvailable = RoControlIntegration::isAvailable();
  snapshot.sessionType = qEnvironmentVariable("XDG_SESSION_TYPE");

  if (qEnvironmentVariableIsSet("RO_ASSIST_TEST_REBOOT_REQUIRED")) {
    snapshot.rebootRequired = envFlagEnabled("RO_ASSIST_TEST_REBOOT_REQUIRED");
  } else if (QFile::exists(QStringLiteral("/run/reboot-required")) ||
             QFile::exists(QStringLiteral("/var/run/reboot-required"))) {
    snapshot.rebootRequired = true;
  } else if (SystemUpdateService::commandExists(QStringLiteral("needs-restarting"))) {
    QString output;
    const int exitCode = runAndReadAllWithExitCode(
        QStringLiteral("needs-restarting"), {QStringLiteral("-r")}, &output);
    snapshot.rebootRequired =
        rebootRequiredFromNeedsRestarting(exitCode, output);
  }

  QStorageInfo root = QStorageInfo::root();
  if (root.isValid() && root.isReady()) {
    snapshot.availableDiskBytes = root.bytesAvailable();
    snapshot.lowDiskSpace = snapshot.availableDiskBytes >= 0 &&
                            snapshot.availableDiskBytes < LOW_DISK_SPACE_BYTES;
  }

  return snapshot;
}

bool SystemRiskService::detectsNvidiaGpu(const QString &lspciOutput) {
  return lspciOutput.contains(QStringLiteral("nvidia"), Qt::CaseInsensitive);
}

bool SystemRiskService::detectsNouveauModule(const QString &lsmodOutput) {
  static const QRegularExpression nouveauRegex(
      QStringLiteral("(^|\\n)nouveau\\s"), QRegularExpression::CaseInsensitiveOption);
  return nouveauRegex.match(lsmodOutput).hasMatch();
}

bool SystemRiskService::rebootRequiredFromNeedsRestarting(
    int exitCode, const QString &output) {
  if (exitCode == 1)
    return true;
  return output.contains(QStringLiteral("reboot is required"),
                         Qt::CaseInsensitive) ||
         output.contains(QStringLiteral("restart is required"),
                         Qt::CaseInsensitive);
}

bool SystemRiskService::isHighRiskGraphicsState(
    const SystemRiskSnapshot &snapshot) {
  return snapshot.nvidiaGpuDetected && snapshot.nouveauLoaded;
}

QStringList SystemRiskService::warningCodes(const SystemRiskSnapshot &snapshot) {
  QStringList warnings;
  if (isHighRiskGraphicsState(snapshot))
    warnings.append(QStringLiteral("nvidia-nouveau"));
  if (snapshot.lowDiskSpace)
    warnings.append(QStringLiteral("low-disk"));
  if (snapshot.rebootRequired)
    warnings.append(QStringLiteral("reboot-required"));
  return warnings;
}

QString RoControlIntegration::executableName() {
  return QStringLiteral("ro-control");
}

bool RoControlIntegration::isAvailable() {
  if (qEnvironmentVariableIsSet("RO_ASSIST_TEST_RO_CONTROL_AVAILABLE"))
    return envFlagEnabled("RO_ASSIST_TEST_RO_CONTROL_AVAILABLE");
  return SystemUpdateService::commandExists(executableName());
}

bool RoControlIntegration::open() {
  if (!isAvailable())
    return false;
  if (qEnvironmentVariableIsSet("RO_ASSIST_TEST_RO_CONTROL_OPEN"))
    return envFlagEnabled("RO_ASSIST_TEST_RO_CONTROL_OPEN");
  return QProcess::startDetached(executableName());
}

} // namespace RoAssist
