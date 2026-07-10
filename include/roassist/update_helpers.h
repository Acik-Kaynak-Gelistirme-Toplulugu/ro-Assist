#ifndef ROASSIST_UPDATE_HELPERS_H
#define ROASSIST_UPDATE_HELPERS_H

#include <QProcess>
#include <QString>

#include <cstdint>
#include <optional>

namespace RoAssist::UpdateHelpers {

enum class UpdateCheckStatus : std::uint8_t {
    UpToDate,
    UpdatesAvailable,
    Failed
};

struct TransactionProgress {
    int current;
    int total;
};

std::optional<TransactionProgress> parseTransactionProgress(const QString &output);
std::optional<int> parseDownloadPercent(const QString &output);
bool containsNoWorkMarker(const QString &output);
UpdateCheckStatus classifyCheckUpdateResult(int exitCode,
                                            QProcess::ExitStatus exitStatus);

} // namespace RoAssist::UpdateHelpers

#endif // ROASSIST_UPDATE_HELPERS_H
