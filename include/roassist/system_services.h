#ifndef ROASSIST_SYSTEM_SERVICES_H
#define ROASSIST_SYSTEM_SERVICES_H

#include <QString>
#include <QStringList>
#include <QVector>

#include <cstdint>

namespace RoAssist {

struct ProcessCommand {
  QString label;
  QString program;
  QStringList arguments;
};

enum class UpdateStepKind : std::uint8_t {
  SystemPackages,
  FlatpakApplications,
  SnapPackages
};

struct UpdateStep {
  UpdateStepKind kind;
  ProcessCommand command;
  bool available;
};

struct UpdatePlan {
  QVector<UpdateStep> steps;
  bool flatpakAvailable;
  bool snapAvailable;
};

class SystemUpdateService {
public:
  static bool commandExists(const QString &command);
  static UpdatePlan buildUpdatePlan(bool flatpakAvailable, bool snapAvailable);
  static QVector<ProcessCommand> buildUpdateCommands(bool flatpakAvailable,
                                                     bool snapAvailable);
  static QString commandPreview(const ProcessCommand &command);
};

struct SystemRiskSnapshot {
  bool nvidiaGpuDetected = false;
  bool nouveauLoaded = false;
  bool roControlAvailable = false;
  bool rebootRequired = false;
  bool lowDiskSpace = false;
  qint64 availableDiskBytes = -1;
  QString sessionType;
};

class SystemRiskService {
public:
  static SystemRiskSnapshot collect();
  static bool detectsNvidiaGpu(const QString &lspciOutput);
  static bool detectsNouveauModule(const QString &lsmodOutput);
  static bool rebootRequiredFromNeedsRestarting(int exitCode,
                                                const QString &output);
  static bool isHighRiskGraphicsState(const SystemRiskSnapshot &snapshot);
  static QStringList warningCodes(const SystemRiskSnapshot &snapshot);
};

class RoControlIntegration {
public:
  static QString executableName();
  static bool isAvailable();
  static bool open();
};

} // namespace RoAssist

#endif // ROASSIST_SYSTEM_SERVICES_H
