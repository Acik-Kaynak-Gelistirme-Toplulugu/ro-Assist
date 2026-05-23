#include "roassist/mainwindow.h"

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

QTEST_MAIN(MainWindowIntegrationTest)

#include "mainwindow_integration_test.moc"
