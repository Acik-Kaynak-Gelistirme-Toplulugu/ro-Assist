#include "roassist/update_helpers.h"

#include <QLatin1String>
#include <QRegularExpression>

namespace {

constexpr int DNF_UPDATES_EXIT_CODE = 100;

} // namespace

namespace RoAssist::UpdateHelpers {

QString buildSystemUpdateCommand() {
  return QStringLiteral(
      "LANG=C dnf upgrade -y && "
      "if command -v flatpak > /dev/null; then LANG=C flatpak update -y; fi && "
      "if command -v snap > /dev/null; then snap refresh; fi");
}

std::optional<TransactionProgress> parseTransactionProgress(
    const QString &output) {
  static const QRegularExpression txRegex(QStringLiteral("\\((\\d+)/(\\d+)\\)"));
  QRegularExpressionMatchIterator matches = txRegex.globalMatch(output);

  std::optional<TransactionProgress> lastMatch;
  while (matches.hasNext()) {
    const QRegularExpressionMatch match = matches.next();
    lastMatch = TransactionProgress{
        match.captured(1).toInt(),
        match.captured(2).toInt(),
    };
  }

  return lastMatch;
}

std::optional<int> parseDownloadPercent(const QString &output) {
  static const QRegularExpression dlRegex(QStringLiteral("(\\d+)%"));
  QRegularExpressionMatchIterator matches = dlRegex.globalMatch(output);

  std::optional<int> lastPercent;
  while (matches.hasNext()) {
    const QRegularExpressionMatch match = matches.next();
    lastPercent = match.captured(1).toInt();
  }

  return lastPercent;
}

bool containsNoWorkMarker(const QString &output) {
  return output.contains(QLatin1String("Nothing to do"), Qt::CaseInsensitive) ||
         output.contains(QStringLiteral("Yapılacak bir şey yok"),
                         Qt::CaseInsensitive);
}

UpdateCheckStatus classifyCheckUpdateResult(int exitCode,
                                            QProcess::ExitStatus exitStatus) {
  if (exitStatus != QProcess::NormalExit) {
    return UpdateCheckStatus::Failed;
  }

  if (exitCode == DNF_UPDATES_EXIT_CODE) {
    return UpdateCheckStatus::UpdatesAvailable;
  }

  if (exitCode == 0) {
    return UpdateCheckStatus::UpToDate;
  }

  return UpdateCheckStatus::Failed;
}

} // namespace RoAssist::UpdateHelpers
