#include "roassist/update_helpers.h"

#include <QtTest/QtTest>

using RoAssist::UpdateHelpers::TransactionProgress;
using RoAssist::UpdateHelpers::UpdateCheckStatus;

class UpdateHelpersTest : public QObject
{
    Q_OBJECT

private slots:
    void buildsSystemUpdateCommand();
    void parsesLatestTransactionProgress();
    void parsesLatestDownloadPercent();
    void detectsNoWorkMarkers();
    void classifiesCheckUpdateResults();
};

void UpdateHelpersTest::buildsSystemUpdateCommand()
{
    const QString command = RoAssist::UpdateHelpers::buildSystemUpdateCommand();
    QVERIFY(command.contains("dnf upgrade -y"));
    QVERIFY(command.contains("flatpak update -y"));
    QVERIFY(command.contains("snap refresh"));
    QVERIFY(command.contains("&&"));
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
