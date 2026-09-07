#include "roassist/mainwindow.h"

#include <QtTest/QtTest>
#include <QDir>
#include <QSettings>
#include <QScrollArea>
#include <QScrollBar>
#include <QStandardPaths>

class MainWindowIntegrationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void init();
    void languageSwitchUpdatesPrimaryTexts();
    void systemLocaleSelectsSupportedLanguageAndFallsBackToEnglish();
    void networkIndicatorFollowsConnectivity();
    void firstRunRequiresOrderedCompletion();
    void welcomePageCanOpenAndReturnToItsCurrentStep();
    void printerSupportPreferencesArePersisted();
    void telemetryPreferencePagePersistsSliderLevel();
    void updateScreenShowsNouveauRiskAndRoControlAction();
    void compactWindowKeepsLongPagesReachable();
};

void MainWindowIntegrationTest::initTestCase()
{
    qputenv("RO_ASSIST_SKIP_SYSTEM_CHECKS", "1");
    qputenv("QT_QPA_PLATFORM", "offscreen");
    QStandardPaths::setTestModeEnabled(true);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope,
                       QDir::tempPath() + "/ro-assist-tests");
    QCoreApplication::setOrganizationName("Project-Ro-ASD");
    QCoreApplication::setApplicationName("ro-assist");
}

void MainWindowIntegrationTest::init()
{
    qunsetenv("RO_ASSIST_TEST_LSPCI");
    qunsetenv("RO_ASSIST_TEST_LSMOD");
    qunsetenv("RO_ASSIST_TEST_RO_CONTROL_AVAILABLE");
    qunsetenv("RO_ASSIST_TEST_REBOOT_REQUIRED");
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "Project-Ro-ASD", "ro-assist");
    settings.clear();
    settings.sync();
}

void MainWindowIntegrationTest::languageSwitchUpdatesPrimaryTexts()
{
    MainWindow window;
    auto *languageButton = window.findChild<QPushButton *>("languageButton");
    auto *updateButton = window.findChild<QPushButton *>("systemUpdateButton");
    QVERIFY(languageButton);
    QVERIFY(updateButton);

    QAction english;
    english.setData(QStringLiteral("en"));
    QMetaObject::invokeMethod(&window, "changeLanguage",
                              Q_ARG(QAction *, &english));
    QVERIFY(languageButton->text().contains("English"));
    QCOMPARE(updateButton->text(), QString("Update System"));

    QAction turkish;
    turkish.setData(QStringLiteral("tr"));
    QMetaObject::invokeMethod(&window, "changeLanguage",
                              Q_ARG(QAction *, &turkish));
    QVERIFY(languageButton->text().contains("Türkçe"));
    QCOMPARE(updateButton->text(), QString("Sistemi Güncelle"));

    QAction spanish;
    spanish.setData(QStringLiteral("es"));
    QMetaObject::invokeMethod(&window, "changeLanguage",
                              Q_ARG(QAction *, &spanish));
    QVERIFY(languageButton->text().contains("Español"));
    QCOMPARE(updateButton->text(), QString("Actualizar Sistema"));

    QAction german;
    german.setData(static_cast<int>(MainWindow::DE));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &german));
    QCOMPARE(languageButton->text(), QString("🇩🇪 Deutsch"));
    QCOMPARE(updateButton->text(), QString("System aktualisieren"));

    QAction french;
    french.setData(static_cast<int>(MainWindow::FR));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &french));
    QCOMPARE(languageButton->text(), QString("🇫🇷 Français"));
    QCOMPARE(updateButton->text(), QString("Mettre le système à jour"));
}

void MainWindowIntegrationTest::systemLocaleSelectsSupportedLanguageAndFallsBackToEnglish()
{
    const QByteArray previousLanguage = qgetenv("LANG");
    qputenv("LANG", "tr_TR.UTF-8");

    MainWindow window;
    auto *languageButton = window.findChild<QPushButton *>("languageButton");
    QVERIFY(languageButton);
    QCOMPARE(languageButton->text(), QString("🇹🇷 Türkçe"));

    qputenv("LANG", "zh_CN.UTF-8");
    QEvent localeChange(QEvent::LocaleChange);
    QCoreApplication::sendEvent(&window, &localeChange);
    QCOMPARE(languageButton->text(), QString("🇬🇧 English"));

    if (previousLanguage.isEmpty())
        qunsetenv("LANG");
    else
        qputenv("LANG", previousLanguage);
}

void MainWindowIntegrationTest::networkIndicatorFollowsConnectivity()
{
    MainWindow window;
    auto *networkStatusLabel =
        window.findChild<QLabel *>("networkStatusLabel");
    QVERIFY(networkStatusLabel);
    window.show();
    QCoreApplication::processEvents();

    QMetaObject::invokeMethod(&window, "onNetworkConnectedChanged",
                              Q_ARG(bool, false));
    QVERIFY(!networkStatusLabel->isHidden());

    QMetaObject::invokeMethod(&window, "onNetworkConnectedChanged",
                              Q_ARG(bool, true));
    QVERIFY(networkStatusLabel->isHidden());
}

void MainWindowIntegrationTest::firstRunRequiresOrderedCompletion()
{
    MainWindow window;
    auto *nextButton = window.findChild<QPushButton *>("welcomeNextButton");
    auto *mainStack = window.findChild<QStackedWidget *>("mainStack");
    auto *dashboardCard =
        window.findChild<QPushButton *>("dashboardUpdateCard");
    QVERIFY(nextButton);
    QVERIFY(mainStack);
    QVERIFY(dashboardCard);

    QAction english;
    english.setData(static_cast<int>(MainWindow::EN));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &english));
    QCOMPARE(nextButton->text(), QString("Next"));

    for (int index = 0; index < 5; ++index) {
        QMetaObject::invokeMethod(&window, "advanceWelcome");
    }
    QCOMPARE(nextButton->text(), QString("Open ro-Assist"));

    QMetaObject::invokeMethod(&window, "advanceWelcome");
    QVERIFY(mainStack->currentWidget()->isAncestorOf(dashboardCard));

    MainWindow reopenedWindow;
    auto *reopenedMainStack =
        reopenedWindow.findChild<QStackedWidget *>("mainStack");
    auto *reopenedDashboardCard =
        reopenedWindow.findChild<QPushButton *>("dashboardUpdateCard");
    QVERIFY(reopenedMainStack);
    QVERIFY(reopenedDashboardCard);
    QVERIFY(reopenedMainStack->currentWidget()->isAncestorOf(
        reopenedDashboardCard));
}

void MainWindowIntegrationTest::welcomePageCanOpenAndReturnToItsCurrentStep()
{
    MainWindow window;
    window.show();
    QCoreApplication::processEvents();

    auto *mainStack = window.findChild<QStackedWidget *>("mainStack");
    auto *welcomeStack = window.findChild<QStackedWidget *>("welcomeStack");
    auto *backButton = window.findChild<QPushButton *>("backToHomeButton");
    QVERIFY(mainStack);
    QVERIFY(welcomeStack);
    QVERIFY(backButton);

    QAction english;
    english.setData(static_cast<int>(MainWindow::EN));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &english));

    QMetaObject::invokeMethod(&window, "advanceWelcome");
    QCOMPARE(welcomeStack->currentIndex(), 1);
    auto *title =
        welcomeStack->currentWidget()->findChild<QLabel *>("slideTitle");
    QVERIFY(title);

    QTest::mouseClick(title, Qt::LeftButton);
    QCOMPARE(mainStack->currentIndex(), 5);

    QTest::mouseClick(backButton, Qt::LeftButton);
    QCOMPARE(mainStack->currentIndex(), 0);
    QCOMPARE(welcomeStack->currentIndex(), 1);
}

void MainWindowIntegrationTest::printerSupportPreferencesArePersisted()
{
    MainWindow window;
    window.show();
    QCoreApplication::processEvents();
    auto *printerCard =
        window.findChild<QPushButton *>("dashboardPrinterCard");
    auto *mainStack = window.findChild<QStackedWidget *>("mainStack");
    auto *printerView = window.findChild<QWidget *>("printerSupportView");
    auto *laterButton =
        window.findChild<QPushButton *>("printerSupportLaterButton");
    auto *disableButton =
        window.findChild<QPushButton *>("printerSupportDisableButton");
    QVERIFY(printerCard);
    QVERIFY(mainStack);
    QVERIFY(printerView);
    QVERIFY(laterButton);
    QVERIFY(disableButton);

    QMetaObject::invokeMethod(&window, "showDashboardScreen");
    QTest::mouseClick(printerCard, Qt::LeftButton);
    QCOMPARE(mainStack->currentWidget(), printerView);

    QTest::mouseClick(laterButton, Qt::LeftButton);
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "Project-Ro-ASD", "ro-assist");
    QCOMPARE(settings.value("printer/supportPreference").toString(),
             QString("later"));

    QTest::mouseClick(disableButton, Qt::LeftButton);
    QCOMPARE(settings.value("printer/supportPreference").toString(),
             QString("disabled"));
}

void MainWindowIntegrationTest::telemetryPreferencePagePersistsSliderLevel()
{
    MainWindow window;
    window.show();
    QCoreApplication::processEvents();
    auto *telemetryCard =
        window.findChild<QPushButton *>("dashboardTelemetryCard");
    auto *mainStack = window.findChild<QStackedWidget *>("mainStack");
    auto *telemetryView = window.findChild<QWidget *>("telemetryView");
    auto *slider = window.findChild<QSlider *>("telemetryLevelSlider");
    auto *currentLevelLabel =
        window.findChild<QLabel *>("telemetryCurrentLevelLabel");
    QVERIFY(telemetryCard);
    QVERIFY(mainStack);
    QVERIFY(telemetryView);
    QVERIFY(slider);
    QVERIFY(currentLevelLabel);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       "Project-Ro-ASD", "ro-assist");
    QCOMPARE(slider->value(), 1);
    QCOMPARE(settings.value("telemetry/level").toInt(), 1);
    QCOMPARE(settings.value("telemetry/levelName").toString(),
             QString("count"));

    QMetaObject::invokeMethod(&window, "showDashboardScreen");
    QTest::mouseClick(telemetryCard, Qt::LeftButton);
    QCOMPARE(mainStack->currentWidget(), telemetryView);

    QAction english;
    english.setData(static_cast<int>(MainWindow::EN));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &english));
    QVERIFY(currentLevelLabel->text().contains("Selected level"));
    QVERIFY(currentLevelLabel->text().contains("Count"));

    slider->setValue(0);
    settings.sync();
    QCOMPARE(settings.value("telemetry/level").toInt(), 0);
    QCOMPARE(settings.value("telemetry/levelName").toString(), QString("off"));

    slider->setValue(3);
    settings.sync();
    QCOMPARE(settings.value("telemetry/level").toInt(), 3);
    QCOMPARE(settings.value("telemetry/levelName").toString(),
             QString("extended"));
    QVERIFY(currentLevelLabel->text().contains("Extended"));
}

void MainWindowIntegrationTest::compactWindowKeepsLongPagesReachable()
{
    MainWindow window;
    window.resize(900, 520);
    window.show();
    QCoreApplication::processEvents();

    QMetaObject::invokeMethod(&window, "showDashboardScreen");
    auto *dashboardScroll =
        window.findChild<QScrollArea *>("dashboardScrollArea");
    auto *telemetryScroll =
        window.findChild<QScrollArea *>("telemetryScrollArea");
    auto *printerScroll =
        window.findChild<QScrollArea *>("printerSupportScrollArea");
    QVERIFY(dashboardScroll);
    QVERIFY(telemetryScroll);
    QVERIFY(printerScroll);
    QVERIFY(dashboardScroll->verticalScrollBar()->maximum() > 0);

    dashboardScroll->verticalScrollBar()->setValue(
        dashboardScroll->verticalScrollBar()->maximum());
    QCoreApplication::processEvents();
    auto *lastCard = window.findChild<QPushButton *>("dashboardTelemetryCard");
    QVERIFY(lastCard);
    const QRect cardRect(lastCard->mapTo(dashboardScroll->viewport(),
                                         QPoint(0, 0)),
                         lastCard->size());
    QVERIFY(cardRect.intersects(dashboardScroll->viewport()->rect()));
}

void MainWindowIntegrationTest::updateScreenShowsNouveauRiskAndRoControlAction()
{
    qputenv("RO_ASSIST_TEST_LSPCI",
            "01:00.0 VGA compatible controller: NVIDIA Corporation TU116");
    qputenv("RO_ASSIST_TEST_LSMOD", "nouveau 3891200 2");
    qputenv("RO_ASSIST_TEST_RO_CONTROL_AVAILABLE", "1");

    MainWindow window;
    window.show();
    QCoreApplication::processEvents();

    QAction english;
    english.setData(static_cast<int>(MainWindow::EN));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &english));
    QMetaObject::invokeMethod(&window, "showUpdateScreen");

    auto *riskLabel =
        window.findChild<QLabel *>("maintenanceRiskLabel");
    auto *roControlButton =
        window.findChild<QPushButton *>("openRoControlButton");
    QVERIFY(riskLabel);
    QVERIFY(roControlButton);
    QTRY_VERIFY(riskLabel->text().contains("NVIDIA GPU with nouveau"));
    QVERIFY(!roControlButton->isHidden());
    QVERIFY(roControlButton->isEnabled());
    QCOMPARE(roControlButton->text(), QString("Open ro Control"));

    qunsetenv("RO_ASSIST_TEST_LSPCI");
    qunsetenv("RO_ASSIST_TEST_LSMOD");
    qunsetenv("RO_ASSIST_TEST_RO_CONTROL_AVAILABLE");
}

QTEST_MAIN(MainWindowIntegrationTest)

#include "mainwindow_integration_test.moc"
