#ifndef ROASSIST_UPDATE_HELPERS_H
#define ROASSIST_UPDATE_HELPERS_H

#include <QProcess>
#include <QString>

#include <optional>

namespace RoAssist::UpdateHelpers {

enum class UpdateCheckStatus {
    UpToDate,
    UpdatesAvailable,
    Failed
};

struct TransactionProgress {
    int current;
    int total;
};

QString buildSystemUpdateCommand();
std::optional<TransactionProgress> parseTransactionProgress(const QString &output);
std::optional<int> parseDownloadPercent(const QString &output);
bool containsNoWorkMarker(const QString &output);
UpdateCheckStatus classifyCheckUpdateResult(int exitCode,
                                            QProcess::ExitStatus exitStatus);

} // namespace RoAssist::UpdateHelpers

#endif // ROASSIST_UPDATE_HELPERS_H
