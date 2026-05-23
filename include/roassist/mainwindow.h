#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QProcess>
#include <QStackedWidget>
#include <QTimer>
#include <QPlainTextEdit>
#include <QNetworkInformation>
#include <QMenu>
#include <QAction>
#include <QEvent>
#include <QTranslator>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum Theme { Light, Dark };
    enum OperationType { None, SystemUpdate, LibraryInstall };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *event) override;

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

    void nextSlide();
    void prevSlide();
    void showUpdateScreen();
    void showLibraryScreen();
    void showAppStoreScreen();
    void showCarouselScreen();

    void changeLanguage(QAction *action);
    void toggleTheme();
    void onNetworkConnectedChanged(bool isConnected);
    
    void openBozokCommunity();
    void dummyAppStoreAction();

    void toggleUpdateLogs();
    void toggleLibraryLogs();

private:
    QString langFlag(const QString &langCode) const;
    QString langName(const QString &langCode) const;
    void loadLanguage(const QString &langCode);
    void setupUi();
    void setupStyle();
    void checkDnfErrors(const QString &output);
    void createCarouselSlides();
    void updateUiTextAndImages();
    void detectTheme();
    void appendLog(const QString &text, const QString &color = "#666666");
    void resetOperationUI(const QString &statusText, bool reenableButton);
    void setInitialUpdateStatus();
    void setOperationRunning(OperationType operation);
    void clearActiveOperation();
    bool isOperationRunning() const;
    bool isLibraryOperationActive() const;

    QTranslator m_translator;
    QString m_currentLang;

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

    QStackedWidget *mainStack;
    QWidget *carouselViewWidget;
    QWidget *updateViewWidget;
    QWidget *libraryViewWidget;
    QWidget *appStoreViewWidget;

    QStackedWidget *carousel;
    QTimer *carouselTimer;

    QPlainTextEdit *logConsole;
    QPlainTextEdit *libraryLogConsole;

    QPushButton *toggleLogBtn;
    QPushButton *toggleLibraryLogBtn;

    QLabel *networkStatusLabel;
    
    QPushButton *langBtn;
    QMenu *langMenu;
    QPushButton *themeToggleBtn;
    QPushButton *prevSlideBtn;
    QPushButton *nextSlideBtn;
    QPushButton *backToCarouselBtn;
    QPushButton *backFromLibraryBtn;
    QPushButton *backFromAppStoreBtn;
    QPushButton *aboutBtn;

    QLabel *slide1Title;
    QLabel *slide1Desc;
    QPushButton *updateSlideBtn;
    
    QLabel *slide2Title;
    QToolButton *websiteBtn;
    QToolButton *roAsdGitHubBtn;
    QToolButton *roAssistGitHubBtn;
    
    QLabel *slide3Title;
    QLabel *slide3Desc;
    QPushButton *appStoreSlideBtn;
    
    QLabel *slide4Title;
    QLabel *slide4Desc;
    QPushButton *bozokBtn;
    
    QLabel *slide5Title;
    QLabel *slide5Desc;
    QPushButton *libraryPackageSlideBtn;

    QLabel *appStoreTitleLabel;
    QLabel *appStorePlaceholderIcon;
    QPushButton *appStoreOpenAppBtn;

    Theme currentTheme;
    OperationType activeOperation;
    bool transactionPhaseStarted;
    bool isTerminatingIntentionally;
    bool isNetworkConnected;
    bool isLibraryInstalled;
};

#endif // MAINWINDOW_H
