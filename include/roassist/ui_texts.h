#ifndef ROASSIST_UI_TEXTS_H
#define ROASSIST_UI_TEXTS_H

#include <QString>

namespace RoAssist::UiTexts {

struct Bundle {
  QString languageButton;
  QString themeToggle;
  QString networkStatus;
  QString backButton;
  QString toggleLogs;
  QString toggleLibraryLogs;
  QString updateButton;
  QString welcomeNext;
  QString welcomeFinish;
  QString welcomeProgress;
  QString dashboardGreeting;
  QString dashboardDescription;
  QString dashboardUpdateCard;
  QString dashboardSocialCard;
  QString dashboardStoreCard;
  QString dashboardCommunityCard;
  QString dashboardLibraryCard;
  QString dashboardPrinterCard;
  QString dashboardTelemetryCard;
  QString slide1Title;
  QString slide1Description;
  QString updateSlideButton;
  QString slide2Title;
  QString slide2Description;
  QString slide3Title;
  QString slide3Description;
  QString appStoreSlideButton;
  QString appStoreTitle;
  QString appStoreOpenButton;
  QString slide4Title;
  QString slide4Description;
  QString communityButton;
  QString slide5Title;
  QString slide5Description;
  QString librarySlideButton;
  QString slide6Title;
  QString slide6Description;
  QString libraryStatusIdle;
  QString logPlaceholder;
  QString websiteButton;
  QString libraryActionButton;
  QString checkingUpdates;
  QString printerSupportTitle;
  QString printerSupportDescription;
  QString printerSupportBenefits;
  QString printerSupportStatusIdle;
  QString printerSupportInstallButton;
  QString printerSupportLaterButton;
  QString printerSupportDisableButton;
  QString printerSupportOpenSettingsButton;
  QString printerSupportOpenScannerButton;
  QString printerSupportInstalling;
  QString printerSupportInstalled;
  QString printerSupportLaterSelected;
  QString printerSupportDisabled;
  QString printerSupportFailed;
  QString telemetryTitle;
  QString telemetryIntro;
  QString telemetryPurposeTitle;
  QString telemetryPurposeText;
  QString telemetryLevelsTitle;
  QString telemetryLevelsText;
  QString telemetryDoesNotCollectTitle;
  QString telemetryDoesNotCollectText;
  QString telemetrySliderTitle;
  QString telemetryCurrentLevel;
  QString telemetryLevelOff;
  QString telemetryLevelCount;
  QString telemetryLevelBasic;
  QString telemetryLevelExtended;
  QString telemetryDescriptionOff;
  QString telemetryDescriptionCount;
  QString telemetryDescriptionBasic;
  QString telemetryDescriptionExtended;
};

Bundle buildBundle(const QString &languageCode, bool darkTheme,
                   bool updateLogsVisible, bool libraryLogsVisible,
                   bool libraryInstalled);

QString aboutTitle(const QString &languageCode);
QString aboutDescription(const QString &languageCode);
QString developerLabel(const QString &languageCode);
QString yearLabel(const QString &languageCode);
QString closeLabel(const QString &languageCode);
QString versionText(const QString &languageCode, const QString &version);

QString storeVersionTitle(const QString &languageCode);
QString storeVersionMessage(const QString &languageCode);

QString errorTitle(const QString &languageCode);
QString infoTitle(const QString &languageCode);
QString criticalErrorTitle(const QString &languageCode);
QString noInternetMessage(const QString &languageCode);
QString operationRunningMessage(const QString &languageCode);
QString librariesInstalling(const QString &languageCode);
QString updateStarting(const QString &languageCode);
QString librariesInstalled(const QString &languageCode);
QString allOperationsCompleted(const QString &languageCode);
QString processFailed(const QString &languageCode);
QString processFailedDetails(const QString &languageCode, bool libraryOperation);
QString systemBusy(const QString &languageCode);
QString networkError(const QString &languageCode);
QString wrongPasswordShort(const QString &languageCode);
QString wrongPasswordDetail(const QString &languageCode);
QString checkingLibraries(const QString &languageCode);
QString updateCheckFailed(const QString &languageCode);
QString updateAvailable(const QString &languageCode);
QString systemUpToDate(const QString &languageCode);
QString downloading(const QString &languageCode, int percent);
QString installingPackages(const QString &languageCode, int current, int total);
QString componentFailedToStart(const QString &languageCode);
QString componentCrashed(const QString &languageCode);
QString criticalErrorPrefix(const QString &languageCode);
QString processCompleted(const QString &languageCode);
QString maintenanceReady(const QString &languageCode);
QString maintenancePlanSummary(const QString &languageCode,
                               bool flatpakAvailable, bool snapAvailable);
QString maintenanceNoKnownRisk(const QString &languageCode);
QString nouveauRiskWarning(const QString &languageCode,
                           bool roControlAvailable);
QString lowDiskWarning(const QString &languageCode);
QString rebootRequiredWarning(const QString &languageCode);
QString roControlButtonText(const QString &languageCode,
                            bool roControlAvailable);
QString roControlMissingMessage(const QString &languageCode);
QString updateStepStarting(const QString &languageCode, const QString &label,
                           int current, int total);
QString updatePartialFailure(const QString &languageCode);
QString updateNeedsReboot(const QString &languageCode);
QString updateAborted(const QString &languageCode);

} // namespace RoAssist::UiTexts

#endif // ROASSIST_UI_TEXTS_H
