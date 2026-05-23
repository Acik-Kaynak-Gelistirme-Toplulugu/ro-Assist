#include "roassist/mainwindow.h"
#include "roassist/update_helpers.h"

#include <QDesktopServices>
#include <QDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLocale>
#include <QMessageBox>
#include <QPalette>
#include <QScreen>
#include <QScrollBar>
#include <QStyleHints>
#include <QUrl>
#include <QVBoxLayout>
#include <QEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), updateProcess(new QProcess(this)),
      checkUpdateProcess(new QProcess(this)),
      checkLibProcess(new QProcess(this)), carousel(new QStackedWidget(this)),
      carouselTimer(new QTimer(this)), logConsole(new QPlainTextEdit(this)),
      libraryLogConsole(new QPlainTextEdit(this)), activeOperation(None),
      transactionPhaseStarted(false), isTerminatingIntentionally(false),
      isNetworkConnected(true), isLibraryInstalled(false) {
  detectTheme();

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

  QLocale locale = QLocale::system();
  if (locale.language() == QLocale::Turkish)
    loadLanguage(QStringLiteral("tr"));
  else if (locale.language() == QLocale::Spanish)
    loadLanguage(QStringLiteral("es"));
  else
    loadLanguage(QStringLiteral("en"));

  setupUi();
  updateUiTextAndImages();
  setupStyle();

  logConsole->hide();
  libraryLogConsole->hide();
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

  connect(carouselTimer, &QTimer::timeout, this, &MainWindow::nextSlide);
  carouselTimer->start(5000);

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
}

void MainWindow::changeEvent(QEvent *event) {
  if (event->type() == QEvent::LanguageChange) {
    updateUiTextAndImages();
  }
  QMainWindow::changeEvent(event);
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
         updateProcess->state() != QProcess::NotRunning;
}

bool MainWindow::isLibraryOperationActive() const {
  return activeOperation == LibraryInstall;
}

QString MainWindow::langFlag(const QString &langCode) const {
  if (langCode == QStringLiteral("tr")) return QStringLiteral("🇹🇷");
  if (langCode == QStringLiteral("es")) return QStringLiteral("🇪🇸");
  return QStringLiteral("🇬🇧");
}

QString MainWindow::langName(const QString &langCode) const {
  if (langCode == QStringLiteral("tr")) return tr("Türkçe");
  if (langCode == QStringLiteral("es")) return tr("Español");
  return tr("English");
}

void MainWindow::loadLanguage(const QString &langCode) {
  if (m_currentLang == langCode) return;
  m_currentLang = langCode;
  qApp->removeTranslator(&m_translator);
  if (!m_translator.load(QStringLiteral(":/translations/ro-assist_%1.qm").arg(langCode))) {
    qWarning("Failed to load translation file for: %s", qPrintable(langCode));
  }
  qApp->installTranslator(&m_translator);
}

void MainWindow::detectTheme() {
  currentTheme = Light;

#if QT_VERSION >= QT_VERSION_CHECK(6, 5, 0)
  if (const QStyleHints *hints = QGuiApplication::styleHints()) {
    if (hints->colorScheme() == Qt::ColorScheme::Dark)
      currentTheme = Dark;
  }
#else
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
  langBtn->setMinimumSize(130, 42);
  langBtn->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

  langMenu = new QMenu(langBtn);
  QAction *actTR = langMenu->addAction(QStringLiteral("🇹🇷 Türkçe"));
  actTR->setData(QStringLiteral("tr"));
  QAction *actEN = langMenu->addAction(QStringLiteral("🇬🇧 English"));
  actEN->setData(QStringLiteral("en"));
  QAction *actES = langMenu->addAction(QStringLiteral("🇪🇸 Español"));
  actES->setData(QStringLiteral("es"));

  langBtn->setMenu(langMenu);

  themeToggleBtn->setFixedSize(110, 42);
  connect(themeToggleBtn, &QPushButton::clicked, this,
          &MainWindow::toggleTheme);
  connect(langMenu, &QMenu::triggered, this, &MainWindow::changeLanguage);

  topLayout->addWidget(themeToggleBtn);
  topLayout->addWidget(langBtn);
  mainLayout->addWidget(topBarWidget);

  mainStack = new QStackedWidget(this);

  // 1. CAROUSEL
  carouselViewWidget = new QWidget(this);
  QHBoxLayout *carouselLayout = new QHBoxLayout(carouselViewWidget);
  carouselLayout->setContentsMargins(0, 0, 0, 0);
  prevSlideBtn = new QPushButton("◀", this);
  prevSlideBtn->setObjectName("navButton");
  prevSlideBtn->setCursor(Qt::PointingHandCursor);
  nextSlideBtn = new QPushButton("▶", this);
  nextSlideBtn->setObjectName("navButton");
  nextSlideBtn->setCursor(Qt::PointingHandCursor);
  connect(prevSlideBtn, &QPushButton::clicked, this, &MainWindow::prevSlide);
  connect(nextSlideBtn, &QPushButton::clicked, this, &MainWindow::nextSlide);
  carousel->setObjectName("panelWidget");
  createCarouselSlides();
  carouselLayout->addWidget(prevSlideBtn, 0, Qt::AlignCenter);
  carouselLayout->addSpacing(10);
  carouselLayout->addWidget(carousel, 1);
  carouselLayout->addSpacing(10);
  carouselLayout->addWidget(nextSlideBtn, 0, Qt::AlignCenter);

  // 2. UPDATE VIEW
  updateViewWidget = new QWidget(this);
  QVBoxLayout *updateLayout = new QVBoxLayout(updateViewWidget);
  updateLayout->setContentsMargins(20, 0, 20, 0);
  backToCarouselBtn = new QPushButton(this);
  backToCarouselBtn->setObjectName("backButton");
  backToCarouselBtn->setMinimumSize(120, 40);
  backToCarouselBtn->setCursor(Qt::PointingHandCursor);
  connect(backToCarouselBtn, &QPushButton::clicked, this,
          &MainWindow::showCarouselScreen);

  QHBoxLayout *updateTopLayout = new QHBoxLayout();
  updateTopLayout->addWidget(backToCarouselBtn);
  updateTopLayout->addStretch();

  QWidget *updatePanel = new QWidget(this);
  updatePanel->setObjectName("panelWidget");
  QVBoxLayout *panelLayout = new QVBoxLayout(updatePanel);
  panelLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

  versionLabel = new QLabel(QStringLiteral(APP_VERSION), this);
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
          &MainWindow::showCarouselScreen);

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
          &MainWindow::showCarouselScreen);

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
  appStorePlaceholderIcon = new QLabel(QStringLiteral("App Store"), this);
  appStorePlaceholderIcon->setAlignment(Qt::AlignCenter);
  appStorePlaceholderIcon->setStyleSheet(
      "font-size: 48px; font-weight: bold; color: palette(text); background-color: transparent; border: none;");
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
  mainStack->addWidget(carouselViewWidget); // 0
  mainStack->addWidget(updateViewWidget);   // 1
  mainStack->addWidget(libraryViewWidget);  // 2
  mainStack->addWidget(appStoreViewWidget); // 3

  mainLayout->addWidget(mainStack, 1);

  // BOTTOM BAR
  QWidget *bottomBarWidget = new QWidget(this);
  QHBoxLayout *aboutLayout = new QHBoxLayout(bottomBarWidget);
  aboutLayout->setContentsMargins(20, 10, 20, 20);

  aboutBtn = new QPushButton(tr("About"), this);
  aboutBtn->setObjectName("aboutButton");
  aboutBtn->setFixedSize(70, 50);
  aboutBtn->setCursor(Qt::PointingHandCursor);
  connect(aboutBtn, &QPushButton::clicked, this, &MainWindow::showAboutDialog);

  aboutLayout->addWidget(aboutBtn);
  aboutLayout->addStretch();
  mainLayout->addWidget(bottomBarWidget);
}

void MainWindow::createCarouselSlides() {
  // Slide 1: Update System
  QWidget *slide1 = new QWidget();
  QVBoxLayout *l1 = new QVBoxLayout(slide1);
  slide1Title = new QLabel(this);
  slide1Title->setObjectName("slideTitle");
  slide1Title->setAlignment(Qt::AlignCenter);
  slide1Title->setWordWrap(true);
  slide1Desc = new QLabel(this);
  slide1Desc->setObjectName("slideDesc");
  slide1Desc->setAlignment(Qt::AlignCenter);
  slide1Desc->setWordWrap(true);
  updateSlideBtn = new QPushButton(this);
  updateSlideBtn->setObjectName("actionButton");
  updateSlideBtn->setMinimumSize(280, 70);
  updateSlideBtn->setCursor(Qt::PointingHandCursor);
  connect(updateSlideBtn, &QPushButton::clicked, this, &MainWindow::showUpdateScreen);
  l1->addStretch();
  l1->addWidget(slide1Title);
  l1->addSpacing(20);
  l1->addWidget(slide1Desc);
  l1->addSpacing(40);
  l1->addWidget(updateSlideBtn, 0, Qt::AlignCenter);
  l1->addStretch();
  carousel->addWidget(slide1);

  // Slide 2: Social Media
  QWidget *slide2 = new QWidget();
  QVBoxLayout *l2 = new QVBoxLayout(slide2);
  slide2Title = new QLabel(this);
  slide2Title->setObjectName("slideTitle");
  slide2Title->setAlignment(Qt::AlignCenter);
  slide2Title->setWordWrap(true);

  QHBoxLayout *socialLayout = new QHBoxLayout();
  socialLayout->setSpacing(20);
  websiteBtn = new QToolButton(this);
  roAsdGitHubBtn = new QToolButton(this);
  roAssistGitHubBtn = new QToolButton(this);
  websiteBtn->setObjectName("squareSoftButton");
  roAsdGitHubBtn->setObjectName("squareSoftButton");
  roAssistGitHubBtn->setObjectName("squareSoftButton");

  websiteBtn->setFixedSize(160, 160);
  roAsdGitHubBtn->setFixedSize(160, 160);
  roAssistGitHubBtn->setFixedSize(160, 160);

  websiteBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  roAsdGitHubBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
  roAssistGitHubBtn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

  websiteBtn->setIconSize(QSize(64, 64));
  roAsdGitHubBtn->setIconSize(QSize(64, 64));
  roAssistGitHubBtn->setIconSize(QSize(64, 64));

  websiteBtn->setIcon(QIcon(":/icons/globe.svg"));
  roAsdGitHubBtn->setIcon(QIcon(":/icons/github.svg"));
  roAssistGitHubBtn->setIcon(QIcon(":/icons/github.svg"));
  websiteBtn->setCursor(Qt::PointingHandCursor);
  roAsdGitHubBtn->setCursor(Qt::PointingHandCursor);
  roAssistGitHubBtn->setCursor(Qt::PointingHandCursor);

  socialLayout->addStretch();
  socialLayout->addWidget(websiteBtn);
  socialLayout->addWidget(roAsdGitHubBtn);
  socialLayout->addWidget(roAssistGitHubBtn);
  socialLayout->addStretch();

  connect(websiteBtn, &QAbstractButton::clicked, this, &MainWindow::openWebsite);
  connect(roAsdGitHubBtn, &QAbstractButton::clicked, this,
          &MainWindow::openRoAsdGitHub);
  connect(roAssistGitHubBtn, &QAbstractButton::clicked, this,
          &MainWindow::openRoAssistGitHub);

  l2->addStretch();
  l2->addWidget(slide2Title);
  l2->addSpacing(30);
  l2->addLayout(socialLayout);
  l2->addStretch();
  carousel->addWidget(slide2);

  // Slide 3: App Store
  QWidget *slide3 = new QWidget();
  QVBoxLayout *l3 = new QVBoxLayout(slide3);
  slide3Title = new QLabel(this);
  slide3Title->setObjectName("slideTitle");
  slide3Title->setAlignment(Qt::AlignCenter);
  slide3Title->setWordWrap(true);
  slide3Desc = new QLabel(this);
  slide3Desc->setObjectName("slideDesc");
  slide3Desc->setAlignment(Qt::AlignCenter);
  slide3Desc->setWordWrap(true);
  appStoreSlideBtn = new QPushButton(this);
  appStoreSlideBtn->setObjectName("actionButton");
  appStoreSlideBtn->setMinimumSize(280, 70);
  appStoreSlideBtn->setCursor(Qt::PointingHandCursor);
  connect(appStoreSlideBtn, &QPushButton::clicked, this,
          &MainWindow::showAppStoreScreen);
  l3->addStretch();
  l3->addWidget(slide3Title);
  l3->addSpacing(20);
  l3->addWidget(slide3Desc);
  l3->addSpacing(40);
  l3->addWidget(appStoreSlideBtn, 0, Qt::AlignCenter);
  l3->addStretch();
  carousel->addWidget(slide3);

  // Slide 4: Bozok Community
  QWidget *slide4 = new QWidget();
  QVBoxLayout *l4 = new QVBoxLayout(slide4);
  slide4Title = new QLabel(this);
  slide4Title->setObjectName("slideTitle");
  slide4Title->setAlignment(Qt::AlignCenter);
  slide4Title->setWordWrap(true);
  slide4Desc = new QLabel(this);
  slide4Desc->setObjectName("slideDesc");
  slide4Desc->setAlignment(Qt::AlignCenter);
  slide4Desc->setWordWrap(true);
  bozokBtn = new QPushButton(this);
  bozokBtn->setObjectName("actionButton");
  bozokBtn->setMinimumSize(280, 70);
  bozokBtn->setCursor(Qt::PointingHandCursor);
  connect(bozokBtn, &QPushButton::clicked, this,
          &MainWindow::openBozokCommunity);
  l4->addStretch();
  l4->addWidget(slide4Title);
  l4->addSpacing(20);
  l4->addWidget(slide4Desc);
  l4->addSpacing(40);
  l4->addWidget(bozokBtn, 0, Qt::AlignCenter);
  l4->addStretch();
  carousel->addWidget(slide4);

  // Slide 5: Library Package
  QWidget *slide5 = new QWidget();
  QVBoxLayout *l5 = new QVBoxLayout(slide5);
  slide5Title = new QLabel(this);
  slide5Title->setObjectName("slideTitle");
  slide5Title->setAlignment(Qt::AlignCenter);
  slide5Title->setWordWrap(true);
  slide5Desc = new QLabel(this);
  slide5Desc->setObjectName("slideDesc");
  slide5Desc->setAlignment(Qt::AlignCenter);
  slide5Desc->setWordWrap(true);
  libraryPackageSlideBtn = new QPushButton(this);
  libraryPackageSlideBtn->setObjectName("actionButton");
  libraryPackageSlideBtn->setMinimumSize(280, 70);
  libraryPackageSlideBtn->setCursor(Qt::PointingHandCursor);
  connect(libraryPackageSlideBtn, &QPushButton::clicked, this,
          &MainWindow::showLibraryScreen);
  l5->addStretch();
  l5->addWidget(slide5Title);
  l5->addSpacing(20);
  l5->addWidget(slide5Desc);
  l5->addSpacing(40);
  l5->addWidget(libraryPackageSlideBtn, 0, Qt::AlignCenter);
  l5->addStretch();
  carousel->addWidget(slide5);
}

void MainWindow::updateUiTextAndImages() {
  const QString themeEmoji = currentTheme == Dark ? QStringLiteral("☀️ ") : QStringLiteral("🌙 ");
  themeToggleBtn->setText(themeEmoji + (currentTheme == Dark ? tr("Light") : tr("Dark")));

  langBtn->setText(langFlag(m_currentLang) + QStringLiteral(" ") + langName(m_currentLang));

  networkStatusLabel->setText(QStringLiteral("⚠ ") + tr("No Internet Connection"));

  backToCarouselBtn->setText(tr("Back"));
  backFromLibraryBtn->setText(tr("Back"));
  backFromAppStoreBtn->setText(tr("Back"));

  toggleLogBtn->setText(logConsole->isVisible() ? tr("Hide") : tr("Show Logs"));
  toggleLibraryLogBtn->setText(libraryLogConsole->isVisible() ? tr("Hide") : tr("Show Logs"));

  versionLabel->setText(
      QStringLiteral("%1: %2")
          .arg(tr("Current Version"), QStringLiteral(APP_VERSION)));

  updateButton->setText(tr("Update System"));
  slide1Title->setText(tr("Update Your System With One Click"));
  slide1Desc->setText(tr("Updates all system packages, including flatpak and snap."));
  updateSlideBtn->setText(tr("Go to Update Screen"));

  slide2Title->setText(tr("Follow Us on Social Media"));

  slide3Title->setText(tr("Discover Our App Store"));
  slide3Desc->setText(tr("Browse our custom application store."));
  appStoreSlideBtn->setText(tr("Go to App Store"));

  appStoreTitleLabel->setText(tr("Explore Our Custom App Store"));
  appStoreOpenAppBtn->setText(tr("Open App / Store"));

  slide4Title->setText(tr("Who Are We? Discover Our Community"));
  slide4Desc->setText(tr("As Yozgat Bozok University Open Source Software Development Club, we work for the ro-ASD project."));
  bozokBtn->setText(tr("Join the Club"));

  slide5Title->setText(tr("Game Library"));
  slide5Desc->setText(tr("You can download and update game libraries."));
  libraryPackageSlideBtn->setText(tr("Open Library Screen"));

  if (activeOperation != LibraryInstall) {
    libraryStatusLabel->setText(tr("Start Download"));
  }
  logConsole->setPlaceholderText(tr("Logs..."));
  libraryLogConsole->setPlaceholderText(tr("Logs..."));

  roAsdGitHubBtn->setText(QStringLiteral("ro-ASD OS\nRepo"));
  roAssistGitHubBtn->setText(QStringLiteral("ro-Assist\nRepo"));
  websiteBtn->setText(tr("Website"));
  aboutBtn->setText(tr("About"));

  if (activeOperation != LibraryInstall &&
      checkLibProcess->state() == QProcess::NotRunning) {
    libraryInstallButton->setText(isLibraryInstalled ? tr("Update Libraries") : tr("Download Libraries"));
  }

  setInitialUpdateStatus();
}

void MainWindow::setInitialUpdateStatus() {
  if (updateProcess->state() != QProcess::NotRunning || transactionPhaseStarted)
    return;

  statusLabel->setText(tr("Checking for updates..."));
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
        
        QPlainTextEdit#logConsole {
            background-color: palette(base); color: palette(text); border: 1px solid %5;
            border-radius: 8px; font-family: 'Cascadia Code', 'Consolas', monospace; font-size: 12px;
            padding: 8px;
        }
        QPushButton, QToolButton {
            padding: 8px 16px; font-size: 14px; font-weight: 600;
        }
        
        QPushButton#navButton { background-color: transparent; border: none; color: %4; font-size: 36px; font-weight: bold; }
        QPushButton#navButton:hover { color: %7; }
        
        QPushButton#backButton {
            background-color: palette(button); border: 1px solid %5; color: palette(button-text);
            border-radius: 8px; font-size: 14px; font-weight: bold;
        }
        QPushButton#backButton:hover { border: 1px solid %7; color: %7; }
        
        QPushButton#systemUpdateButton, QPushButton#libraryInstallButton, QPushButton#actionButton { 
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %7, stop:1 %8); 
            color: white; border: none; font-size: 18px; font-weight: 700; border-radius: 14px; 
            padding: 12px;
        }
        QPushButton#systemUpdateButton:hover, QPushButton#libraryInstallButton:hover, QPushButton#actionButton:hover { 
            background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 %9, stop:1 %7); 
        }
        QPushButton#systemUpdateButton:disabled, QPushButton#libraryInstallButton:disabled, QPushButton#actionButton:disabled { 
            background: %5; color: %4; 
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
        
        QPushButton#aboutButton { border-radius: 8px; font-size: 13px; font-weight: bold; border: 1px solid %5; background-color: transparent; padding: 4px 12px; }
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

void MainWindow::nextSlide() {
  int next = (carousel->currentIndex() + 1) % carousel->count();
  carousel->setCurrentIndex(next);
  carouselTimer->start(5000);
}

void MainWindow::prevSlide() {
  int prev =
      (carousel->currentIndex() - 1 + carousel->count()) % carousel->count();
  carousel->setCurrentIndex(prev);
  carouselTimer->start(5000);
}

void MainWindow::showUpdateScreen() {
  mainStack->setCurrentIndex(1);
  carouselTimer->stop();
}
void MainWindow::showLibraryScreen() {
  mainStack->setCurrentIndex(2);
  carouselTimer->stop();

  libraryInstallButton->setEnabled(false);
  libraryInstallButton->setText(tr("Checking..."));

  if (qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS") &&
      checkLibProcess->state() == QProcess::NotRunning) {
    checkLibProcess->start("rpm", QStringList()
                                      << "-q" << "gamemode" << "mangohud"
                                      << "vulkan-loader" << "vulkan-tools");
  } else if (!qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS")) {
    libraryInstallButton->setEnabled(true);
    libraryInstallButton->setText(isLibraryInstalled ? tr("Update Libraries") : tr("Download Libraries"));
  }
}
void MainWindow::showAppStoreScreen() {
  mainStack->setCurrentIndex(3);
  carouselTimer->stop();
}
void MainWindow::showCarouselScreen() {
  mainStack->setCurrentIndex(0);
  carouselTimer->start(5000);
  if (!transactionPhaseStarted &&
      checkUpdateProcess->state() == QProcess::NotRunning &&
      qEnvironmentVariableIsEmpty("RO_ASSIST_SKIP_SYSTEM_CHECKS")) {
    setInitialUpdateStatus();
    checkUpdateProcess->start("dnf", QStringList() << "check-update");
  }
}

void MainWindow::changeLanguage(QAction *action) {
  if (!action) return;
  loadLanguage(action->data().toString());
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
  QDesktopServices::openUrl(QUrl("https://github.com/orgs/Project-Ro-ASD/discussions"));
}
void MainWindow::showAboutDialog() {
  QDialog dialog(this);
  dialog.setWindowTitle(tr("About"));
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
  descLabel->setText(tr("Yozgat Bozok University Open Source Software Development Club ro-ASD project system management tool."));
  descLabel->setStyleSheet(
      "font-size: 16px; color: " +
      QString(currentTheme == Dark ? "#B9B4C7" : "#5C5470") + ";");

  QLabel *infoLabel = new QLabel(&dialog);
  infoLabel->setAlignment(Qt::AlignCenter);
  infoLabel->setText(
      QString("<span style='font-size: 15px;'><b>%1:</b> Ebubekir "
              "Bulut<br><br><b>%2:</b> 2026</span>")
          .arg(tr("Developer"))
          .arg(tr("Year")));
  infoLabel->setStyleSheet(
      "color: " + QString(currentTheme == Dark ? "#B9B4C7" : "#5C5470") + ";");

  QPushButton *okBtn = new QPushButton(tr("Close"), &dialog);
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
      this, tr("Store Version"), tr("Integration coming soon!"));
}

void MainWindow::startLibraryPackageInstall() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, tr("Error"), tr("No internet connection!"));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, tr("Info"), tr("Another operation is already running."));
    return;
  }

  libraryInstallButton->setEnabled(false);
  libraryProgressBar->setRange(0, 100);
  libraryProgressBar->setValue(0);
  libraryProgressBar->show();
  libraryStatusLabel->setText(tr("Installing libraries..."));

  libraryLogConsole->clear();
  appendLog(tr("Installing libraries..."), "#0066cc");
  setOperationRunning(LibraryInstall);

  QString action = isLibraryInstalled ? "upgrade" : "install";
  updateProcess->start("pkexec", QStringList()
                                     << "dnf" << action << "-y" << "gamemode"
                                     << "mangohud" << "vulkan-loader"
                                     << "vulkan-tools");
}

void MainWindow::startUpdate() {
  if (!isNetworkConnected) {
    QMessageBox::warning(
        this, tr("Error"), tr("No internet connection!"));
    return;
  }
  if (isOperationRunning()) {
    QMessageBox::information(
        this, tr("Info"), tr("Another operation is already running."));
    return;
  }

  updateButton->setEnabled(false);
  progressBar->setRange(0, 100);
  progressBar->setValue(0);
  progressBar->show();

  statusLabel->setText(tr("System update starting..."));
  logConsole->clear();
  appendLog(tr("System update starting..."), "#0066cc");
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

void MainWindow::checkDnfErrors(const QString &output) {
  if (output.contains("Waiting for process", Qt::CaseInsensitive) ||
      output.contains("Another app is currently holding the yum lock",
                      Qt::CaseInsensitive)) {
    QString msg = tr("System is busy...");
    if (isLibraryOperationActive())
      libraryStatusLabel->setText(msg);
    else
      statusLabel->setText(msg);
  }
  if (output.contains("Error: Failed to download metadata",
                      Qt::CaseInsensitive) ||
      output.contains("Could not resolve host", Qt::CaseInsensitive)) {
    QString msg = tr("Network Error");
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
    appendLog(line, currentTheme == Dark ? "#cccccc" : "#e60909");
  }
  checkDnfErrors(output);

  if (RoAssist::UpdateHelpers::containsNoWorkMarker(output)) {
    transactionPhaseStarted = true;
  }

  if (const auto progress =
          RoAssist::UpdateHelpers::parseTransactionProgress(output)) {
    transactionPhaseStarted = true;
    QString t = tr("Installing packages... (%1/%2)")
                    .arg(progress->current)
                    .arg(progress->total);
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
      QString t = tr("Downloading... %1%").arg(*percent);
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
    resetOperationUI(tr("Wrong Password!"), true);
    QMessageBox::critical(
        this, tr("Error"),
        tr("Wrong password or no sudo permission!"));
  }
}

void MainWindow::handleCheckUpdateFinished(int exitCode,
                                           QProcess::ExitStatus exitStatus) {
  if (transactionPhaseStarted)
    return;

  switch (RoAssist::UpdateHelpers::classifyCheckUpdateResult(exitCode,
                                                             exitStatus)) {
  case RoAssist::UpdateHelpers::UpdateCheckStatus::UpdatesAvailable:
    statusLabel->setText(tr("System Update Available"));
    break;
  case RoAssist::UpdateHelpers::UpdateCheckStatus::UpToDate:
    statusLabel->setText(tr("Your system is currently up to date!"));
    break;
  case RoAssist::UpdateHelpers::UpdateCheckStatus::Failed:
    statusLabel->setText(tr("Update check failed."));
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
      libraryStatusLabel->setText(tr("Process Completed!"));
      appendLog(tr("Libraries installed."), "#00cc00");
      updateUiTextAndImages();
    } else {
      if (!isTerminatingIntentionally) {
        libraryProgressBar->hide();
        libraryStatusLabel->setText(tr("Process Failed!"));
        appendLog(tr("Error occurred in process."), "#ff4444");
        if (!libraryLogConsole->isVisible())
          toggleLibraryLogs();
      }
    }
  } else {
    updateButton->setEnabled(true);
    if (exitStatus == QProcess::NormalExit && exitCode == 0) {
      progressBar->setRange(0, 100);
      progressBar->setValue(100);
      statusLabel->setText(tr("Process Completed!"));
      appendLog(tr("All operations completed successfully."), "#00cc00");
    } else {
      if (!isTerminatingIntentionally) {
        progressBar->hide();
        statusLabel->setText(tr("Process Failed!"));
        appendLog(tr("Error occurred during process."), "#ff4444");
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
          ? tr("Component failed to start.")
          : tr("Component crashed.");

  QMessageBox::critical(
      this, tr("Critical Error"), errorMsg);

  resetOperationUI(tr("Critical Error: ") + errorMsg, true);
  clearActiveOperation();
}
