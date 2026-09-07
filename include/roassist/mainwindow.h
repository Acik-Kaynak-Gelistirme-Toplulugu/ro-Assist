#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "roassist/system_services.h"

#include <QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QProcess>
#include <QSlider>
#include <QStackedWidget>
#include <QTextEdit>
#include <QNetworkInformation>
#include <QMenu>
#include <QAction>
#include <QFutureWatcher>
#include <QString>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum Language { TR, EN, ES, DE, FR };
    enum Theme { Light, Dark };
    enum OperationType { None, SystemUpdate, LibraryInstall, PrinterSupportInstall };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openWebsite();
    void openRoAsdGitHub();
    void openRoAssistGitHub();
    void showAboutDialog();
    
    void startUpdate();
    void handleUpdateOutput();
    void handleUpdateErrorOutput();
    void handleUpdateFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleUpdateProcessError(QProcess::ProcessError error);

    void handleCheckUpdateFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleCheckLibFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void startLibraryPackageInstall();
    void startPrinterSupportInstall();
    void choosePrinterSupportLater();
    void disablePrinterSupport();
    void openPrinterSettings();
    void openScannerApplication();
    void openRoControl();
    void handlePrinterSupportOutput();
    void handlePrinterSupportErrorOutput();
    void handlePrinterSupportFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handlePrinterSupportProcessError(QProcess::ProcessError error);

    void advanceWelcome();
    void showUpdateScreen();
    void showLibraryScreen();
    void showAppStoreScreen();
    void showSocialScreen();
    void showCommunityScreen();
    void showPrinterSupportScreen();
    void showTelemetryScreen();
    void showWelcomeScreen();
    void showDashboardScreen();
    void showHomeScreen();

    void changeLanguage(QAction *action);
    void changeLanguageAction(QAction *action);
    void toggleTheme();
    void onNetworkConnectedChanged(bool isConnected);
    
    void openBozokCommunity();
    void dummyAppStoreAction();

    void toggleUpdateLogs();
    void toggleLibraryLogs();
    void setTelemetryLevel(int level);

private:
    void setupUi();
    void setupStyle();
    void checkDnfErrors(const QString &output);
    void createWelcomeSlides();
    void createDashboard();
    void updateUiTextAndImages();
    void detectSystemLanguageAndTheme();
    void applySystemLanguage();
    void appendLog(const QString &text, const QString &color = "#666666");
    void appendPrinterLog(const QString &text, const QString &color = "#666666");
    void refreshMaintenanceStatus();
    void applyMaintenanceStatus();
    void setInitialUpdateStatus();
    void setOperationRunning(OperationType operation);
    void clearActiveOperation();
    void startNextSystemUpdateStep();
    void finishSystemUpdateWorkflow();
    void handleSystemUpdateStepFinished(int exitCode,
                                        QProcess::ExitStatus exitStatus);
    void preparePackageProcessEnvironment();
    bool isOperationRunning() const;
    bool isLibraryOperationActive() const;
    bool isPrinterSupportOperationActive() const;
    QString currentLanguageCode() const;

protected:
    void changeEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *versionLabel;
    QLabel *statusLabel;
    QLabel *updateRiskLabel;
    QLabel *updatePlanLabel;
    QLabel *libraryStatusLabel;
    
    QPushButton *updateButton;
    QPushButton *libraryInstallButton;
    QPushButton *openRoControlButton;
    
    QProgressBar *progressBar;
    QProgressBar *libraryProgressBar;
    
    QProcess *updateProcess;
    QProcess *checkUpdateProcess;
    QProcess *checkLibProcess;
    QProcess *printerSupportProcess;
    QFutureWatcher<RoAssist::SystemRiskSnapshot> *riskWatcher;
    bool riskSnapshotReady = false;

    // Main layout
    QStackedWidget *mainStack;
    QWidget *welcomeViewWidget;
    QWidget *dashboardViewWidget;
    QWidget *updateViewWidget;
    QWidget *libraryViewWidget;
    QWidget *appStoreViewWidget;
    QWidget *socialViewWidget;
    QWidget *communityViewWidget;
    QWidget *printerSupportViewWidget;
    QWidget *telemetryViewWidget;

    QStackedWidget *welcomeStack;

    QTextEdit *logConsole;
    QTextEdit *libraryLogConsole;
    QTextEdit *printerLogConsole;

    QPushButton *toggleLogBtn;
    QPushButton *toggleLibraryLogBtn;

    QLabel *networkStatusLabel;
    
    QPushButton *langBtn;
    QMenu *langMenu;
    QPushButton *themeToggleBtn;
    QPushButton *welcomeNextBtn;
    QPushButton *backToCarouselBtn;
    QPushButton *backFromLibraryBtn;
    QPushButton *backFromAppStoreBtn;
    QPushButton *backFromSocialBtn;
    QPushButton *backFromCommunityBtn;
    QPushButton *backFromTelemetryBtn;

    QLabel *welcomeProgressLabel;
    QLabel *dashboardGreetingLabel;
    QLabel *dashboardDescriptionLabel;
    QLabel *dashboardStatusLabel;
    QPushButton *dashboardUpdateCard;
    QPushButton *dashboardSocialCard;
    QPushButton *dashboardStoreCard;
    QPushButton *dashboardCommunityCard;
    QPushButton *dashboardLibraryCard;
    QPushButton *dashboardPrinterCard;
    QPushButton *dashboardTelemetryCard;

    // Slide specific elements
    QLabel *slide1Title;
    QLabel *slide1Desc;
    
    QLabel *slide2Title;
    QLabel *slide2Desc;
    
    QLabel *slide3Title;
    QLabel *slide3Desc;
    
    QLabel *slide4Title;
    QLabel *slide4Desc;
    
    QLabel *slide5Title;
    QLabel *slide5Desc;

    QLabel *slide6Title;
    QLabel *slide6Desc;

    QLabel *socialTitleLabel;
    QLabel *socialDescriptionLabel;
    QToolButton *websiteBtn;
    QToolButton *roAsdGitHubBtn;
    QToolButton *roAssistGitHubBtn;
    QLabel *communityTitleLabel;
    QLabel *communityDescriptionLabel;
    QPushButton *bozokBtn;

    QPushButton *backFromPrinterSupportBtn;
    QLabel *printerSupportTitleLabel;
    QLabel *printerSupportDescriptionLabel;
    QLabel *printerSupportBenefitsLabel;
    QLabel *printerSupportStatusLabel;
    QPushButton *printerSupportInstallButton;
    QPushButton *printerSupportLaterButton;
    QPushButton *printerSupportDisableButton;
    QPushButton *openPrinterSettingsButton;
    QPushButton *openScannerButton;
    QProgressBar *printerSupportProgressBar;

    QLabel *telemetryTitleLabel;
    QLabel *telemetryIntroLabel;
    QLabel *telemetryPurposeTitleLabel;
    QLabel *telemetryPurposeTextLabel;
    QLabel *telemetryLevelsTitleLabel;
    QLabel *telemetryLevelsTextLabel;
    QLabel *telemetryDoesNotCollectTitleLabel;
    QLabel *telemetryDoesNotCollectTextLabel;
    QLabel *telemetrySliderTitleLabel;
    QLabel *telemetryCurrentLevelLabel;
    QLabel *telemetryLevelDescriptionLabel;
    QLabel *telemetryOffLabel;
    QLabel *telemetryCountLabel;
    QLabel *telemetryBasicLabel;
    QLabel *telemetryExtendedLabel;
    QSlider *telemetryLevelSlider;

    // App Store View special elements
    QLabel *appStoreTitleLabel;
    QLabel *appStorePlaceholderIcon;
    QPushButton *appStoreOpenAppBtn;

    // State
    Language currentLang;
    Theme currentTheme;
    OperationType activeOperation;
    QVector<RoAssist::ProcessCommand> systemUpdateCommands;
    int currentSystemUpdateStep;
    bool transactionPhaseStarted;
    bool isTerminatingIntentionally;
    bool systemUpdateHadFailures;
    bool isNetworkConnected;
    bool isLibraryInstalled;
    bool isPrinterSupportInstalled;
    bool welcomeCompleted;
    RoAssist::SystemRiskSnapshot lastRiskSnapshot;
};

#endif // MAINWINDOW_H
