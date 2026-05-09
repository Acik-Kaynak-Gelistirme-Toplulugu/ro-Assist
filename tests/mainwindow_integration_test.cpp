#define private public
#include "roassist/mainwindow.h"
#undef private

#include <QtTest/QtTest>

class MainWindowIntegrationTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void languageSwitchUpdatesPrimaryTexts();
    void networkIndicatorFollowsConnectivity();
};

void MainWindowIntegrationTest::initTestCase()
{
    qputenv("RO_ASSIST_SKIP_SYSTEM_CHECKS", "1");
    qputenv("QT_QPA_PLATFORM", "offscreen");
}

void MainWindowIntegrationTest::languageSwitchUpdatesPrimaryTexts()
{
    MainWindow window;

    QAction english;
    english.setData(static_cast<int>(MainWindow::EN));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &english));
    QCOMPARE(window.langBtn->text(), QString("🇬🇧 English"));
    QCOMPARE(window.updateButton->text(), QString("Update System"));

    QAction turkish;
    turkish.setData(static_cast<int>(MainWindow::TR));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &turkish));
    QCOMPARE(window.langBtn->text(), QString("🇹🇷 Türkçe"));
    QCOMPARE(window.updateButton->text(), QString("Sistemi Güncelle"));

    QAction spanish;
    spanish.setData(static_cast<int>(MainWindow::ES));
    QMetaObject::invokeMethod(&window, "changeLanguageAction",
                              Q_ARG(QAction *, &spanish));
    QCOMPARE(window.langBtn->text(), QString("🇪🇸 Español"));
    QCOMPARE(window.updateButton->text(), QString("Actualizar Sistema"));
}

void MainWindowIntegrationTest::networkIndicatorFollowsConnectivity()
{
    MainWindow window;
    window.show();
    QCoreApplication::processEvents();

    QMetaObject::invokeMethod(&window, "onNetworkConnectedChanged",
                              Q_ARG(bool, false));
    QVERIFY(!window.networkStatusLabel->isHidden());

    QMetaObject::invokeMethod(&window, "onNetworkConnectedChanged",
                              Q_ARG(bool, true));
    QVERIFY(window.networkStatusLabel->isHidden());
}

QTEST_MAIN(MainWindowIntegrationTest)

#include "mainwindow_integration_test.moc"
