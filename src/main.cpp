#include "roassist/mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLatin1String>
#include <QSettings>
#include <QStyleFactory>


static void configureQtPlatform()
{
#ifdef Q_OS_LINUX
    const QByteArray platform = qgetenv("QT_QPA_PLATFORM");
    if ((platform.isEmpty() || platform.contains("wayland")) &&
        !qEnvironmentVariableIsEmpty("WAYLAND_DISPLAY") &&
        !qEnvironmentVariableIsEmpty("DISPLAY")) {
        qputenv("QT_QPA_PLATFORM", "xcb");
    }

    if (qEnvironmentVariableIsEmpty("QT_XCB_GL_INTEGRATION")) {
        qputenv("QT_XCB_GL_INTEGRATION", "xcb_glx");
    }
#endif
}

static void configureDesktopIntegration(QApplication &app)
{
    Q_UNUSED(app);
    QCoreApplication::setApplicationName("ro-assist");
    QCoreApplication::setApplicationVersion(
        QLatin1String(APP_VERSION));
    QCoreApplication::setOrganizationName("Project-Ro-ASD");
    QCoreApplication::setOrganizationDomain("github.com/Project-Ro-ASD");
    QGuiApplication::setDesktopFileName("ro-assist");

#ifdef Q_OS_LINUX
    const QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    if (desktop.contains(QLatin1String("KDE"), Qt::CaseInsensitive) &&
        QStyleFactory::keys().contains(QLatin1String("Breeze"))) {
        app.setStyle("Breeze");
    }
#endif

    QIcon icon = QIcon::fromTheme("system-software-update");
    if (icon.isNull()) {
        icon = QIcon(QStringLiteral(":/icons/ro-assist.svg"));
    }
    if (!icon.isNull()) {
        QGuiApplication::setWindowIcon(icon);
    }
}

static bool hasCompletedAutostartWelcome()
{
    const QSettings settings;
    return settings.value(QStringLiteral("autostart/welcomeShown"), false).toBool();
}

static void markAutostartWelcomeCompleted()
{
    QSettings settings;
    settings.setValue(QStringLiteral("autostart/welcomeShown"), true);
    settings.sync();
}


int main(int argc, char *argv[])
{
    configureQtPlatform();

    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(
        QCoreApplication::translate("MainWindow", "Fedora KDE desktop assistant for system maintenance"));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption autostartOption("autostart",
        QCoreApplication::translate("MainWindow", "Launch from the desktop autostart entry."));
    const QCommandLineOption smokeTestOption("smoke-test",
        QCoreApplication::translate("MainWindow", "Create the UI and exit immediately."));
    parser.addOption(autostartOption);
    parser.addOption(smokeTestOption);
    parser.process(a);

    if (!parser.isSet(smokeTestOption)) {
        configureDesktopIntegration(a);
    }

    if (parser.isSet(autostartOption) && hasCompletedAutostartWelcome()) {
        return 0;
    }

    MainWindow w;
    w.show();

    if (parser.isSet(autostartOption)) {
        markAutostartWelcomeCompleted();
    }

    if (parser.isSet(smokeTestOption)) {
        QCoreApplication::processEvents();
        return 0;
    }

    return QCoreApplication::exec();
}
