#include "roassist/mainwindow.h"
#include "roassist/ui_texts.h"
#include "roassist/update_helpers.h"

#include <QDesktopServices>
#include <QDialog>
#include <QEvent>
#include <QFile>
#include <QGridLayout>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLocale>
#include <QMessageBox>
#include <QPalette>
#include <QScreen>
#include <QScrollBar>
#include <QSettings>
#include <QStandardPaths>
#include <QStyleHints>
#include <QUrl>
#include <QVBoxLayout>

namespace {

QString localeValueFromFile(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return {};

  while (!file.atEnd()) {
    const QString line = QString::fromUtf8(file.readLine()).trimmed();
    if (line.startsWith(QStringLiteral("LANGUAGE=")) ||
        line.startsWith(QStringLiteral("LANG="))) {
      QString value = line.section(QLatin1Char('='), 1).trimmed();
      value.remove(QLatin1Char('"'));
      value.remove(QLatin1Char('\''));
      return value.section(QLatin1Char(':'), 0, 0);
    }
  }
  return {};
}

QString desktopLocaleName() {
  for (const char *variable : {"LANGUAGE", "LC_ALL", "LC_MESSAGES", "LANG"}) {
    const QString value = qEnvironmentVariable(variable).trimmed();
    if (!value.isEmpty() && !value.startsWith(QStringLiteral("C")) &&
        !value.startsWith(QStringLiteral("POSIX"))) {
      return value.section(QLatin1Char(':'), 0, 0);
    }
  }

  const QLocale locale = QLocale::system();
  if (locale.language() != QLocale::C)
    return locale.name();

  const QString configDirectory =
      QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
  for (const QString &path :
       {configDirectory + QStringLiteral("/plasma-localerc"),
        configDirectory + QStringLiteral("/kdeglobals"),
        QStringLiteral("/etc/locale.conf")}) {
    const QString value = localeValueFromFile(path);
    if (!value.isEmpty())
      return value;
  }
  return {};
}

} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), updateProcess(new QProcess(this)),
      checkUpdateProcess(new QProcess(this)),
      checkLibProcess(new QProcess(this)), printerSupportProcess(new QProcess(this)),
      welcomeStack(new QStackedWidget(this)),
      logConsole(new QTextEdit(this)),
      libraryLogConsole(new QTextEdit(this)), printerLogConsole(new QTextEdit(this)),
      activeOperation(None),
      transactionPhaseStarted(false), isTerminatingIntentionally(false),
      isNetworkConnected(true), isLibraryInstalled(false),
      isPrinterSupportInstalled(false), welcomeCompleted(false) {
  detectSystemLanguageAndTheme();

  QNetworkInformation::loadBackendByFeatures(
      QNetworkInformation::Feature::Reachability);
  if (QNetworkInformation::instance()) {
    isNetworkConnected = QNetworkInformation::instance()->reachability() ==
                         QNetworkInformation::Reachability::Online;
    connect(QNetworkInformation::instance(),
            &QNetworkInformation::reachabilityChanged, this,
            [this](QNetworkInformation::Reachability r) {
              onNetworkConnectedChanged(
                  r == QNetworkInformation::Reachability::Online);
            });
  }

  setupUi();
  updateUiTextAndImages();
  setupStyle();

  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  welcomeCompleted = settings
                         .value(QStringLiteral("welcome/completed"),
                                settings.value(QStringLiteral("autostart/welcomeShown"),
                                               false))
                         .toBool();
  if (welcomeCompleted)
    showDashboardScreen();
  else
    showWelcomeScreen();

  logConsole->hide();
  libraryLogConsole->hide();
  printerLogConsole->hide();
  if (QScreen *screen = QGuiApplication::primaryScreen()) {
    QRect screenGeometry = screen->geometry();
    int width = screenGeometry.width() * 0.66;
    int height = screenGeometry.height() * 0.66;
    resize(width, height);
  }

  connect(updateProcess, &QProcess::readyReadStandardOutput, this,
          &MainWindow::handleUpdateOutput);
  connect(updateProcess, &QProcess::readyReadStandardError, this,
          &MainWindow::handleUpdateErrorOutput);
  connect(updateProcess, &QProcess::finished, this,
          &MainWindow::handleUpdateFinished);
  connect(updateProcess, &QProcess::errorOccurred, this,
          &MainWindow::handleUpdateProcessError);

  connect(checkUpdateProcess, &QProcess::finished, this,
          &MainWindow::handleCheckUpdateFinished);
  connect(checkLibProcess, &QProcess::finished, this,
          &MainWindow::handleCheckLibFinished);
  connect(printerSupportProcess, &QProcess::readyReadStandardOutput, this,
          &MainWindow::handlePrinterSupportOutput);
  connect(printerSupportProcess, &QProcess::readyReadStandardError, this,
          &MainWindow::handlePrinterSupportErrorOutput);
  connect(printerSupportProcess, &QProcess::finished, this,
          &MainWindow::handlePrinterSupportFinished);
  connect(printerSupportProcess, &QProcess::errorOccurred, this,
          &MainWindow::handlePrinterSupportProcessError);

  setInitialUpdateStatus();
  if (qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS")) {
    checkUpdateProcess->start("dnf", QStringList() << "check-update");
  }
}

MainWindow::~MainWindow() {
  if (updateProcess && updateProcess->state() != QProcess::NotRunning) {
    updateProcess->terminate();
    if (!updateProcess->waitForFinished(1000)) {
      updateProcess->kill();
      updateProcess->waitForFinished(1000);
    }
  }
  if (checkUpdateProcess &&
      checkUpdateProcess->state() != QProcess::NotRunning) {
    checkUpdateProcess->kill();
    checkUpdateProcess->waitForFinished(1000);
  }
  if (checkLibProcess && checkLibProcess->state() != QProcess::NotRunning) {
    checkLibProcess->kill();
    checkLibProcess->waitForFinished(1000);
  }
  if (printerSupportProcess &&
      printerSupportProcess->state() != QProcess::NotRunning) {
    printerSupportProcess->terminate();
    if (!printerSupportProcess->waitForFinished(1000)) {
      printerSupportProcess->kill();
      printerSupportProcess->waitForFinished(1000);
    }
  }
}

void MainWindow::setOperationRunning(OperationType operation) {
  activeOperation = operation;
  transactionPhaseStarted = false;
  isTerminatingIntentionally = false;
}

void MainWindow::clearActiveOperation() {
  activeOperation = None;
  transactionPhaseStarted = false;
  isTerminatingIntentionally = false;
}

bool MainWindow::isOperationRunning() const {
  return activeOperation != None ||
         updateProcess->state() != QProcess::NotRunning ||
         printerSupportProcess->state() != QProcess::NotRunning;
}

bool MainWindow::isLibraryOperationActive() const {
  return activeOperation == LibraryInstall;
}

bool MainWindow::isPrinterSupportOperationActive() const {
  return activeOperation == PrinterSupportInstall;
}

QString MainWindow::currentLanguageCode() const {
  switch (currentLang) {
  case TR:
    return QStringLiteral("tr");
  case ES:
    return QStringLiteral("es");
  case DE:
    return QStringLiteral("de");
  case FR:
    return QStringLiteral("fr");
  case EN:
  default:
    return QStringLiteral("en");
  }
}

void MainWindow::detectSystemLanguageAndTheme() {
  const QString localeName = desktopLocaleName().toLower();
  if (localeName.startsWith(QStringLiteral("tr")))
    currentLang = TR;
  else if (localeName.startsWith(QStringLiteral("es")))
    currentLang = ES;
  else if (localeName.startsWith(QStringLiteral("de")))
    currentLang = DE;
  else if (localeName.startsWith(QStringLiteral("fr")))
    currentLang = FR;
  else
    currentLang = EN;

  currentTheme = Light;

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  if (const QStyleHints *hints = QGuiApplication::styleHints()) {
    if (hints->colorScheme() == Qt::ColorScheme::Dark)
      currentTheme = Dark;
  }
#else
  // Fallback for older Qt versions (Qt < 6.5)
  if (QGuiApplication::palette().color(QPalette::WindowText).lightness() >
      QGuiApplication::palette().color(QPalette::Window).lightness()) {
    currentTheme = Dark;
  }
#endif
}

void MainWindow::setupUi() {
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  // TOP BAR
  QWidget *topBarWidget = new QWidget(this);
  QHBoxLayout *topLayout = new QHBoxLayout(topBarWidget);
  topLayout->setContentsMargins(20, 20, 20, 10);
  networkStatusLabel = new QLabel(this);
  networkStatusLabel->setObjectName("networkStatusLabel");
  networkStatusLabel->setStyleSheet(
      "font-weight: bold; color: #ffcc00; font-size: 14px;");
  networkStatusLabel->setVisible(!isNetworkConnected);

  topLayout->addWidget(networkStatusLabel);
  topLayout->addStretch();
  themeToggleBtn = new QPushButton(this);

  langBtn = new QPushButton(this);
  langBtn->setObjectName("languageButton");
  langBtn->setCursor(Qt::PointingHandCursor);
  langBtn->setFixedSize(130, 42);

  langMenu = new QMenu(langBtn);
  QAction *actTR = langMenu->addAction("🇹🇷 Türkçe");
  actTR->setData(QVariant::fromValue((int)TR));
  QAction *actEN = langMenu->addAction("🇬🇧 English");
  actEN->setData(QVariant::fromValue((int)EN));
  QAction *actES = langMenu->addAction("🇪🇸 Español");
  actES->setData(QVariant::fromValue((int)ES));
  QAction *actDE = langMenu->addAction("🇩🇪 Deutsch");
  actDE->setData(QVariant::fromValue((int)DE));
  QAction *actFR = langMenu->addAction("🇫🇷 Français");
  actFR->setData(QVariant::fromValue((int)FR));

  langBtn->setMenu(langMenu);

  themeToggleBtn->setFixedSize(110, 42);
  connect(themeToggleBtn, &QPushButton::clicked, this,
          &MainWindow::toggleTheme);
  connect(langMenu, &QMenu::triggered, this, &MainWindow::changeLanguageAction);

  topLayout->addWidget(themeToggleBtn);
  topLayout->addWidget(langBtn);
  mainLayout->addWidget(topBarWidget);

  mainStack = new QStackedWidget(this);
  mainStack->setObjectName("mainStack");

  // 1. FIRST-RUN WELCOME FLOW
  welcomeViewWidget = new QWidget(this);
  QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomeViewWidget);
  welcomeLayout->setContentsMargins(36, 12, 36, 26);
  welcomeLayout->setSpacing(12);
  welcomeStack->setObjectName("welcomeStack");
  createWelcomeSlides();

  welcomeProgressLabel = new QLabel(this);
  welcomeProgressLabel->setObjectName("welcomeProgressLabel");
  welcomeProgressLabel->setAlignment(Qt::AlignCenter);

  welcomeNextBtn = new QPushButton(this);
  welcomeNextBtn->setObjectName("welcomeNextButton");
  welcomeNextBtn->setMinimumSize(280, 62);
  welcomeNextBtn->setCursor(Qt::PointingHandCursor);
  connect(welcomeNextBtn, &QPushButton::clicked, this,
          &MainWindow::advanceWelcome);

  welcomeLayout->addWidget(welcomeStack, 1);
  welcomeLayout->addWidget(welcomeProgressLabel, 0, Qt::AlignCenter);
  welcomeLayout->addWidget(welcomeNextBtn, 0, Qt::AlignCenter);

  // 2. UPDATE VIEW
  updateViewWidget = new QWidget(this);
  QVBoxLayout *updateLayout = new QVBoxLayout(updateViewWidget);
  updateLayout->setContentsMargins(20, 0, 20, 0);
  backToCarouselBtn = new QPushButton(this);
  backToCarouselBtn->setObjectName("backToHomeButton");
  backToCarouselBtn->setMinimumSize(120, 40);
  backToCarouselBtn->setCursor(Qt::PointingHandCursor);
  connect(backToCarouselBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);

  QHBoxLayout *updateTopLayout = new QHBoxLayout();
  updateTopLayout->addWidget(backToCarouselBtn);
  updateTopLayout->addStretch();

  QWidget *updatePanel = new QWidget(this);
  updatePanel->setObjectName("panelWidget");
  QVBoxLayout *panelLayout = new QVBoxLayout(updatePanel);
  panelLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  versionLabel = new QLabel("0.1.1", this);
  versionLabel->setAlignment(Qt::AlignCenter);
  versionLabel->setObjectName("versionLabel");
  statusLabel = new QLabel("", this);
  statusLabel->setAlignment(Qt::AlignCenter);
  statusLabel->setObjectName("statusLabel");
  statusLabel->setWordWrap(true);
  statusLabel->setMinimumWidth(400);

  updateButton = new QPushButton(this);
  updateButton->setObjectName("systemUpdateButton");
  updateButton->setFixedSize(320, 70);
  updateButton->setCursor(Qt::PointingHandCursor);
  connect(updateButton, &QPushButton::clicked, this, &MainWindow::startUpdate);

  progressBar = new QProgressBar(this);
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->hide();
  progressBar->setFixedWidth(400);

  toggleLogBtn = new QPushButton(this);
  toggleLogBtn->setObjectName("backButton");
  toggleLogBtn->setCursor(Qt::PointingHandCursor);
  connect(toggleLogBtn, &QPushButton::clicked, this,
          &MainWindow::toggleUpdateLogs);

  logConsole->setReadOnly(true);
  logConsole->setObjectName("logConsole");
  logConsole->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  logConsole->setMaximumHeight(150);
  logConsole->setMinimumHeight(100);

  panelLayout->addWidget(versionLabel, 0, Qt::AlignCenter);
  panelLayout->addSpacing(10);
  panelLayout->addWidget(statusLabel, 0, Qt::AlignCenter);
  panelLayout->addSpacing(20);
  panelLayout->addWidget(updateButton, 0, Qt::AlignCenter);
  panelLayout->addSpacing(20);
  panelLayout->addWidget(progressBar, 0, Qt::AlignCenter);
  panelLayout->addSpacing(10);
  panelLayout->addWidget(toggleLogBtn, 0, Qt::AlignCenter);
  panelLayout->addSpacing(5);
  panelLayout->addWidget(logConsole, 1);

  updateLayout->addLayout(updateTopLayout);
  updateLayout->addSpacing(10);
  updateLayout->addWidget(updatePanel, 1);

  // 3. LIBRARY PACKAGE VIEW
  libraryViewWidget = new QWidget(this);
  QVBoxLayout *libraryLayout = new QVBoxLayout(libraryViewWidget);
  libraryLayout->setContentsMargins(20, 0, 20, 0);

  backFromLibraryBtn = new QPushButton(this);
  backFromLibraryBtn->setObjectName("backButton");
  backFromLibraryBtn->setMinimumSize(120, 40);
  backFromLibraryBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromLibraryBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);

  QHBoxLayout *libraryTopLayout = new QHBoxLayout();
  libraryTopLayout->addWidget(backFromLibraryBtn);
  libraryTopLayout->addStretch();

  QWidget *libraryPanel = new QWidget(this);
  libraryPanel->setObjectName("panelWidget");
  QVBoxLayout *libraryPanelLayout = new QVBoxLayout(libraryPanel);
  libraryPanelLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  libraryStatusLabel = new QLabel("", this);
  libraryStatusLabel->setAlignment(Qt::AlignCenter);
  libraryStatusLabel->setObjectName("statusLabel");
  libraryStatusLabel->setWordWrap(true);
  libraryStatusLabel->setMinimumWidth(400);

  libraryInstallButton = new QPushButton(this);
  libraryInstallButton->setObjectName("libraryInstallButton");
  libraryInstallButton->setFixedSize(320, 70);
  libraryInstallButton->setCursor(Qt::PointingHandCursor);
  connect(libraryInstallButton, &QPushButton::clicked, this,
          &MainWindow::startLibraryPackageInstall);

  libraryProgressBar = new QProgressBar(this);
  libraryProgressBar->setRange(0, 100);
  libraryProgressBar->setValue(0);
  libraryProgressBar->hide();
  libraryProgressBar->setFixedWidth(400);

  toggleLibraryLogBtn = new QPushButton(this);
  toggleLibraryLogBtn->setObjectName("backButton");
  toggleLibraryLogBtn->setCursor(Qt::PointingHandCursor);
  connect(toggleLibraryLogBtn, &QPushButton::clicked, this,
          &MainWindow::toggleLibraryLogs);

  libraryLogConsole->setReadOnly(true);
  libraryLogConsole->setObjectName("logConsole");
  libraryLogConsole->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  libraryLogConsole->setMaximumHeight(150);
  libraryLogConsole->setMinimumHeight(100);

  libraryPanelLayout->addSpacing(40);
  libraryPanelLayout->addWidget(libraryStatusLabel, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(20);
  libraryPanelLayout->addWidget(libraryInstallButton, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(20);
  libraryPanelLayout->addWidget(libraryProgressBar, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(10);
  libraryPanelLayout->addWidget(toggleLibraryLogBtn, 0, Qt::AlignCenter);
  libraryPanelLayout->addSpacing(5);
  libraryPanelLayout->addWidget(libraryLogConsole, 1);

  libraryLayout->addLayout(libraryTopLayout);
  libraryLayout->addSpacing(10);
  libraryLayout->addWidget(libraryPanel, 1);

  // 4. CUSTOM APP STORE VIEW
  appStoreViewWidget = new QWidget(this);
  QVBoxLayout *storeLayout = new QVBoxLayout(appStoreViewWidget);
  storeLayout->setContentsMargins(20, 0, 20, 0);

  backFromAppStoreBtn = new QPushButton(this);
  backFromAppStoreBtn->setObjectName("backButton");
  backFromAppStoreBtn->setMinimumSize(120, 40);
  backFromAppStoreBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromAppStoreBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);

  QHBoxLayout *storeTopLayout = new QHBoxLayout();
  storeTopLayout->addWidget(backFromAppStoreBtn);
  storeTopLayout->addStretch();

  QWidget *storePanel = new QWidget(this);
  storePanel->setObjectName("panelWidget");
  QVBoxLayout *storePanelLayout = new QVBoxLayout(storePanel);
  storePanelLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  appStoreTitleLabel = new QLabel(this);
  appStoreTitleLabel->setObjectName("slideTitle");
  appStoreTitleLabel->setAlignment(Qt::AlignCenter);
  appStorePlaceholderIcon = new QLabel("🛍️", this);
  appStorePlaceholderIcon->setAlignment(Qt::AlignCenter);
  appStorePlaceholderIcon->setStyleSheet(
      "font-size: 150px; background-color: transparent; border: none;");
  appStoreOpenAppBtn = new QPushButton(this);
  appStoreOpenAppBtn->setObjectName("actionButton");
  appStoreOpenAppBtn->setMinimumSize(320, 70);
  appStoreOpenAppBtn->setCursor(Qt::PointingHandCursor);
  connect(appStoreOpenAppBtn, &QPushButton::clicked, this,
          &MainWindow::dummyAppStoreAction);

  storePanelLayout->addStretch();
  storePanelLayout->addWidget(appStoreTitleLabel, 0, Qt::AlignCenter);
  storePanelLayout->addSpacing(40);
  storePanelLayout->addWidget(appStorePlaceholderIcon, 0, Qt::AlignCenter);
  storePanelLayout->addSpacing(40);
  storePanelLayout->addWidget(appStoreOpenAppBtn, 0, Qt::AlignCenter);
  storePanelLayout->addStretch();

  storeLayout->addLayout(storeTopLayout);
  storeLayout->addSpacing(10);
  storeLayout->addWidget(storePanel, 1);
  createDashboard();

  // 5. SOCIAL LINKS VIEW
  socialViewWidget = new QWidget(this);
  QVBoxLayout *socialViewLayout = new QVBoxLayout(socialViewWidget);
  socialViewLayout->setContentsMargins(20, 0, 20, 0);
  backFromSocialBtn = new QPushButton(this);
  backFromSocialBtn->setObjectName("backButton");
  backFromSocialBtn->setMinimumSize(120, 40);
  backFromSocialBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromSocialBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);
  QHBoxLayout *socialTopLayout = new QHBoxLayout();
  socialTopLayout->addWidget(backFromSocialBtn);
  socialTopLayout->addStretch();

  QWidget *socialPanel = new QWidget(this);
  socialPanel->setObjectName("panelWidget");
  QVBoxLayout *socialPanelLayout = new QVBoxLayout(socialPanel);
  socialPanelLayout->setAlignment(Qt::AlignCenter);
  socialTitleLabel = new QLabel(this);
  socialTitleLabel->setObjectName("slideTitle");
  socialTitleLabel->setAlignment(Qt::AlignCenter);
  socialTitleLabel->setWordWrap(true);
  socialDescriptionLabel = new QLabel(this);
  socialDescriptionLabel->setObjectName("slideDesc");
  socialDescriptionLabel->setAlignment(Qt::AlignCenter);
  socialDescriptionLabel->setWordWrap(true);

  QHBoxLayout *socialLayout = new QHBoxLayout();
  socialLayout->setSpacing(20);
  websiteBtn = new QToolButton(this);
  roAsdGitHubBtn = new QToolButton(this);
  roAssistGitHubBtn = new QToolButton(this);
  for (QToolButton *button : {websiteBtn, roAsdGitHubBtn, roAssistGitHubBtn}) {
    button->setObjectName("squareSoftButton");
    button->setFixedSize(160, 160);
    button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    button->setIconSize(QSize(64, 64));
    button->setCursor(Qt::PointingHandCursor);
  }
  websiteBtn->setIcon(QIcon(":/icons/globe.svg"));
  roAsdGitHubBtn->setIcon(QIcon(":/icons/github.svg"));
  roAssistGitHubBtn->setIcon(QIcon(":/icons/github.svg"));
  connect(websiteBtn, &QToolButton::clicked, this, &MainWindow::openWebsite);
  connect(roAsdGitHubBtn, &QToolButton::clicked, this,
          &MainWindow::openRoAsdGitHub);
  connect(roAssistGitHubBtn, &QToolButton::clicked, this,
          &MainWindow::openRoAssistGitHub);
  socialLayout->addStretch();
  socialLayout->addWidget(websiteBtn);
  socialLayout->addWidget(roAsdGitHubBtn);
  socialLayout->addWidget(roAssistGitHubBtn);
  socialLayout->addStretch();
  socialPanelLayout->addStretch();
  socialPanelLayout->addWidget(socialTitleLabel);
  socialPanelLayout->addSpacing(16);
  socialPanelLayout->addWidget(socialDescriptionLabel);
  socialPanelLayout->addSpacing(30);
  socialPanelLayout->addLayout(socialLayout);
  socialPanelLayout->addStretch();
  socialViewLayout->addLayout(socialTopLayout);
  socialViewLayout->addWidget(socialPanel, 1);

  // 6. COMMUNITY VIEW
  communityViewWidget = new QWidget(this);
  QVBoxLayout *communityLayout = new QVBoxLayout(communityViewWidget);
  communityLayout->setContentsMargins(20, 0, 20, 0);
  backFromCommunityBtn = new QPushButton(this);
  backFromCommunityBtn->setObjectName("backButton");
  backFromCommunityBtn->setMinimumSize(120, 40);
  backFromCommunityBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromCommunityBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);
  QHBoxLayout *communityTopLayout = new QHBoxLayout();
  communityTopLayout->addWidget(backFromCommunityBtn);
  communityTopLayout->addStretch();

  QWidget *communityPanel = new QWidget(this);
  communityPanel->setObjectName("panelWidget");
  QVBoxLayout *communityPanelLayout = new QVBoxLayout(communityPanel);
  communityPanelLayout->setAlignment(Qt::AlignCenter);
  communityTitleLabel = new QLabel(this);
  communityTitleLabel->setObjectName("slideTitle");
  communityTitleLabel->setAlignment(Qt::AlignCenter);
  communityTitleLabel->setWordWrap(true);
  communityDescriptionLabel = new QLabel(this);
  communityDescriptionLabel->setObjectName("slideDesc");
  communityDescriptionLabel->setAlignment(Qt::AlignCenter);
  communityDescriptionLabel->setWordWrap(true);
  bozokBtn = new QPushButton(this);
  bozokBtn->setObjectName("actionButton");
  bozokBtn->setMinimumSize(280, 70);
  bozokBtn->setCursor(Qt::PointingHandCursor);
  connect(bozokBtn, &QPushButton::clicked, this,
          &MainWindow::openBozokCommunity);
  communityPanelLayout->addStretch();
  communityPanelLayout->addWidget(communityTitleLabel);
  communityPanelLayout->addSpacing(20);
  communityPanelLayout->addWidget(communityDescriptionLabel);
  communityPanelLayout->addSpacing(40);
  communityPanelLayout->addWidget(bozokBtn, 0, Qt::AlignCenter);
  communityPanelLayout->addStretch();
  communityLayout->addLayout(communityTopLayout);
  communityLayout->addWidget(communityPanel, 1);

  // 7. PRINTER AND SCANNER SUPPORT VIEW
  printerSupportViewWidget = new QWidget(this);
  printerSupportViewWidget->setObjectName("printerSupportView");
  QVBoxLayout *printerLayout = new QVBoxLayout(printerSupportViewWidget);
  printerLayout->setContentsMargins(20, 0, 20, 0);
  backFromPrinterSupportBtn = new QPushButton(this);
  backFromPrinterSupportBtn->setObjectName("backButton");
  backFromPrinterSupportBtn->setMinimumSize(120, 40);
  backFromPrinterSupportBtn->setCursor(Qt::PointingHandCursor);
  connect(backFromPrinterSupportBtn, &QPushButton::clicked, this,
          &MainWindow::showHomeScreen);
  QHBoxLayout *printerTopLayout = new QHBoxLayout();
  printerTopLayout->addWidget(backFromPrinterSupportBtn);
  printerTopLayout->addStretch();

  QWidget *printerPanel = new QWidget(this);
  printerPanel->setObjectName("panelWidget");
  QVBoxLayout *printerPanelLayout = new QVBoxLayout(printerPanel);
  printerPanelLayout->setContentsMargins(40, 24, 40, 30);
  printerPanelLayout->setSpacing(22);
  printerSupportTitleLabel = new QLabel(this);
  printerSupportTitleLabel->setObjectName("printerTitle");
  printerSupportTitleLabel->setAlignment(Qt::AlignLeft);
  printerSupportTitleLabel->setWordWrap(true);
  printerSupportDescriptionLabel = new QLabel(this);
  printerSupportDescriptionLabel->setObjectName("printerDescription");
  printerSupportDescriptionLabel->setAlignment(Qt::AlignLeft);
  printerSupportDescriptionLabel->setWordWrap(true);
  printerSupportBenefitsLabel = new QLabel(this);
  printerSupportBenefitsLabel->setObjectName("printerBenefits");
  printerSupportBenefitsLabel->setAlignment(Qt::AlignLeft);
  printerSupportBenefitsLabel->setWordWrap(true);
  printerSupportStatusLabel = new QLabel(this);
  printerSupportStatusLabel->setObjectName("printerStatus");
  printerSupportStatusLabel->setAlignment(Qt::AlignLeft);
  printerSupportStatusLabel->setWordWrap(true);

  printerSupportInstallButton = new QPushButton(this);
  printerSupportInstallButton->setObjectName("printerSupportInstallButton");
  printerSupportInstallButton->setMinimumSize(320, 62);
  printerSupportInstallButton->setCursor(Qt::PointingHandCursor);
  connect(printerSupportInstallButton, &QPushButton::clicked, this,
          &MainWindow::startPrinterSupportInstall);
  printerSupportLaterButton = new QPushButton(this);
  printerSupportLaterButton->setObjectName("printerSupportLaterButton");
  printerSupportLaterButton->setMinimumSize(320, 46);
  printerSupportLaterButton->setCursor(Qt::PointingHandCursor);
  connect(printerSupportLaterButton, &QPushButton::clicked, this,
          &MainWindow::choosePrinterSupportLater);
  printerSupportDisableButton = new QPushButton(this);
  printerSupportDisableButton->setObjectName("printerSupportDisableButton");
  printerSupportDisableButton->setMinimumSize(320, 46);
  printerSupportDisableButton->setCursor(Qt::PointingHandCursor);
  connect(printerSupportDisableButton, &QPushButton::clicked, this,
          &MainWindow::disablePrinterSupport);
  openPrinterSettingsButton = new QPushButton(this);
  openPrinterSettingsButton->setObjectName("backButton");
  openPrinterSettingsButton->setCursor(Qt::PointingHandCursor);
  connect(openPrinterSettingsButton, &QPushButton::clicked, this,
          &MainWindow::openPrinterSettings);
  openScannerButton = new QPushButton(this);
  openScannerButton->setObjectName("backButton");
  openScannerButton->setCursor(Qt::PointingHandCursor);
  connect(openScannerButton, &QPushButton::clicked, this,
          &MainWindow::openScannerApplication);

  printerSupportProgressBar = new QProgressBar(this);
  printerSupportProgressBar->setRange(0, 100);
  printerSupportProgressBar->setValue(0);
  printerSupportProgressBar->setFixedWidth(500);
  printerSupportProgressBar->hide();
  printerLogConsole->setReadOnly(true);
  printerLogConsole->setObjectName("logConsole");
  printerLogConsole->setMaximumHeight(130);
  printerLogConsole->hide();

  QWidget *printerInfoPanel = new QWidget(printerPanel);
  printerInfoPanel->setObjectName("printerInfoPanel");
  QVBoxLayout *printerInfoLayout = new QVBoxLayout(printerInfoPanel);
  printerInfoLayout->setContentsMargins(32, 30, 32, 30);
  QLabel *printerIcon = new QLabel(QStringLiteral("🖨️"), printerInfoPanel);
  printerIcon->setObjectName("printerIllustration");
  printerIcon->setAlignment(Qt::AlignLeft);
  printerInfoLayout->addWidget(printerIcon);
  printerInfoLayout->addSpacing(10);
  printerInfoLayout->addWidget(printerSupportTitleLabel);
  printerInfoLayout->addSpacing(12);
  printerInfoLayout->addWidget(printerSupportDescriptionLabel);
  printerInfoLayout->addSpacing(24);
  printerInfoLayout->addWidget(printerSupportBenefitsLabel);
  printerInfoLayout->addStretch();

  QWidget *printerActionPanel = new QWidget(printerPanel);
  printerActionPanel->setObjectName("printerActionPanel");
  QVBoxLayout *printerActionLayout = new QVBoxLayout(printerActionPanel);
  printerActionLayout->setContentsMargins(28, 30, 28, 30);
  printerActionLayout->addWidget(printerSupportStatusLabel);
  printerActionLayout->addSpacing(20);
  printerActionLayout->addWidget(printerSupportInstallButton);
  printerActionLayout->addSpacing(10);
  printerActionLayout->addWidget(printerSupportLaterButton);
  printerActionLayout->addSpacing(10);
  printerActionLayout->addWidget(printerSupportDisableButton);
  printerActionLayout->addSpacing(20);
  printerActionLayout->addWidget(printerSupportProgressBar);
  printerActionLayout->addWidget(printerLogConsole);
  printerActionLayout->addStretch();

  QHBoxLayout *printerContentLayout = new QHBoxLayout();
  printerContentLayout->setSpacing(24);
  printerContentLayout->addWidget(printerInfoPanel, 3);
  printerContentLayout->addWidget(printerActionPanel, 2);

  QWidget *printerToolsPanel = new QWidget(printerPanel);
  printerToolsPanel->setObjectName("printerToolsPanel");
  QHBoxLayout *printerToolsLayout = new QHBoxLayout(printerToolsPanel);
  printerToolsLayout->setContentsMargins(24, 16, 24, 16);
  printerToolsLayout->addWidget(openPrinterSettingsButton);
  printerToolsLayout->addWidget(openScannerButton);
  printerPanelLayout->addLayout(printerContentLayout, 1);
  printerPanelLayout->addWidget(printerToolsPanel);
  printerLayout->addLayout(printerTopLayout);
  printerLayout->addWidget(printerPanel, 1);

  mainStack->addWidget(welcomeViewWidget);
  mainStack->addWidget(dashboardViewWidget);
  mainStack->addWidget(updateViewWidget);
  mainStack->addWidget(libraryViewWidget);
  mainStack->addWidget(appStoreViewWidget);
  mainStack->addWidget(socialViewWidget);
  mainStack->addWidget(communityViewWidget);
  mainStack->addWidget(printerSupportViewWidget);

  mainLayout->addWidget(mainStack, 1);

  // BOTTOM BAR
  QWidget *bottomBarWidget = new QWidget(this);
  QHBoxLayout *aboutLayout = new QHBoxLayout(bottomBarWidget);
  aboutLayout->setContentsMargins(20, 10, 20, 20);

  QPushButton *aboutBtn = new QPushButton("ℹ️", this);
  aboutBtn->setObjectName("aboutButton");
  aboutBtn->setFixedSize(50, 50);
  aboutBtn->setCursor(Qt::PointingHandCursor);
  connect(aboutBtn, &QPushButton::clicked, this, &MainWindow::showAboutDialog);

  aboutLayout->addWidget(aboutBtn);
  aboutLayout->addStretch();
  mainLayout->addWidget(bottomBarWidget);
}

void MainWindow::createWelcomeSlides() {
  auto addSlide = [this](QLabel *&title, QLabel *&description,
                         const QString &illustration, const QString &target) {
    QWidget *slide = new QWidget(welcomeStack);
    QVBoxLayout *layout = new QVBoxLayout(slide);
    layout->setContentsMargins(32, 20, 32, 20);

    QLabel *icon = new QLabel(illustration, slide);
    icon->setObjectName("welcomeIllustration");
    icon->setAlignment(Qt::AlignCenter);
    title = new QLabel(slide);
    title->setObjectName("slideTitle");
    title->setAlignment(Qt::AlignCenter);
    title->setWordWrap(true);
    description = new QLabel(slide);
    description->setObjectName("slideDesc");
    description->setAlignment(Qt::AlignCenter);
    description->setWordWrap(true);

    for (QLabel *clickTarget : {icon, title, description}) {
      clickTarget->setProperty("welcomeTarget", target);
      clickTarget->setCursor(Qt::PointingHandCursor);
      clickTarget->installEventFilter(this);
    }

    layout->addStretch();
    layout->addWidget(icon);
    layout->addSpacing(20);
    layout->addWidget(title);
    layout->addSpacing(16);
    layout->addWidget(description);
    layout->addStretch();
    welcomeStack->addWidget(slide);
  };

  addSlide(slide1Title, slide1Desc, QStringLiteral("↻"),
           QStringLiteral("update"));
  addSlide(slide2Title, slide2Desc, QStringLiteral("⌘"),
           QStringLiteral("social"));
  addSlide(slide3Title, slide3Desc, QStringLiteral("▣"),
           QStringLiteral("store"));
  addSlide(slide4Title, slide4Desc, QStringLiteral("♥"),
           QStringLiteral("community"));
  addSlide(slide5Title, slide5Desc, QStringLiteral("◈"),
           QStringLiteral("library"));
}

void MainWindow::createDashboard() {
  dashboardViewWidget = new QWidget(this);
  QVBoxLayout *layout = new QVBoxLayout(dashboardViewWidget);
  layout->setContentsMargins(36, 20, 36, 30);
  layout->setSpacing(20);

  QWidget *profilePanel = new QWidget(dashboardViewWidget);
  profilePanel->setObjectName("profilePanel");
  QHBoxLayout *profileLayout = new QHBoxLayout(profilePanel);
  profileLayout->setContentsMargins(24, 18, 24, 18);
  QLabel *avatar = new QLabel("R", profilePanel);
  avatar->setObjectName("profileAvatar");
  avatar->setAlignment(Qt::AlignCenter);
  avatar->setFixedSize(64, 64);
  dashboardGreetingLabel = new QLabel(profilePanel);
  dashboardGreetingLabel->setObjectName("dashboardGreeting");
  dashboardDescriptionLabel = new QLabel(profilePanel);
  dashboardDescriptionLabel->setObjectName("dashboardDescription");
  dashboardDescriptionLabel->setWordWrap(true);
  QVBoxLayout *profileTextLayout = new QVBoxLayout();
  profileTextLayout->addWidget(dashboardGreetingLabel);
  profileTextLayout->addWidget(dashboardDescriptionLabel);
  profileLayout->addWidget(avatar);
  profileLayout->addSpacing(14);
  profileLayout->addLayout(profileTextLayout, 1);

  QGridLayout *grid = new QGridLayout();
  grid->setHorizontalSpacing(18);
  grid->setVerticalSpacing(18);
  grid->setColumnStretch(0, 1);
  grid->setColumnStretch(1, 1);

  auto configureCard = [this](QPushButton *&card) {
    card = new QPushButton(dashboardViewWidget);
    card->setObjectName("dashboardCard");
    card->setMinimumHeight(118);
    card->setCursor(Qt::PointingHandCursor);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  };
  configureCard(dashboardUpdateCard);
  configureCard(dashboardSocialCard);
  configureCard(dashboardStoreCard);
  configureCard(dashboardCommunityCard);
  configureCard(dashboardLibraryCard);
  configureCard(dashboardPrinterCard);
  dashboardUpdateCard->setObjectName("dashboardUpdateCard");
  dashboardSocialCard->setObjectName("dashboardSocialCard");
  dashboardStoreCard->setObjectName("dashboardStoreCard");
  dashboardCommunityCard->setObjectName("dashboardCommunityCard");
  dashboardLibraryCard->setObjectName("dashboardLibraryCard");
  dashboardPrinterCard->setObjectName("dashboardPrinterCard");
  connect(dashboardUpdateCard, &QPushButton::clicked, this,
          &MainWindow::showUpdateScreen);
  connect(dashboardSocialCard, &QPushButton::clicked, this,
          &MainWindow::showSocialScreen);
  connect(dashboardStoreCard, &QPushButton::clicked, this,
          &MainWindow::showAppStoreScreen);
  connect(dashboardCommunityCard, &QPushButton::clicked, this,
          &MainWindow::showCommunityScreen);
  connect(dashboardLibraryCard, &QPushButton::clicked, this,
          &MainWindow::showLibraryScreen);
  connect(dashboardPrinterCard, &QPushButton::clicked, this,
          &MainWindow::showPrinterSupportScreen);

  grid->addWidget(dashboardUpdateCard, 0, 0);
  grid->addWidget(dashboardSocialCard, 0, 1);
  grid->addWidget(dashboardStoreCard, 1, 0);
  grid->addWidget(dashboardCommunityCard, 1, 1);
  grid->addWidget(dashboardPrinterCard, 2, 0);
  grid->addWidget(dashboardLibraryCard, 2, 1);

  layout->addWidget(profilePanel);
  layout->addLayout(grid);
  layout->addStretch();
}

void MainWindow::updateUiTextAndImages() {
  const auto bundle = RoAssist::UiTexts::buildBundle(
      currentLanguageCode(), currentTheme == Dark, logConsole->isVisible(),
      libraryLogConsole->isVisible(), isLibraryInstalled);

  langBtn->setText(bundle.languageButton);
  themeToggleBtn->setText(bundle.themeToggle);
  networkStatusLabel->setText(bundle.networkStatus);
  backToCarouselBtn->setText(bundle.backButton);
  backFromLibraryBtn->setText(bundle.backButton);
  backFromAppStoreBtn->setText(bundle.backButton);
  backFromSocialBtn->setText(bundle.backButton);
  backFromCommunityBtn->setText(bundle.backButton);
  backFromPrinterSupportBtn->setText(bundle.backButton);
  toggleLogBtn->setText(bundle.toggleLogs);
  toggleLibraryLogBtn->setText(bundle.toggleLibraryLogs);
  versionLabel->setText(bundle.versionLabel);
  updateButton->setText(bundle.updateButton);
  welcomeProgressLabel->setText(
      bundle.welcomeProgress.arg(welcomeStack->currentIndex() + 1)
          .arg(welcomeStack->count()));
  welcomeNextBtn->setText(welcomeStack->currentIndex() == welcomeStack->count() - 1
                              ? bundle.welcomeFinish
                              : bundle.welcomeNext);
  dashboardGreetingLabel->setText(bundle.dashboardGreeting);
  dashboardDescriptionLabel->setText(bundle.dashboardDescription);
  dashboardUpdateCard->setText(bundle.dashboardUpdateCard);
  dashboardSocialCard->setText(bundle.dashboardSocialCard);
  dashboardStoreCard->setText(bundle.dashboardStoreCard);
  dashboardCommunityCard->setText(bundle.dashboardCommunityCard);
  dashboardLibraryCard->setText(bundle.dashboardLibraryCard);
  dashboardPrinterCard->setText(bundle.dashboardPrinterCard);
  slide1Title->setText(bundle.slide1Title);
  slide1Desc->setText(bundle.slide1Description);
  slide2Title->setText(bundle.slide2Title);
  slide2Desc->setText(bundle.slide2Description);
  slide3Title->setText(bundle.slide3Title);
  slide3Desc->setText(bundle.slide3Description);
  appStoreTitleLabel->setText(bundle.appStoreTitle);
  appStoreOpenAppBtn->setText(bundle.appStoreOpenButton);
  slide4Title->setText(bundle.slide4Title);
  slide4Desc->setText(bundle.slide4Description);
  socialTitleLabel->setText(bundle.slide2Title);
  socialDescriptionLabel->setText(bundle.slide2Description);
  communityTitleLabel->setText(bundle.slide4Title);
  communityDescriptionLabel->setText(bundle.slide4Description);
  bozokBtn->setText(bundle.communityButton);
  slide5Title->setText(bundle.slide5Title);
  slide5Desc->setText(bundle.slide5Description);
  if (activeOperation != LibraryInstall) {
    libraryStatusLabel->setText(bundle.libraryStatusIdle);
  }
  logConsole->setPlaceholderText(bundle.logPlaceholder);
  libraryLogConsole->setPlaceholderText(bundle.logPlaceholder);
  roAsdGitHubBtn->setText("ro-ASD OS\nRepo");
  roAssistGitHubBtn->setText("ro-Assist\nRepo");
  websiteBtn->setText(bundle.websiteButton);
  printerSupportTitleLabel->setText(bundle.printerSupportTitle);
  printerSupportDescriptionLabel->setText(bundle.printerSupportDescription);
  printerSupportBenefitsLabel->setText(bundle.printerSupportBenefits);
  printerSupportInstallButton->setText(bundle.printerSupportInstallButton);
  printerSupportLaterButton->setText(bundle.printerSupportLaterButton);
  printerSupportDisableButton->setText(bundle.printerSupportDisableButton);
  openPrinterSettingsButton->setText(bundle.printerSupportOpenSettingsButton);
  openScannerButton->setText(bundle.printerSupportOpenScannerButton);
  printerLogConsole->setPlaceholderText(bundle.logPlaceholder);
  if (!isPrinterSupportOperationActive()) {
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("Project-Ro-ASD"),
                       QStringLiteral("ro-assist"));
    const QString preference =
        settings.value(QStringLiteral("printer/supportPreference")).toString();
    printerSupportStatusLabel->setText(
        isPrinterSupportInstalled || preference == QStringLiteral("installed")
            ? bundle.printerSupportInstalled
            : (preference == QStringLiteral("later")
                   ? bundle.printerSupportLaterSelected
                   : (preference == QStringLiteral("disabled")
                          ? bundle.printerSupportDisabled
                          : bundle.printerSupportStatusIdle)));
  }
  if (activeOperation != LibraryInstall &&
      checkLibProcess->state() == QProcess::NotRunning) {
    libraryInstallButton->setText(bundle.libraryActionButton);
  }

  setInitialUpdateStatus();
}

void MainWindow::setInitialUpdateStatus() {
  if (updateProcess->state() != QProcess::NotRunning || transactionPhaseStarted)
    return;

  statusLabel->setText(RoAssist::UiTexts::buildBundle(
                           currentLanguageCode(), currentTheme == Dark,
                           logConsole->isVisible(),
                           libraryLogConsole->isVisible(), isLibraryInstalled)
                           .checkingUpdates);
}

void MainWindow::setupStyle() {
  QString baseBg = currentTheme == Dark ? "#352F44" : "#FBF9F1";
  QString textCol = currentTheme == Dark ? "#FAF0E6" : "#352F44";
  QString subTextCol = currentTheme == Dark ? "#B9B4C7" : "#5C5470";
  QString borderCol = currentTheme == Dark ? "#5C5470" : "#E5E1DA";
  QString surfaceSoft = currentTheme == Dark ? "#5C5470" : "#E5E1DA";

  QString primary = currentTheme == Dark ? "#5C5470" : "#92C7CF";
  QString accent1 = currentTheme == Dark ? "#352F44" : "#AAD7D9";
  QString accent2 = currentTheme == Dark ? "#B9B4C7" : "#92C7CF";

  QString style =
      QString(R"(
        * { outline: none; }
        QMainWindow { background-color: %1; }
        QWidget#panelWidget { 
            background-color: transparent; 
            border: none; 
        }
        QLabel { color: %3; }
        QLabel#versionLabel { font-size: 14px; font-weight: 600; color: %4; }
        QLabel#statusLabel { font-size: 16px; color: %4; font-weight: 600; }
        QLabel#slideTitle { font-size: 34px; font-weight: 800; margin-bottom: 8px; color: %3; }
        QLabel#slideDesc { font-size: 17px; color: %4; line-height: 1.5; }
        QLabel#welcomeIllustration { font-size: 92px; color: %7; }
        QLabel#welcomeProgressLabel { font-size: 14px; color: %4; font-weight: 600; }
        QWidget#profilePanel {
            background-color: %6; border: 1px solid %5; border-radius: 16px;
        }
        QLabel#profileAvatar {
            background-color: %7; color: white; border-radius: 32px;
            font-size: 28px; font-weight: 800;
        }
        QLabel#dashboardGreeting { font-size: 25px; font-weight: 800; color: %3; }
        QLabel#dashboardDescription { font-size: 15px; color: %4; }
        QWidget#printerInfoPanel, QWidget#printerActionPanel, QWidget#printerToolsPanel {
            background-color: %6; border: 1px solid %5; border-radius: 18px;
        }
        QLabel#printerIllustration { font-size: 58px; }
        QLabel#printerTitle { font-size: 34px; font-weight: 800; color: %3; }
        QLabel#printerDescription { font-size: 18px; color: %4; line-height: 1.45; }
        QLabel#printerBenefits {
            background-color: %1; border: 1px solid %5; border-radius: 12px;
            color: %3; font-size: 17px; line-height: 1.55; padding: 18px;
        }
        QLabel#printerStatus { font-size: 17px; color: %3; font-weight: 700; }
        
        QTextEdit#logConsole {
            background-color: palette(base); color: palette(text); border: 1px solid %5;
            border-radius: 8px; font-family: 'Cascadia Code', 'Consolas', monospace; font-size: 12px;
            padding: 8px;
        }
        QPushButton, QToolButton {
            padding: 8px 16px; font-size: 14px; font-weight: 600;
        }
        
        QPushButton#backButton, QPushButton#backToHomeButton, QPushButton#printerSupportLaterButton, QPushButton#printerSupportDisableButton {
            background-color: palette(button); border: 1px solid %5; color: palette(button-text);
            border-radius: 8px; font-size: 14px; font-weight: bold;
        }
        QPushButton#backButton:hover, QPushButton#backToHomeButton:hover, QPushButton#printerSupportLaterButton:hover, QPushButton#printerSupportDisableButton:hover { border: 1px solid %7; color: %7; }
        
        QPushButton#systemUpdateButton, QPushButton#libraryInstallButton, QPushButton#actionButton, QPushButton#welcomeNextButton, QPushButton#printerSupportInstallButton {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %7, stop:1 %8); 
            color: white; border: none; font-size: 18px; font-weight: 700; border-radius: 14px; 
            padding: 12px;
        }
        QPushButton#systemUpdateButton:hover, QPushButton#libraryInstallButton:hover, QPushButton#actionButton:hover, QPushButton#welcomeNextButton:hover, QPushButton#printerSupportInstallButton:hover {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %9, stop:1 %7); 
        }
        QPushButton#systemUpdateButton:disabled, QPushButton#libraryInstallButton:disabled, QPushButton#actionButton:disabled, QPushButton#welcomeNextButton:disabled, QPushButton#printerSupportInstallButton:disabled {
            background: %5; color: %4; 
        }

        QPushButton#dashboardCard {
            background-color: %6; border: 1px solid %5; border-radius: 16px;
            color: %3; font-size: 17px; font-weight: 700; padding: 20px;
            text-align: left;
        }
        QPushButton#dashboardCard:hover {
            background-color: %1; border: 2px solid %7;
        }
        
        QToolButton#squareSoftButton { 
            border-radius: 16px; font-size: 15px; font-weight: 600;
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %7, stop:1 %8); 
            border: none; color: white; padding: 12px; 
        }
        QToolButton#squareSoftButton:hover { 
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %8, stop:1 %7); 
            color: white; 
        }
        
        QPushButton#roundButton { border-radius: 60px; font-size: 46px; background-color: %10; border: 2px solid %7; }
        QPushButton#roundButton:hover { background-color: %2; border: 2px solid %8; color: %7; }
        
        QPushButton#aboutButton { border-radius: 25px; font-size: 22px; border: none; background-color: transparent; }
        QPushButton#aboutButton:hover { background-color: %6; }
        
        QProgressBar { 
            border: none; border-radius: 8px; text-align: center; 
            background-color: %10; color: transparent; font-weight: bold; height: 16px;
        }
        QProgressBar::chunk { background-color: %7; border-radius: 8px; }
    )")
          .arg(baseBg, baseBg, textCol, subTextCol, borderCol, surfaceSoft)
          .arg(primary, accent1, accent2, surfaceSoft);

  setStyleSheet(style);
}

void MainWindow::advanceWelcome() {
  const int nextIndex = welcomeStack->currentIndex() + 1;
  if (nextIndex < welcomeStack->count()) {
    welcomeStack->setCurrentIndex(nextIndex);
    updateUiTextAndImages();
    return;
  }

  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  settings.setValue(QStringLiteral("welcome/completed"), true);
  settings.sync();
  welcomeCompleted = true;
  showDashboardScreen();
}

void MainWindow::showWelcomeScreen() {
  mainStack->setCurrentWidget(welcomeViewWidget);
  updateUiTextAndImages();
}

void MainWindow::showDashboardScreen() {
  mainStack->setCurrentWidget(dashboardViewWidget);
}

void MainWindow::showHomeScreen() {
  if (welcomeCompleted)
    showDashboardScreen();
  else
    showWelcomeScreen();
}

void MainWindow::showUpdateScreen() {
  mainStack->setCurrentWidget(updateViewWidget);
}
void MainWindow::showLibraryScreen() {
  mainStack->setCurrentWidget(libraryViewWidget);

  libraryInstallButton->setEnabled(false);
  libraryInstallButton->setText(
      RoAssist::UiTexts::checkingLibraries(currentLanguageCode()));

  if (qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS") &&
      checkLibProcess->state() == QProcess::NotRunning) {
    checkLibProcess->start("rpm", QStringList()
                                      << "-q" << "gamemode" << "mangohud"
                                      << "vulkan-loader" << "vulkan-tools");
  } else if (!qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS")) {
    libraryInstallButton->setEnabled(true);
    libraryInstallButton->setText(RoAssist::UiTexts::buildBundle(
                                      currentLanguageCode(),
                                      currentTheme == Dark,
                                      logConsole->isVisible(),
                                      libraryLogConsole->isVisible(),
                                      isLibraryInstalled)
                                      .libraryActionButton);
  }
}
void MainWindow::showAppStoreScreen() {
  mainStack->setCurrentWidget(appStoreViewWidget);
}
void MainWindow::showSocialScreen() {
  mainStack->setCurrentWidget(socialViewWidget);
}
void MainWindow::showCommunityScreen() {
  mainStack->setCurrentWidget(communityViewWidget);
}
void MainWindow::showPrinterSupportScreen() {
  mainStack->setCurrentWidget(printerSupportViewWidget);
  updateUiTextAndImages();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event) {
  if (!welcomeCompleted && mainStack->currentWidget() == welcomeViewWidget &&
      event->type() == QEvent::MouseButtonRelease) {
    const QString target = watched->property("welcomeTarget").toString();
    if (target == QStringLiteral("update"))
      showUpdateScreen();
    else if (target == QStringLiteral("social"))
      showSocialScreen();
    else if (target == QStringLiteral("store"))
      showAppStoreScreen();
    else if (target == QStringLiteral("community"))
      showCommunityScreen();
    else if (target == QStringLiteral("library"))
      showLibraryScreen();
    else
      return QMainWindow::eventFilter(watched, event);
    return true;
  }

  return QMainWindow::eventFilter(watched, event);
}

void MainWindow::changeLanguageAction(QAction *action) {
  if (!action)
    return;
  currentLang = static_cast<Language>(action->data().toInt());
  updateUiTextAndImages();
}

void MainWindow::changeLanguage(QAction *action) {
  if (!action)
    return;

  const QString languageCode = action->data().toString().toLower();
  if (languageCode == QStringLiteral("tr"))
    currentLang = TR;
  else if (languageCode == QStringLiteral("es"))
    currentLang = ES;
  else if (languageCode == QStringLiteral("de"))
    currentLang = DE;
  else if (languageCode == QStringLiteral("fr"))
    currentLang = FR;
  else
    currentLang = EN;
  updateUiTextAndImages();
}

void MainWindow::toggleTheme() {
  currentTheme = (currentTheme == Light) ? Dark : Light;
  updateUiTextAndImages();
  setupStyle();
}

void MainWindow::toggleUpdateLogs() {
  logConsole->setVisible(!logConsole->isVisible());
  updateUiTextAndImages();
}

void MainWindow::toggleLibraryLogs() {
  libraryLogConsole->setVisible(!libraryLogConsole->isVisible());
  updateUiTextAndImages();
}

void MainWindow::onNetworkConnectedChanged(bool isConnected) {
  isNetworkConnected = isConnected;
  networkStatusLabel->setVisible(!isNetworkConnected);
}

void MainWindow::openWebsite() {
  QDesktopServices::openUrl(QUrl("https://github.com/Project-Ro-ASD"));
}
void MainWindow::openRoAsdGitHub() {
  QDesktopServices::openUrl(QUrl("https://github.com/Project-Ro-ASD/ro-asd"));
}
void MainWindow::openRoAssistGitHub() {
  QDesktopServices::openUrl(
      QUrl("https://github.com/Project-Ro-ASD/ro-Assist"));
}
void MainWindow::openBozokCommunity() {
  QDesktopServices::openUrl(QUrl("https://github.com/Project-Ro-ASD"));
}
void MainWindow::showAboutDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle(
      RoAssist::UiTexts::aboutTitle(currentLanguageCode()));
  dialog.setFixedSize(550, 400);
  dialog.setStyleSheet(this->styleSheet() +
                       QString(" QDialog { background-color: %1; "
                               "border-radius: 16px; border: 1px solid %2; }")
                           .arg(currentTheme == Dark ? "#352F44" : "#FBF9F1",
                                currentTheme == Dark ? "#5C5470" : "#E5E1DA"));

  QVBoxLayout *layout = new QVBoxLayout(&dialog);
  layout->setContentsMargins(40, 40, 40, 40);
  layout->setSpacing(25);

  QLabel *titleLabel = new QLabel("ro-Assist", &dialog);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet(
      QString("font-size: 42px; font-weight: 900; color: %1;")
          .arg(currentTheme == Dark ? "#FAF0E6" : "#352F44"));

  QLabel *descLabel = new QLabel(&dialog);
  descLabel->setWordWrap(true);
  descLabel->setAlignment(Qt::AlignCenter);
  QString desc =
      RoAssist::UiTexts::aboutDescription(currentLanguageCode());
  descLabel->setText(desc);
  descLabel->setStyleSheet(
      "font-size: 16px; color: " +
      QString(currentTheme == Dark ? "#B9B4C7" : "#5C5470") + ";");

  QLabel *infoLabel = new QLabel(&dialog);
  infoLabel->setAlignment(Qt::AlignCenter);
  infoLabel->setText(
      QString("<span style='font-size: 15px;'><b>%1:</b> Ebubekir "
              "Bulut<br><br><b>%2:</b> 2026</span>")
          .arg(RoAssist::UiTexts::developerLabel(currentLanguageCode()))
          .arg(RoAssist::UiTexts::yearLabel(currentLanguageCode())));
  infoLabel->setStyleSheet(
      "color: " + QString(currentTheme == Dark ? "#B9B4C7" : "#5C5470") + ";");

  QPushButton *okBtn = new QPushButton(
      RoAssist::UiTexts::closeLabel(currentLanguageCode()),
      &dialog);
  okBtn->setObjectName("actionButton");
  okBtn->setFixedSize(200, 50);
  okBtn->setCursor(Qt::PointingHandCursor);
  QObject::connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

  layout->addStretch();
  layout->addWidget(titleLabel);
  layout->addSpacing(10);
  layout->addWidget(descLabel);
  layout->addSpacing(20);
  layout->addWidget(infoLabel);
  layout->addStretch();
  layout->addWidget(okBtn, 0, Qt::AlignHCenter);

  dialog.exec();
}

void MainWindow::dummyAppStoreAction() {
  QMessageBox::information(
      this,
      RoAssist::UiTexts::storeVersionTitle(currentLanguageCode()),
      RoAssist::UiTexts::storeVersionMessage(currentLanguageCode()));
}

void MainWindow::startLibraryPackageInstall() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::noInternetMessage(currentLanguageCode()));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::operationRunningMessage(currentLanguageCode()));
    return;
  }

  libraryInstallButton->setEnabled(false);
  libraryProgressBar->setRange(0, 100);
  libraryProgressBar->setValue(0);
  libraryProgressBar->show();
  libraryStatusLabel->setText(
      RoAssist::UiTexts::librariesInstalling(currentLanguageCode()));

  libraryLogConsole->clear();
  appendLog(RoAssist::UiTexts::librariesInstalling(currentLanguageCode()),
            "#0066cc");
  setOperationRunning(LibraryInstall);

  QString action = isLibraryInstalled ? "upgrade" : "install";
  updateProcess->start("pkexec", QStringList()
                                     << "dnf" << action << "-y" << "gamemode"
                                     << "mangohud" << "vulkan-loader"
                                     << "vulkan-tools");
}

void MainWindow::startPrinterSupportInstall() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::noInternetMessage(currentLanguageCode()));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::operationRunningMessage(currentLanguageCode()));
    return;
  }

  printerSupportInstallButton->setEnabled(false);
  printerSupportLaterButton->setEnabled(false);
  printerSupportDisableButton->setEnabled(false);
  printerSupportProgressBar->setRange(0, 0);
  printerSupportProgressBar->show();
  printerSupportStatusLabel->setText(
      RoAssist::UiTexts::buildBundle(
          currentLanguageCode(), currentTheme == Dark, logConsole->isVisible(),
          libraryLogConsole->isVisible(), isLibraryInstalled)
          .printerSupportInstalling);
  printerLogConsole->clear();
  printerLogConsole->show();
  appendPrinterLog(printerSupportStatusLabel->text(), "#0066cc");
  setOperationRunning(PrinterSupportInstall);
  printerSupportProcess->start("pkexec",
                               QStringList() << "dnf" << "install" << "-y"
                                             << "ro-printer-support");
}

void MainWindow::choosePrinterSupportLater() {
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  settings.setValue(QStringLiteral("printer/supportPreference"),
                    QStringLiteral("later"));
  settings.sync();
  updateUiTextAndImages();
}

void MainWindow::disablePrinterSupport() {
  QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                     QStringLiteral("Project-Ro-ASD"),
                     QStringLiteral("ro-assist"));
  settings.setValue(QStringLiteral("printer/supportPreference"),
                    QStringLiteral("disabled"));
  settings.sync();
  updateUiTextAndImages();
}

void MainWindow::openPrinterSettings() {
  if (!QProcess::startDetached("kcmshell6",
                               QStringList() << "kcm_printer_manager")) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::componentFailedToStart(currentLanguageCode()));
  }
}

void MainWindow::openScannerApplication() {
  if (!QProcess::startDetached("skanpage")) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::componentFailedToStart(currentLanguageCode()));
  }
}

void MainWindow::startUpdate() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::noInternetMessage(currentLanguageCode()));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, RoAssist::UiTexts::infoTitle(currentLanguageCode()),
        RoAssist::UiTexts::operationRunningMessage(currentLanguageCode()));
    return;
  }

  updateButton->setEnabled(false);
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->show();

  statusLabel->setText(
      RoAssist::UiTexts::updateStarting(currentLanguageCode()));
  logConsole->clear();
  appendLog(RoAssist::UiTexts::updateStarting(currentLanguageCode()),
            "#0066cc");
  setOperationRunning(SystemUpdate);

  updateProcess->start("pkexec", QStringList()
                                     << "sh" << "-c"
                                     << RoAssist::UpdateHelpers::buildSystemUpdateCommand());
}

void MainWindow::appendLog(const QString &text, const QString &color) {
  QString formattedText = QString("<span style='color:%1'>%2</span>")
                              .arg(color, text.toHtmlEscaped());
  if (isLibraryOperationActive()) {
    libraryLogConsole->append(formattedText);
    QScrollBar *sb = libraryLogConsole->verticalScrollBar();
    sb->setValue(sb->maximum());
  } else {
    logConsole->append(formattedText);
    QScrollBar *sb = logConsole->verticalScrollBar();
    sb->setValue(sb->maximum());
  }
}

void MainWindow::appendPrinterLog(const QString &text, const QString &color) {
  const QString formattedText = QString("<span style='color:%1'>%2</span>")
                                    .arg(color, text.toHtmlEscaped());
  printerLogConsole->append(formattedText);
  QScrollBar *scrollBar = printerLogConsole->verticalScrollBar();
  scrollBar->setValue(scrollBar->maximum());
}

void MainWindow::handlePrinterSupportOutput() {
  const QString output =
      QString::fromUtf8(printerSupportProcess->readAllStandardOutput());
  for (const QString &line : output.split('\n', Qt::SkipEmptyParts))
    appendPrinterLog(line, currentTheme == Dark ? "#cccccc" : "#e60909ff");

  if (const auto progress =
          RoAssist::UpdateHelpers::parseTransactionProgress(output)) {
    printerSupportProgressBar->setRange(0, progress->total);
    printerSupportProgressBar->setValue(progress->current);
    printerSupportStatusLabel->setText(RoAssist::UiTexts::installingPackages(
        currentLanguageCode(), progress->current, progress->total));
  } else if (const auto percent =
                 RoAssist::UpdateHelpers::parseDownloadPercent(output)) {
    printerSupportProgressBar->setRange(0, 100);
    printerSupportProgressBar->setValue(*percent);
    printerSupportStatusLabel->setText(
        RoAssist::UiTexts::downloading(currentLanguageCode(), *percent));
  }
}

void MainWindow::handlePrinterSupportErrorOutput() {
  const QString output =
      QString::fromUtf8(printerSupportProcess->readAllStandardError());
  for (const QString &line : output.split('\n', Qt::SkipEmptyParts))
    appendPrinterLog(line, "#cc7700");
}

void MainWindow::handlePrinterSupportFinished(
    int exitCode, QProcess::ExitStatus exitStatus) {
  printerSupportInstallButton->setEnabled(true);
  printerSupportLaterButton->setEnabled(true);
  printerSupportDisableButton->setEnabled(true);
  if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    isPrinterSupportInstalled = true;
    printerSupportProgressBar->setRange(0, 100);
    printerSupportProgressBar->setValue(100);
    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QStringLiteral("Project-Ro-ASD"),
                       QStringLiteral("ro-assist"));
    settings.setValue(QStringLiteral("printer/supportPreference"),
                      QStringLiteral("installed"));
    settings.sync();
    clearActiveOperation();
    updateUiTextAndImages();
    appendPrinterLog(printerSupportStatusLabel->text(), "#00cc00");
    return;
  } else {
    printerSupportProgressBar->hide();
    printerSupportStatusLabel->setText(
        RoAssist::UiTexts::buildBundle(
            currentLanguageCode(), currentTheme == Dark, logConsole->isVisible(),
            libraryLogConsole->isVisible(), isLibraryInstalled)
            .printerSupportFailed);
    appendPrinterLog(printerSupportStatusLabel->text(), "#ff4444");
  }
  clearActiveOperation();
}

void MainWindow::handlePrinterSupportProcessError(QProcess::ProcessError error) {
  if (error == QProcess::Crashed || error == QProcess::FailedToStart) {
    printerSupportProgressBar->hide();
    printerSupportInstallButton->setEnabled(true);
    printerSupportLaterButton->setEnabled(true);
    printerSupportDisableButton->setEnabled(true);
    printerSupportStatusLabel->setText(
        RoAssist::UiTexts::criticalErrorPrefix(currentLanguageCode()) +
        (error == QProcess::FailedToStart
             ? RoAssist::UiTexts::componentFailedToStart(currentLanguageCode())
             : RoAssist::UiTexts::componentCrashed(currentLanguageCode())));
    printerLogConsole->show();
    clearActiveOperation();
  }
}

void MainWindow::checkDnfErrors(const QString &output) {
  if (output.contains("Waiting for process", Qt::CaseInsensitive) ||
      output.contains("Another app is currently holding the yum lock",
                      Qt::CaseInsensitive)) {
    QString msg = RoAssist::UiTexts::systemBusy(currentLanguageCode());
    if (isLibraryOperationActive())
      libraryStatusLabel->setText(msg);
    else
      statusLabel->setText(msg);
  }
  if (output.contains("Error: Failed to download metadata",
                      Qt::CaseInsensitive) ||
      output.contains("Could not resolve host", Qt::CaseInsensitive)) {
    QString msg = RoAssist::UiTexts::networkError(currentLanguageCode());
    if (isLibraryOperationActive()) {
      libraryStatusLabel->setText(msg);
      libraryInstallButton->setEnabled(true);
    } else {
      statusLabel->setText(msg);
      updateButton->setEnabled(true);
    }
  }
}

void MainWindow::handleUpdateOutput() {
  QString output = QString::fromUtf8(updateProcess->readAllStandardOutput());
  QStringList lines = output.split('\n', Qt::SkipEmptyParts);
  for (const QString &line : lines) {
    appendLog(line, currentTheme == Dark ? "#cccccc" : "#e60909ff");
  }
  checkDnfErrors(output);

  if (RoAssist::UpdateHelpers::containsNoWorkMarker(output)) {
    transactionPhaseStarted = true;
  }

  if (const auto progress =
          RoAssist::UpdateHelpers::parseTransactionProgress(output)) {
    transactionPhaseStarted = true;
    QString t = RoAssist::UiTexts::installingPackages(
        currentLanguageCode(), progress->current, progress->total);
    if (isLibraryOperationActive()) {
      libraryProgressBar->setRange(0, progress->total);
      libraryProgressBar->setValue(progress->current);
      libraryStatusLabel->setText(t);
    } else {
      progressBar->setRange(0, progress->total);
      progressBar->setValue(progress->current);
      statusLabel->setText(t);
    }
  }

  if (!transactionPhaseStarted) {
    if (const auto percent =
            RoAssist::UpdateHelpers::parseDownloadPercent(output)) {
      QString t = RoAssist::UiTexts::downloading(currentLanguageCode(),
                                                 *percent);
      if (isLibraryOperationActive()) {
        libraryProgressBar->setRange(0, 100);
        libraryProgressBar->setValue(*percent);
        libraryStatusLabel->setText(t);
      } else {
        progressBar->setRange(0, 100);
        progressBar->setValue(*percent);
        statusLabel->setText(t);
      }
    }
  }
}

void MainWindow::handleUpdateErrorOutput() {
  QString errorOutput =
      QString::fromUtf8(updateProcess->readAllStandardError());
  QStringList lines = errorOutput.split('\n', Qt::SkipEmptyParts);
  for (const QString &line : lines) {
    if (!line.contains("[sudo]")) {
      appendLog(line, "#cc7700");
    }
  }
  checkDnfErrors(errorOutput);

  if (errorOutput.contains("standard input:1", Qt::CaseInsensitive) ||
      errorOutput.contains("incorrect password", Qt::CaseInsensitive) ||
      errorOutput.contains("try again", Qt::CaseInsensitive)) {
    isTerminatingIntentionally = true;
    updateProcess->terminate();
    QString t =
        RoAssist::UiTexts::wrongPasswordShort(currentLanguageCode());
    if (isLibraryOperationActive()) {
      libraryProgressBar->hide();
      libraryProgressBar->setValue(0);
      libraryStatusLabel->setText(t);
      libraryInstallButton->setEnabled(true);
      if (!libraryLogConsole->isVisible())
        toggleLibraryLogs();
    } else {
      progressBar->hide();
      progressBar->setValue(0);
      statusLabel->setText(t);
      updateButton->setEnabled(true);
      if (!logConsole->isVisible())
        toggleUpdateLogs();
    }
    QMessageBox::critical(
        this, RoAssist::UiTexts::errorTitle(currentLanguageCode()),
        RoAssist::UiTexts::wrongPasswordDetail(currentLanguageCode()));
  }
}

void MainWindow::handleCheckUpdateFinished(int exitCode,
                                           QProcess::ExitStatus exitStatus) {
  if (transactionPhaseStarted)
    return;

  switch (RoAssist::UpdateHelpers::classifyCheckUpdateResult(exitCode,
                                                             exitStatus)) {
  case RoAssist::UpdateHelpers::UpdateCheckStatus::UpdatesAvailable:
    statusLabel->setText(
        RoAssist::UiTexts::updateAvailable(currentLanguageCode()));
    break;
  case RoAssist::UpdateHelpers::UpdateCheckStatus::UpToDate:
    statusLabel->setText(
        RoAssist::UiTexts::systemUpToDate(currentLanguageCode()));
    break;
  case RoAssist::UpdateHelpers::UpdateCheckStatus::Failed:
    statusLabel->setText(
        RoAssist::UiTexts::updateCheckFailed(currentLanguageCode()));
    break;
  }
}

void MainWindow::handleCheckLibFinished(int exitCode,
                                        QProcess::ExitStatus exitStatus) {
  if (exitStatus == QProcess::NormalExit && exitCode == 0) {
    isLibraryInstalled = true;
  } else {
    isLibraryInstalled = false;
  }
  updateUiTextAndImages();
  libraryInstallButton->setEnabled(true);
}

void MainWindow::handleUpdateFinished(int exitCode,
                                      QProcess::ExitStatus exitStatus) {
  bool libraryOperation = isLibraryOperationActive();
  if (libraryOperation) {
    libraryInstallButton->setEnabled(true);
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      isLibraryInstalled = true;
      libraryProgressBar->setRange(0, 100);
      libraryProgressBar->setValue(100);
      libraryStatusLabel->setText(
          RoAssist::UiTexts::processCompleted(currentLanguageCode()));
      appendLog(RoAssist::UiTexts::librariesInstalled(currentLanguageCode()),
                "#00cc00");
      updateUiTextAndImages();
    } else {
      if (!isTerminatingIntentionally) {
        libraryProgressBar->hide();
        libraryStatusLabel->setText(
            RoAssist::UiTexts::processFailed(currentLanguageCode()));
        appendLog(RoAssist::UiTexts::processFailedDetails(
                      currentLanguageCode(), true),
                  "#ff4444");
        if (!libraryLogConsole->isVisible())
          toggleLibraryLogs();
      }
    }
  } else {
    updateButton->setEnabled(true);
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      progressBar->setRange(0, 100);
      progressBar->setValue(100);
      statusLabel->setText(
          RoAssist::UiTexts::processCompleted(currentLanguageCode()));
      appendLog(RoAssist::UiTexts::allOperationsCompleted(
                    currentLanguageCode()),
                "#00cc00");
    } else {
      if (!isTerminatingIntentionally) {
        progressBar->hide();
        statusLabel->setText(
            RoAssist::UiTexts::processFailed(currentLanguageCode()));
        appendLog(RoAssist::UiTexts::processFailedDetails(
                      currentLanguageCode(), false),
                  "#ff4444");
        if (!logConsole->isVisible())
          toggleUpdateLogs();
      }
    }
  }
  clearActiveOperation();
}

void MainWindow::handleUpdateProcessError(QProcess::ProcessError error) {
  if (isTerminatingIntentionally)
    return;

  QString errorMsg =
      (error == QProcess::FailedToStart)
          ? RoAssist::UiTexts::componentFailedToStart(currentLanguageCode())
          : RoAssist::UiTexts::componentCrashed(currentLanguageCode());

  QMessageBox::critical(
      this, RoAssist::UiTexts::criticalErrorTitle(currentLanguageCode()),
      errorMsg);

  if (isLibraryOperationActive()) {
    libraryProgressBar->hide();
    libraryInstallButton->setEnabled(true);
    libraryStatusLabel->setText(
        RoAssist::UiTexts::criticalErrorPrefix(currentLanguageCode()) +
        errorMsg);
    if (!libraryLogConsole->isVisible())
      toggleLibraryLogs();
  } else {
    progressBar->hide();
    updateButton->setEnabled(true);
    statusLabel->setText(
        RoAssist::UiTexts::criticalErrorPrefix(currentLanguageCode()) +
        errorMsg);
    if (!logConsole->isVisible())
      toggleUpdateLogs();
  }
  clearActiveOperation();
}
