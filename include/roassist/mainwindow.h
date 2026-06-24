#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QProcess>
#include <QStackedWidget>
#include <QTextEdit>
#include <QNetworkInformation>
#include <QMenu>
#include <QAction>
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

private:
    void setupUi();
    void setupStyle();
    void checkDnfErrors(const QString &output);
    void createWelcomeSlides();
    void createDashboard();
    void updateUiTextAndImages();
    void detectSystemLanguageAndTheme();
    void appendLog(const QString &text, const QString &color = "#666666");
    void appendPrinterLog(const QString &text, const QString &color = "#666666");
    void setInitialUpdateStatus();
    void setOperationRunning(OperationType operation);
    void clearActiveOperation();
    bool isOperationRunning() const;
    bool isLibraryOperationActive() const;
    bool isPrinterSupportOperationActive() const;
    QString currentLanguageCode() const;

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    QLabel *versionLabel;
    QLabel *statusLabel;
    QLabel *libraryStatusLabel;
    
    QPushButton *updateButton;
    QPushButton *libraryInstallButton;
    
    QProgressBar *progressBar;
    QProgressBar *libraryProgressBar;
    
    QProcess *updateProcess;
    QProcess *checkUpdateProcess;
    QProcess *checkLibProcess;
    QProcess *printerSupportProcess;

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

    QLabel *welcomeProgressLabel;
    QLabel *dashboardGreetingLabel;
    QLabel *dashboardDescriptionLabel;
    QPushButton *dashboardUpdateCard;
    QPushButton *dashboardSocialCard;
    QPushButton *dashboardStoreCard;
    QPushButton *dashboardCommunityCard;
    QPushButton *dashboardLibraryCard;
    QPushButton *dashboardPrinterCard;

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

    // App Store View special elements
    QLabel *appStoreTitleLabel;
    QLabel *appStorePlaceholderIcon;
    QPushButton *appStoreOpenAppBtn;

    // State
    Language currentLang;
    Theme currentTheme;
    OperationType activeOperation;
    bool transactionPhaseStarted;
    bool isTerminatingIntentionally;
    bool isNetworkConnected;
    bool isLibraryInstalled;
    bool isPrinterSupportInstalled;
    bool welcomeCompleted;
};

#endif // MAINWINDOW_H
