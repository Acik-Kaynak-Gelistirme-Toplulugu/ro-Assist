#include "roassist/mainwindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QIcon>
#include <QSettings>
#include <QStyleFactory>
#include <QTimer>

namespace {

void configureDesktopIntegration(QApplication &app)
{
    QCoreApplication::setApplicationName("ro-assist");
    QCoreApplication::setApplicationVersion("0.1.0");
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

bool hasCompletedAutostartWelcome()
{
    QSettings settings;
    return settings.value(QStringLiteral("autostart/welcomeShown"), false).toBool();
}

void markAutostartWelcomeCompleted()
{
    QSettings settings;
    settings.setValue(QStringLiteral("autostart/welcomeShown"), true);
    settings.sync();
}

} // namespace

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    configureDesktopIntegration(a);

    QCommandLineParser parser;
    parser.setApplicationDescription("Fedora KDE desktop assistant for system maintenance");
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption autostartOption("autostart", "Launch from the desktop autostart entry.");
    QCommandLineOption smokeTestOption("smoke-test", "Create the UI and exit immediately.");
    parser.addOption(autostartOption);
    parser.addOption(smokeTestOption);
    parser.process(a);

    if (parser.isSet(autostartOption) && hasCompletedAutostartWelcome()) {
        return 0;
    }

    MainWindow w;
    w.show();

    if (parser.isSet(autostartOption)) {
        markAutostartWelcomeCompleted();
    }

    if (parser.isSet(smokeTestOption)) {
        QTimer::singleShot(0, &a, &QCoreApplication::quit);
    }

    return a.exec();
}
