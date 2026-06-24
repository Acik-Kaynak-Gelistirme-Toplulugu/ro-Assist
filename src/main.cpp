#include "roassist/mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QLatin1String>
#include <QSettings>
#include <QStyleFactory>

namespace {

void configureQtPlatform()
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

void configureDesktopIntegration(QApplication &app)
{
    QCoreApplication::setApplicationName("ro-assist");
    QCoreApplication::setApplicationVersion(QLatin1String(APP_VERSION));
    QCoreApplication::setOrganizationName("Project-Ro-ASD");
    QCoreApplication::setOrganizationDomain("github.com/Project-Ro-ASD");
    app.setDesktopFileName("ro-assist");

#ifdef Q_OS_LINUX
    const QString desktop = qEnvironmentVariable("XDG_CURRENT_DESKTOP");
    if (desktop.contains("KDE", Qt::CaseInsensitive) &&
        QStyleFactory::keys().contains("Breeze")) {
        app.setStyle("Breeze");
    }
#endif

    QIcon icon = QIcon::fromTheme("system-software-update");
    if (icon.isNull()) {
        icon = QIcon(QStringLiteral(":/icons/ro-assist.svg"));
    }
    if (!icon.isNull()) {
        app.setWindowIcon(icon);
    }
}

bool hasCompletedWelcome()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("Project-Ro-ASD"),
                       QStringLiteral("ro-assist"));
    return settings.value(QStringLiteral("welcome/completed"),
                          settings.value(QStringLiteral("autostart/welcomeShown"), false))
        .toBool();
}

void resetWelcome()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("Project-Ro-ASD"),
                       QStringLiteral("ro-assist"));
    settings.setValue(QStringLiteral("welcome/completed"), false);
    settings.remove(QStringLiteral("autostart/welcomeShown"));
    settings.sync();
}

} // namespace

int main(int argc, char *argv[])
{
    configureQtPlatform();

    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("Fedora KDE desktop assistant for system maintenance");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption autostartOption("autostart", "Launch from the desktop autostart entry.");
    QCommandLineOption smokeTestOption("smoke-test", "Create the UI and exit immediately.");
    QCommandLineOption resetWelcomeOption(
        "reset-welcome", "Show the first-run welcome flow again on the next launch.");
    parser.addOption(autostartOption);
    parser.addOption(smokeTestOption);
    parser.addOption(resetWelcomeOption);
    parser.process(a);

    if (!parser.isSet(smokeTestOption)) {
        configureDesktopIntegration(a);
    }

    if (parser.isSet(resetWelcomeOption)) {
        resetWelcome();
        return 0;
    }

    if (parser.isSet(autostartOption) && hasCompletedWelcome()) {
        return 0;
    }

    MainWindow w;
    w.show();

    if (parser.isSet(smokeTestOption)) {
        a.processEvents();
        return 0;
    }

    return a.exec();
}
