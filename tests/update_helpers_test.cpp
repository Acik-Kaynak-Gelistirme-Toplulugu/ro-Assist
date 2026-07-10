#include "roassist/update_helpers.h"
#include "roassist/system_services.h"

#include <QtTest/QtTest>

using RoAssist::UpdateHelpers::TransactionProgress;
using RoAssist::UpdateHelpers::UpdateCheckStatus;

class UpdateHelpersTest : public QObject
{
    Q_OBJECT

private slots:
    void buildsSplitSystemUpdatePlan();
    void detectsNvidiaNouveauRisk();
    void classifiesRebootRequirement();
    void parsesLatestTransactionProgress();
    void parsesLatestDownloadPercent();
    void detectsNoWorkMarkers();
    void classifiesCheckUpdateResults();
};

void UpdateHelpersTest::buildsSplitSystemUpdatePlan()
{
    const auto commands =
        RoAssist::SystemUpdateService::buildUpdateCommands(true, true);

    QCOMPARE(commands.size(), 3);
    QCOMPARE(commands.at(0).program, QString("pkexec"));
    QCOMPARE(commands.at(0).arguments, QStringList({"dnf", "upgrade", "-y"}));
    QCOMPARE(commands.at(1).program, QString("flatpak"));
    QCOMPARE(commands.at(1).arguments, QStringList({"update", "-y"}));
    QCOMPARE(commands.at(2).program, QString("pkexec"));
    QCOMPARE(commands.at(2).arguments, QStringList({"snap", "refresh"}));
    QVERIFY(!RoAssist::SystemUpdateService::commandPreview(commands.at(0))
                 .contains("sh -c"));

    const auto dnfOnly =
        RoAssist::SystemUpdateService::buildUpdateCommands(false, false);
    QCOMPARE(dnfOnly.size(), 1);
}

void UpdateHelpersTest::detectsNvidiaNouveauRisk()
{
    const QString lspci =
        "01:00.0 VGA compatible controller: NVIDIA Corporation TU116\n";
    const QString lsmod =
        "nouveau              3891200  2\n"
        "drm_ttm_helper         16384  1 nouveau\n";

    QVERIFY(RoAssist::SystemRiskService::detectsNvidiaGpu(lspci));
    QVERIFY(RoAssist::SystemRiskService::detectsNouveauModule(lsmod));

    RoAssist::SystemRiskSnapshot snapshot;
    snapshot.nvidiaGpuDetected = true;
    snapshot.nouveauLoaded = true;
    QVERIFY(RoAssist::SystemRiskService::isHighRiskGraphicsState(snapshot));
    QVERIFY(RoAssist::SystemRiskService::warningCodes(snapshot)
                .contains("nvidia-nouveau"));
}

void UpdateHelpersTest::classifiesRebootRequirement()
{
    QVERIFY(RoAssist::SystemRiskService::rebootRequiredFromNeedsRestarting(
        1, "Reboot is required to fully utilize these updates."));
    QVERIFY(RoAssist::SystemRiskService::rebootRequiredFromNeedsRestarting(
        0, "restart is required"));
    QVERIFY(!RoAssist::SystemRiskService::rebootRequiredFromNeedsRestarting(
        0, "No core libraries or services have been updated."));
}

void UpdateHelpersTest::parsesLatestTransactionProgress()
{
    const QString output =
        "Installing packages (1/4)\nDownloading (2/4)\nDone (4/4)\n";
    const auto progress =
        RoAssist::UpdateHelpers::parseTransactionProgress(output);

    QVERIFY(progress.has_value());
    QCOMPARE(progress->current, 4);
    QCOMPARE(progress->total, 4);
}

void UpdateHelpersTest::parsesLatestDownloadPercent()
{
    const QString output = "foo 12%\nbar 78%\nbaz 100%\n";
    const auto percent = RoAssist::UpdateHelpers::parseDownloadPercent(output);

    QVERIFY(percent.has_value());
    QCOMPARE(*percent, 100);
}

void UpdateHelpersTest::detectsNoWorkMarkers()
{
    QVERIFY(RoAssist::UpdateHelpers::containsNoWorkMarker("Nothing to do."));
    QVERIFY(RoAssist::UpdateHelpers::containsNoWorkMarker(
        "Yapılacak bir şey yok"));
    QVERIFY(!RoAssist::UpdateHelpers::containsNoWorkMarker("Installing..."));
}

void UpdateHelpersTest::classifiesCheckUpdateResults()
{
    QCOMPARE(RoAssist::UpdateHelpers::classifyCheckUpdateResult(
                 0, QProcess::NormalExit),
             UpdateCheckStatus::UpToDate);
    QCOMPARE(RoAssist::UpdateHelpers::classifyCheckUpdateResult(
                 100, QProcess::NormalExit),
             UpdateCheckStatus::UpdatesAvailable);
    QCOMPARE(RoAssist::UpdateHelpers::classifyCheckUpdateResult(
                 1, QProcess::NormalExit),
             UpdateCheckStatus::Failed);
    QCOMPARE(RoAssist::UpdateHelpers::classifyCheckUpdateResult(
                 0, QProcess::CrashExit),
             UpdateCheckStatus::Failed);
}

QTEST_MAIN(UpdateHelpersTest)

#include "update_helpers_test.moc"
