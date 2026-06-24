#include "roassist/ui_texts.h"

namespace RoAssist::UiTexts {

namespace {

bool isTurkish(const QString &code) { return code == "tr"; }
bool isSpanish(const QString &code) { return code == "es"; }
bool isGerman(const QString &code) { return code == "de"; }
bool isFrench(const QString &code) { return code == "fr"; }

QString additionalTranslation(const QString &code, const QString &german,
                              const QString &french) {
  if (isGerman(code))
    return german;
  if (isFrench(code))
    return french;
  return {};
}

} // namespace

#define RETURN_ADDITIONAL_TRANSLATION(german, french)                         \
  if (const QString translation =                                             \
          additionalTranslation(languageCode, QStringLiteral(german),         \
                                QStringLiteral(french));                       \
      !translation.isNull())                                                   \
  return translation;

Bundle buildBundle(const QString &languageCode, bool darkTheme,
                   bool updateLogsVisible, bool libraryLogsVisible,
                   bool libraryInstalled) {
  const bool tr = isTurkish(languageCode);
  const bool es = isSpanish(languageCode);
  const bool de = isGerman(languageCode);
  const bool fr = isFrench(languageCode);

  Bundle bundle;
  bundle.languageButton = tr ? QStringLiteral("🇹🇷 Türkçe")
                             : (es ? QStringLiteral("🇪🇸 Español")
                                   : QStringLiteral("🇬🇧 English"));
  bundle.themeToggle = tr ? (darkTheme ? QStringLiteral("🌙 Koyu")
                                       : QStringLiteral("☀️ Açık"))
                          : (es ? (darkTheme ? QStringLiteral("🌙 Oscuro")
                                             : QStringLiteral("☀️ Claro"))
                                : (darkTheme ? QStringLiteral("🌙 Dark")
                                             : QStringLiteral("☀️ Light")));
  bundle.networkStatus = tr ? QStringLiteral("⚠ İnternet Bağlantısı Yok")
                            : (es ? QStringLiteral("⚠ Sin conexión a Internet")
                                  : QStringLiteral("⚠ No Internet Connection"));
  bundle.backButton =
      tr ? QStringLiteral("⬅ Geri")
         : (es ? QStringLiteral("⬅ Volver") : QStringLiteral("⬅ Back"));
  bundle.toggleLogs =
      tr ? (updateLogsVisible ? QStringLiteral("Gizle ⬆")
                              : QStringLiteral("Logları Göster ⬇"))
         : (es ? (updateLogsVisible ? QStringLiteral("Ocultar ⬆")
                                    : QStringLiteral("Mostrar Registros ⬇"))
               : (updateLogsVisible ? QStringLiteral("Hide ⬆")
                                    : QStringLiteral("Show Logs ⬇")));
  bundle.toggleLibraryLogs =
      tr ? (libraryLogsVisible ? QStringLiteral("Gizle ⬆")
                               : QStringLiteral("Logları Göster ⬇"))
         : (es ? (libraryLogsVisible ? QStringLiteral("Ocultar ⬆")
                                     : QStringLiteral("Mostrar Registros ⬇"))
               : (libraryLogsVisible ? QStringLiteral("Hide ⬆")
                                     : QStringLiteral("Show Logs ⬇")));
  bundle.versionLabel = tr ? QStringLiteral("Mevcut Sürüm: 0.1.1")
                           : (es ? QStringLiteral("Versión Actual: 0.1.1")
                                 : QStringLiteral("Current Version: 0.1.1"));
  bundle.updateButton = tr ? QStringLiteral("Sistemi Güncelle")
                           : (es ? QStringLiteral("Actualizar Sistema")
                                 : QStringLiteral("Update System"));
  bundle.welcomeNext = tr ? QStringLiteral("İleri")
                           : (es ? QStringLiteral("Siguiente")
                                 : QStringLiteral("Next"));
  bundle.welcomeFinish =
      tr ? QStringLiteral("ro-Assist'i Aç")
         : (es ? QStringLiteral("Abrir ro-Assist")
               : QStringLiteral("Open ro-Assist"));
  bundle.welcomeProgress = tr ? QStringLiteral("Adım %1 / %2")
                              : (es ? QStringLiteral("Paso %1 de %2")
                                    : QStringLiteral("Step %1 of %2"));
  bundle.dashboardGreeting =
      tr ? QStringLiteral("Ro-ASD'ye hoş geldiniz")
         : (es ? QStringLiteral("Bienvenido a Ro-ASD")
               : QStringLiteral("Welcome to Ro-ASD"));
  bundle.dashboardDescription =
      tr ? QStringLiteral("Yapmak istediğiniz işlemi seçin.")
         : (es ? QStringLiteral("Elige lo que quieres hacer.")
               : QStringLiteral("Choose what you want to do."));
  bundle.dashboardUpdateCard =
      tr ? QStringLiteral("Sistem Güncelleme\nPaketleri ve uygulamaları güncelleyin")
         : (es ? QStringLiteral("Actualización del sistema\nActualiza paquetes y aplicaciones")
               : QStringLiteral("System update\nUpdate packages and applications"));
  bundle.dashboardSocialCard =
      tr ? QStringLiteral("Bağlantılar ve destek\nWeb sitesi ve kaynak kodları")
         : (es ? QStringLiteral("Enlaces y soporte\nSitio web y código fuente")
               : QStringLiteral("Links and support\nWebsite and source code"));
  bundle.dashboardStoreCard =
      tr ? QStringLiteral("Uygulama Mağazası\nÖnerilen uygulamalara göz atın")
         : (es ? QStringLiteral("Tienda de aplicaciones\nExplora aplicaciones recomendadas")
               : QStringLiteral("App store\nBrowse recommended applications"));
  bundle.dashboardCommunityCard =
      tr ? QStringLiteral("Topluluk\nRo-ASD topluluğunu keşfedin")
         : (es ? QStringLiteral("Comunidad\nDescubre la comunidad Ro-ASD")
               : QStringLiteral("Community\nDiscover the Ro-ASD community"));
  bundle.dashboardLibraryCard =
      tr ? QStringLiteral("Oyun Kütüphaneleri\nOyun araçlarını kurun veya güncelleyin")
         : (es ? QStringLiteral("Bibliotecas de juegos\nInstala o actualiza herramientas de juego")
               : QStringLiteral("Game libraries\nInstall or update gaming tools"));
  bundle.dashboardPrinterCard =
      tr ? QStringLiteral("Yazıcı ve Tarayıcı\nSürücü ve tarama desteğini hazırlayın")
         : (es ? QStringLiteral("Impresora y escáner\nPrepara controladores y soporte de escaneo")
               : QStringLiteral("Printer and scanner\nPrepare driver and scanning support"));
  bundle.slide1Title = tr ? QStringLiteral("Sisteminizi Tek Tuşla Güncelleyin")
                          : (es ? QStringLiteral("Actualiza tu Sistema con un Clic")
                                : QStringLiteral("Update Your System With One Click"));
  bundle.slide1Description =
      tr ? QStringLiteral("Sistemdeki tüm paketleri, flatpak ve snap uygulamalarını günceller.")
         : (es ? QStringLiteral("Actualiza todos los paquetes del sistema, incluyendo flatpak y snap.")
               : QStringLiteral("Updates all system packages, including flatpak and snap."));
  bundle.updateSlideButton =
      tr ? QStringLiteral("🚀 Güncelleme Ekranına Git")
         : (es ? QStringLiteral("🚀 Ir a Pantalla de Actualización")
               : QStringLiteral("🚀 Go to Update Screen"));
  bundle.slide2Title = tr ? QStringLiteral("Sosyal Mecralardan Bizi Takip Edin")
                          : (es ? QStringLiteral("Síganos en las Redes Sociales")
                                : QStringLiteral("Follow Us on Social Media"));
  bundle.slide2Description =
      tr ? QStringLiteral("Yardım, dokümantasyon ve kaynak kodlarına tek yerden ulaşın.")
         : (es ? QStringLiteral("Accede a ayuda, documentación y código fuente desde un solo lugar.")
               : QStringLiteral("Access help, documentation and source code from one place."));
  bundle.slide3Title = tr ? QStringLiteral("Uygulama Mağazamızı Keşfedin")
                          : (es ? QStringLiteral("Descubre nuestra Tienda de Apps")
                                : QStringLiteral("Discover Our App Store"));
  bundle.slide3Description =
      tr ? QStringLiteral("Kendi mağazamızdan uygulamalara göz atın.")
         : (es ? QStringLiteral("Navega hasta nuestra tienda de aplicaciones personalizada.")
               : QStringLiteral("Browse our custom application store."));
  bundle.appStoreSlideButton =
      tr ? QStringLiteral("Mağazaya Git")
         : (es ? QStringLiteral("Ir a Tienda de Apps")
               : QStringLiteral("Go to App Store"));
  bundle.appStoreTitle =
      tr ? QStringLiteral("Özel Uygulama Mağazamızı Keşfedin")
         : (es ? QStringLiteral("Descubre Nuestra Tienda Especial")
               : QStringLiteral("Explore Our Custom App Store"));
  bundle.appStoreOpenButton =
      tr ? QStringLiteral("Mağazayı / Uygulamayı Aç")
         : (es ? QStringLiteral("Abrir Aplicación de la Tienda")
               : QStringLiteral("Open App / Store"));
  bundle.slide4Title =
      tr ? QStringLiteral("Biz Kimiz? Topluluğumuzu Keşfedin")
         : (es ? QStringLiteral("¿Quiénes somos? Nuestra comunidad")
               : QStringLiteral("Who Are We? Discover Our Community"));
  bundle.slide4Description =
      tr ? QStringLiteral("Yozgat Bozok Üniversitesi Açık Kaynak Yazılım Geliştirme Kulübü olarak ro-ASD projesi için çalışıyoruz.")
         : (es ? QStringLiteral("Como el Club de Desarrollo de Código Abierto de la Universidad de Yozgat Bozok, trabajamos para el proyecto ro-ASD.")
               : QStringLiteral("As Yozgat Bozok University Open Source Software Development Club, we work for the ro-ASD project."));
  bundle.communityButton = tr ? QStringLiteral("Kulübe Katıl")
                              : (es ? QStringLiteral("Únete al Club")
                                    : QStringLiteral("Join the Club"));
  bundle.slide5Title = tr ? QStringLiteral("Oyun Kütüphanesi")
                          : (es ? QStringLiteral("Biblioteca de Juegos")
                                : QStringLiteral("Game Library"));
  bundle.slide5Description =
      tr ? QStringLiteral("Oyun kütüphanelerini indirebilir ve güncelleyebilirsiniz.")
         : (es ? QStringLiteral("Puede descargar y actualizar bibliotecas para juegos.")
               : QStringLiteral("You can download and update game libraries."));
  bundle.librarySlideButton =
      tr ? QStringLiteral("Oyun Kütüphanesi Ekranına Geç")
         : (es ? QStringLiteral("Ir a Pantalla de Biblioteca de Juegos")
               : QStringLiteral("Open Library Screen"));
  bundle.libraryStatusIdle = tr ? QStringLiteral("İndirmeyi Başlat")
                                : (es ? QStringLiteral("Iniciar la Descarga")
                                      : QStringLiteral("Start Download"));
  bundle.logPlaceholder =
      tr ? QStringLiteral("Log kayıtları / Logs...")
         : (es ? QStringLiteral("Registros / Logs...") : QStringLiteral("Logs..."));
  bundle.websiteButton =
      tr ? QStringLiteral("Web Sitesi")
         : (es ? QStringLiteral("Sitio Web") : QStringLiteral("Website"));
  bundle.libraryActionButton =
      libraryInstalled
          ? (tr ? QStringLiteral("Kütüphaneleri Güncelle")
                : (es ? QStringLiteral("Actualizar bibliotecas")
                      : QStringLiteral("Update Libraries")))
          : (tr ? QStringLiteral("Kütüphaneleri İndir")
                : (es ? QStringLiteral("Descargar bibliotecas")
                      : QStringLiteral("Download Libraries")));
  bundle.checkingUpdates =
      tr ? QStringLiteral("Güncellemeler denetleniyor...")
         : (es ? QStringLiteral("Buscando actualizaciones...")
               : QStringLiteral("Checking for updates..."));
  bundle.printerSupportTitle =
      tr ? QStringLiteral("Yazıcı ve Tarayıcı Desteği")
         : (es ? QStringLiteral("Soporte para impresora y escáner")
               : QStringLiteral("Printer and scanner support"));
  bundle.printerSupportDescription =
      tr ? QStringLiteral("Ro-ASD, KDE Plasma için yaygın yazıcı sürücülerini ve tarama araçlarını kurabilir.")
         : (es ? QStringLiteral("Ro-ASD puede instalar controladores habituales y herramientas de escaneo para KDE Plasma.")
               : QStringLiteral("Ro-ASD can install common printer drivers and scanning tools for KDE Plasma."));
  bundle.printerSupportBenefits =
      tr ? QStringLiteral("• Modern USB ve ağ yazıcıları\n• HP, Samsung/SpliX, Brother, Canon ve Epson desteği\n• KDE Yazıcı Yöneticisi ve Skanpage tarama uygulaması")
         : (es ? QStringLiteral("• Impresoras USB y de red modernas\n• Soporte para HP, Samsung/SpliX, Brother, Canon y Epson\n• Administrador de impresión KDE y escaneo con Skanpage")
               : QStringLiteral("• Modern USB and network printers\n• HP, Samsung/SpliX, Brother, Canon and Epson support\n• KDE Print Manager and Skanpage scanning"));
  bundle.printerSupportStatusIdle =
      tr ? QStringLiteral("Geniş yazıcı desteği henüz kurulmadı.")
         : (es ? QStringLiteral("El soporte ampliado de impresión aún no está instalado.")
               : QStringLiteral("Extended printer support is not installed yet."));
  bundle.printerSupportInstallButton =
      tr ? QStringLiteral("Desteği Kur")
         : (es ? QStringLiteral("Instalar soporte") : QStringLiteral("Install support"));
  bundle.printerSupportLaterButton =
      tr ? QStringLiteral("Desteği Daha Sonra Hatırlat")
         : (es ? QStringLiteral("Recordarme el soporte más tarde")
               : QStringLiteral("Remind me about support later"));
  bundle.printerSupportDisableButton =
      tr ? QStringLiteral("Yazıcı Desteği İstemiyorum")
         : (es ? QStringLiteral("No quiero soporte de impresión")
               : QStringLiteral("I do not want printer support"));
  bundle.printerSupportOpenSettingsButton =
      tr ? QStringLiteral("Yazıcı Ayarlarını Aç")
         : (es ? QStringLiteral("Abrir ajustes de impresión")
               : QStringLiteral("Open printer settings"));
  bundle.printerSupportOpenScannerButton =
      tr ? QStringLiteral("Tarama Uygulamasını Aç")
         : (es ? QStringLiteral("Abrir aplicación de escaneo")
               : QStringLiteral("Open scanning application"));
  bundle.printerSupportInstalling =
      tr ? QStringLiteral("Yazıcı ve tarayıcı desteği kuruluyor...")
         : (es ? QStringLiteral("Instalando soporte para impresora y escáner...")
               : QStringLiteral("Installing printer and scanner support..."));
  bundle.printerSupportInstalled =
      tr ? QStringLiteral("Yazıcı ve tarayıcı desteği hazır.")
         : (es ? QStringLiteral("El soporte para impresora y escáner está listo.")
               : QStringLiteral("Printer and scanner support is ready."));
  bundle.printerSupportLaterSelected =
      tr ? QStringLiteral("Tercihiniz kaydedildi. Yazıcı takılınca bildirim özelliği Ro-Printer Agent ile daha sonra etkinleşecek.")
         : (es ? QStringLiteral("La preferencia se guardó. Las notificaciones al conectar una impresora se activarán con Ro-Printer Agent.")
               : QStringLiteral("Your preference was saved. Printer connection notifications will be enabled with Ro-Printer Agent."));
  bundle.printerSupportDisabled =
      tr ? QStringLiteral("Otomatik yazıcı desteği bildirimleri kapatıldı.")
         : (es ? QStringLiteral("Las notificaciones automáticas de impresión están desactivadas.")
               : QStringLiteral("Automatic printer support notifications are disabled."));
  bundle.printerSupportFailed =
      tr ? QStringLiteral("Yazıcı desteği kurulamadı. Bağlantınızı ve paket kaynağını kontrol edip tekrar deneyin.")
         : (es ? QStringLiteral("No se pudo instalar el soporte de impresión. Compruebe la conexión y el origen de paquetes.")
               : QStringLiteral("Printer support could not be installed. Check the connection and package source, then try again."));

  if (de) {
    bundle.languageButton = QStringLiteral("🇩🇪 Deutsch");
    bundle.themeToggle = darkTheme ? QStringLiteral("🌙 Dunkel") : QStringLiteral("☀️ Hell");
    bundle.networkStatus = QStringLiteral("⚠ Keine Internetverbindung");
    bundle.backButton = QStringLiteral("⬅ Zurück");
    bundle.toggleLogs = updateLogsVisible ? QStringLiteral("Ausblenden ⬆") : QStringLiteral("Protokolle anzeigen ⬇");
    bundle.toggleLibraryLogs = libraryLogsVisible ? QStringLiteral("Ausblenden ⬆") : QStringLiteral("Protokolle anzeigen ⬇");
    bundle.versionLabel = QStringLiteral("Aktuelle Version: 0.1.1");
    bundle.updateButton = QStringLiteral("System aktualisieren");
    bundle.welcomeNext = QStringLiteral("Weiter");
    bundle.welcomeFinish = QStringLiteral("ro-Assist öffnen");
    bundle.welcomeProgress = QStringLiteral("Schritt %1 von %2");
    bundle.dashboardGreeting = QStringLiteral("Willkommen bei Ro-ASD");
    bundle.dashboardDescription = QStringLiteral("Wählen Sie aus, was Sie tun möchten.");
    bundle.dashboardUpdateCard = QStringLiteral("Systemaktualisierung\nPakete und Anwendungen aktualisieren");
    bundle.dashboardSocialCard = QStringLiteral("Links und Unterstützung\nWebsite und Quellcode");
    bundle.dashboardStoreCard = QStringLiteral("Anwendungsspeicher\nEmpfohlene Anwendungen entdecken");
    bundle.dashboardCommunityCard = QStringLiteral("Gemeinschaft\nDie Ro-ASD-Gemeinschaft entdecken");
    bundle.dashboardLibraryCard = QStringLiteral("Spielebibliotheken\nSpielwerkzeuge installieren oder aktualisieren");
    bundle.dashboardPrinterCard = QStringLiteral("Drucker und Scanner\nTreiber- und Scanunterstützung einrichten");
    bundle.slide1Title = QStringLiteral("System mit einem Klick aktualisieren");
    bundle.slide1Description = QStringLiteral("Aktualisiert alle Systempakete sowie Flatpak- und Snap-Anwendungen.");
    bundle.updateSlideButton = QStringLiteral("🚀 Zur Aktualisierung");
    bundle.slide2Title = QStringLiteral("Links und Unterstützung");
    bundle.slide2Description = QStringLiteral("Hilfe, Dokumentation und Quellcode an einem Ort.");
    bundle.slide3Title = QStringLiteral("Unseren App-Store entdecken");
    bundle.slide3Description = QStringLiteral("Durchsuchen Sie unseren individuellen Anwendungsspeicher.");
    bundle.appStoreSlideButton = QStringLiteral("Zum Store");
    bundle.appStoreTitle = QStringLiteral("Unseren individuellen App-Store entdecken");
    bundle.appStoreOpenButton = QStringLiteral("App / Store öffnen");
    bundle.slide4Title = QStringLiteral("Wer sind wir? Entdecken Sie unsere Gemeinschaft");
    bundle.slide4Description = QStringLiteral("Als Open-Source-Softwareentwicklungsclub der Yozgat-Bozok-Universität arbeiten wir am Ro-ASD-Projekt.");
    bundle.communityButton = QStringLiteral("Dem Club beitreten");
    bundle.slide5Title = QStringLiteral("Spielebibliothek");
    bundle.slide5Description = QStringLiteral("Spielebibliotheken herunterladen und aktualisieren.");
    bundle.librarySlideButton = QStringLiteral("Spielebibliothek öffnen");
    bundle.libraryStatusIdle = QStringLiteral("Download starten");
    bundle.logPlaceholder = QStringLiteral("Protokolle ...");
    bundle.websiteButton = QStringLiteral("Website");
    bundle.libraryActionButton = libraryInstalled ? QStringLiteral("Bibliotheken aktualisieren") : QStringLiteral("Bibliotheken herunterladen");
    bundle.checkingUpdates = QStringLiteral("Suche nach Aktualisierungen ...");
    bundle.printerSupportTitle = QStringLiteral("Drucker- und Scannerunterstützung");
    bundle.printerSupportDescription = QStringLiteral("Ro-ASD kann gängige Druckertreiber und Scanwerkzeuge für KDE Plasma installieren.");
    bundle.printerSupportBenefits = QStringLiteral("• Moderne USB- und Netzwerkdrucker\n• Unterstützung für HP, Samsung/SpliX, Brother, Canon und Epson\n• KDE-Druckverwaltung und Skanpage");
    bundle.printerSupportStatusIdle = QStringLiteral("Erweiterte Druckerunterstützung ist noch nicht installiert.");
    bundle.printerSupportInstallButton = QStringLiteral("Unterstützung installieren");
    bundle.printerSupportLaterButton = QStringLiteral("Unterstützung später erinnern");
    bundle.printerSupportDisableButton = QStringLiteral("Keine Druckerunterstützung gewünscht");
    bundle.printerSupportOpenSettingsButton = QStringLiteral("Druckereinstellungen öffnen");
    bundle.printerSupportOpenScannerButton = QStringLiteral("Scan-Anwendung öffnen");
    bundle.printerSupportInstalling = QStringLiteral("Drucker- und Scannerunterstützung wird installiert ...");
    bundle.printerSupportInstalled = QStringLiteral("Drucker- und Scannerunterstützung ist bereit.");
    bundle.printerSupportLaterSelected = QStringLiteral("Ihre Auswahl wurde gespeichert. Druckerbenachrichtigungen werden mit Ro-Printer Agent aktiviert.");
    bundle.printerSupportDisabled = QStringLiteral("Automatische Druckerbenachrichtigungen sind deaktiviert.");
    bundle.printerSupportFailed = QStringLiteral("Druckerunterstützung konnte nicht installiert werden. Prüfen Sie Verbindung und Paketquelle.");
  } else if (fr) {
    bundle.languageButton = QStringLiteral("🇫🇷 Français");
    bundle.themeToggle = darkTheme ? QStringLiteral("🌙 Sombre") : QStringLiteral("☀️ Clair");
    bundle.networkStatus = QStringLiteral("⚠ Pas de connexion Internet");
    bundle.backButton = QStringLiteral("⬅ Retour");
    bundle.toggleLogs = updateLogsVisible ? QStringLiteral("Masquer ⬆") : QStringLiteral("Afficher les journaux ⬇");
    bundle.toggleLibraryLogs = libraryLogsVisible ? QStringLiteral("Masquer ⬆") : QStringLiteral("Afficher les journaux ⬇");
    bundle.versionLabel = QStringLiteral("Version actuelle : 0.1.1");
    bundle.updateButton = QStringLiteral("Mettre le système à jour");
    bundle.welcomeNext = QStringLiteral("Suivant");
    bundle.welcomeFinish = QStringLiteral("Ouvrir ro-Assist");
    bundle.welcomeProgress = QStringLiteral("Étape %1 sur %2");
    bundle.dashboardGreeting = QStringLiteral("Bienvenue dans Ro-ASD");
    bundle.dashboardDescription = QStringLiteral("Choisissez ce que vous voulez faire.");
    bundle.dashboardUpdateCard = QStringLiteral("Mise à jour du système\nMettre à jour les paquets et les applications");
    bundle.dashboardSocialCard = QStringLiteral("Liens et assistance\nSite web et code source");
    bundle.dashboardStoreCard = QStringLiteral("Magasin d'applications\nDécouvrir les applications recommandées");
    bundle.dashboardCommunityCard = QStringLiteral("Communauté\nDécouvrir la communauté Ro-ASD");
    bundle.dashboardLibraryCard = QStringLiteral("Bibliothèques de jeux\nInstaller ou mettre à jour les outils de jeu");
    bundle.dashboardPrinterCard = QStringLiteral("Imprimante et scanner\nPréparer les pilotes et la numérisation");
    bundle.slide1Title = QStringLiteral("Mettre votre système à jour en un clic");
    bundle.slide1Description = QStringLiteral("Met à jour tous les paquets système, ainsi que les applications Flatpak et Snap.");
    bundle.updateSlideButton = QStringLiteral("🚀 Ouvrir la mise à jour");
    bundle.slide2Title = QStringLiteral("Liens et assistance");
    bundle.slide2Description = QStringLiteral("Accédez à l'aide, la documentation et au code source depuis un seul endroit.");
    bundle.slide3Title = QStringLiteral("Découvrir notre magasin d'applications");
    bundle.slide3Description = QStringLiteral("Parcourez notre magasin d'applications personnalisé.");
    bundle.appStoreSlideButton = QStringLiteral("Accéder au magasin");
    bundle.appStoreTitle = QStringLiteral("Découvrir notre magasin d'applications personnalisé");
    bundle.appStoreOpenButton = QStringLiteral("Ouvrir l'application / le magasin");
    bundle.slide4Title = QStringLiteral("Qui sommes-nous ? Découvrez notre communauté");
    bundle.slide4Description = QStringLiteral("Nous travaillons sur le projet Ro-ASD au sein du club de développement de logiciels libres de l'université Yozgat Bozok.");
    bundle.communityButton = QStringLiteral("Rejoindre le club");
    bundle.slide5Title = QStringLiteral("Bibliothèque de jeux");
    bundle.slide5Description = QStringLiteral("Téléchargez et mettez à jour les bibliothèques de jeux.");
    bundle.librarySlideButton = QStringLiteral("Ouvrir la bibliothèque de jeux");
    bundle.libraryStatusIdle = QStringLiteral("Démarrer le téléchargement");
    bundle.logPlaceholder = QStringLiteral("Journaux ...");
    bundle.websiteButton = QStringLiteral("Site web");
    bundle.libraryActionButton = libraryInstalled ? QStringLiteral("Mettre à jour les bibliothèques") : QStringLiteral("Télécharger les bibliothèques");
    bundle.checkingUpdates = QStringLiteral("Recherche des mises à jour ...");
    bundle.printerSupportTitle = QStringLiteral("Prise en charge des imprimantes et scanners");
    bundle.printerSupportDescription = QStringLiteral("Ro-ASD peut installer les pilotes d'imprimante et outils de numérisation courants pour KDE Plasma.");
    bundle.printerSupportBenefits = QStringLiteral("• Imprimantes USB et réseau modernes\n• Prise en charge HP, Samsung/SpliX, Brother, Canon et Epson\n• Gestionnaire d'impression KDE et Skanpage");
    bundle.printerSupportStatusIdle = QStringLiteral("La prise en charge étendue des imprimantes n'est pas encore installée.");
    bundle.printerSupportInstallButton = QStringLiteral("Installer la prise en charge");
    bundle.printerSupportLaterButton = QStringLiteral("Me rappeler la prise en charge plus tard");
    bundle.printerSupportDisableButton = QStringLiteral("Je ne souhaite pas de prise en charge d'imprimante");
    bundle.printerSupportOpenSettingsButton = QStringLiteral("Ouvrir les paramètres d'impression");
    bundle.printerSupportOpenScannerButton = QStringLiteral("Ouvrir l'application de numérisation");
    bundle.printerSupportInstalling = QStringLiteral("Installation de la prise en charge des imprimantes et scanners ...");
    bundle.printerSupportInstalled = QStringLiteral("La prise en charge des imprimantes et scanners est prête.");
    bundle.printerSupportLaterSelected = QStringLiteral("Votre préférence est enregistrée. Les notifications seront activées avec Ro-Printer Agent.");
    bundle.printerSupportDisabled = QStringLiteral("Les notifications automatiques d'imprimante sont désactivées.");
    bundle.printerSupportFailed = QStringLiteral("La prise en charge de l'imprimante n'a pas pu être installée. Vérifiez la connexion et la source des paquets.");
  }
  return bundle;
}

QString aboutTitle(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Über", "À propos")
  return isTurkish(languageCode)
             ? QStringLiteral("Hakkında")
             : (isSpanish(languageCode) ? QStringLiteral("Acerca de")
                                        : QStringLiteral("About"));
}

QString aboutDescription(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Systemverwaltungswerkzeug des Ro-ASD-Projekts vom Open-Source-Softwareentwicklungsclub der Yozgat-Bozok-Universität.", "Outil de gestion du système du projet Ro-ASD par le club de développement de logiciels libres de l'université Yozgat Bozok.")
  return isTurkish(languageCode)
             ? QStringLiteral("Yozgat Bozok Üniversitesi Açık Kaynak Yazılım Geliştirme Kulübü ro-ASD projesi sistem yönetim aracı.")
             : (isSpanish(languageCode)
                    ? QStringLiteral("Herramienta de gestión del sistema del proyecto ro-ASD del Club de Desarrollo de Software de Código Abierto de la Universidad Yozgat Bozok.")
                    : QStringLiteral("Yozgat Bozok University Open Source Software Development Club ro-ASD project system management tool."));
}

QString developerLabel(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Entwickler", "Développeur")
  return isTurkish(languageCode)
             ? QStringLiteral("Geliştirici")
             : (isSpanish(languageCode) ? QStringLiteral("Desarrollador")
                                        : QStringLiteral("Developer"));
}

QString yearLabel(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Jahr", "Année")
  return isTurkish(languageCode)
             ? QStringLiteral("Yıl")
             : (isSpanish(languageCode) ? QStringLiteral("Año")
                                        : QStringLiteral("Year"));
}

QString closeLabel(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Schließen", "Fermer")
  return isTurkish(languageCode)
             ? QStringLiteral("Kapat")
             : (isSpanish(languageCode) ? QStringLiteral("Cerrar")
                                        : QStringLiteral("Close"));
}

QString storeVersionTitle(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Store-Version", "Version du magasin")
  return isTurkish(languageCode)
             ? QStringLiteral("Mağaza Sürümü")
             : (isSpanish(languageCode) ? QStringLiteral("Versión de la Tienda")
                                        : QStringLiteral("Store Version"));
}

QString storeVersionMessage(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Diese Funktion wird bald verfügbar sein!", "Cette fonctionnalité sera bientôt disponible !")
  return isTurkish(languageCode)
             ? QStringLiteral("Bu sistem yakında eklenecektir!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Integración próximamente!")
                                        : QStringLiteral("Integration coming soon!"));
}

QString errorTitle(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Fehler", "Erreur")
  return isTurkish(languageCode)
             ? QStringLiteral("Hata")
             : (isSpanish(languageCode) ? QStringLiteral("Error")
                                        : QStringLiteral("Error"));
}

QString infoTitle(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Information", "Information")
  return isTurkish(languageCode)
             ? QStringLiteral("Bilgi")
             : (isSpanish(languageCode) ? QStringLiteral("Información")
                                        : QStringLiteral("Info"));
}

QString criticalErrorTitle(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Kritischer Fehler", "Erreur critique")
  return isTurkish(languageCode)
             ? QStringLiteral("Kritik Hata")
             : (isSpanish(languageCode) ? QStringLiteral("Error crítico")
                                        : QStringLiteral("Critical Error"));
}

QString noInternetMessage(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Keine Internetverbindung!", "Pas de connexion Internet !")
  return isTurkish(languageCode)
             ? QStringLiteral("İnternet bağlantısı yok!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Sin conexión a internet!")
                                        : QStringLiteral("No internet connection!"));
}

QString operationRunningMessage(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Ein anderer Vorgang wird bereits ausgeführt.", "Une autre opération est déjà en cours.")
  return isTurkish(languageCode)
             ? QStringLiteral("Başka bir işlem zaten çalışıyor.")
             : (isSpanish(languageCode) ? QStringLiteral("Ya hay otro proceso en ejecución.")
                                        : QStringLiteral("Another operation is already running."));
}

QString librariesInstalling(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Bibliotheken werden installiert ...", "Installation des bibliothèques ...")
  return isTurkish(languageCode)
             ? QStringLiteral("Kütüphaneler kuruluyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Instalando bibliotecas...")
                                        : QStringLiteral("Installing libraries..."));
}

QString updateStarting(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Systemaktualisierung wird gestartet ...", "Démarrage de la mise à jour du système ...")
  return isTurkish(languageCode)
             ? QStringLiteral("Sistem güncellemesi başlatılıyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Comenzando actualización del sistema...")
                                        : QStringLiteral("System update starting..."));
}

QString librariesInstalled(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Bibliotheken wurden installiert.", "Bibliothèques installées.")
  return isTurkish(languageCode)
             ? QStringLiteral("Kütüphaneler kuruldu.")
             : (isSpanish(languageCode) ? QStringLiteral("Bibliotecas instaladas.")
                                        : QStringLiteral("Libraries installed."));
}

QString allOperationsCompleted(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Alle Vorgänge wurden erfolgreich abgeschlossen.", "Toutes les opérations ont été effectuées avec succès.")
  return isTurkish(languageCode)
             ? QStringLiteral("Tüm işlemler başarıyla tamamlandı.")
             : (isSpanish(languageCode)
                    ? QStringLiteral("Todas las operaciones se completaron con éxito.")
                    : QStringLiteral("All operations completed successfully."));
}

QString processFailed(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Vorgang fehlgeschlagen!", "Échec de l'opération !")
  return isTurkish(languageCode)
             ? QStringLiteral("İşlem Başarısız!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Proceso fallido!")
                                        : QStringLiteral("Process Failed!"));
}

QString processFailedDetails(const QString &languageCode, bool libraryOperation) {
  if (isGerman(languageCode))
    return libraryOperation ? QStringLiteral("Im Vorgang ist ein Fehler aufgetreten.")
                            : QStringLiteral("Während des Vorgangs ist ein Fehler aufgetreten.");
  if (isFrench(languageCode))
    return libraryOperation ? QStringLiteral("Une erreur est survenue pendant l'opération.")
                            : QStringLiteral("Une erreur est survenue lors de l'opération.");
  if (isTurkish(languageCode)) {
    return QStringLiteral("İşlem sırasında hata oluştu.");
  }
  if (isSpanish(languageCode)) {
    return libraryOperation ? QStringLiteral("Ocurrió un error en el proceso.")
                            : QStringLiteral("Ocurrió un error durante el proceso.");
  }
  return libraryOperation ? QStringLiteral("Error occurred in process.")
                          : QStringLiteral("Error occurred during process.");
}

QString systemBusy(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("⚠️ Das System ist derzeit mit einem anderen Vorgang beschäftigt ...", "⚠️ Le système est actuellement occupé par une autre opération ...")
  return isTurkish(languageCode)
             ? QStringLiteral("⚠️ Sistem şu an başka bir işlemle meşgul...")
             : (isSpanish(languageCode) ? QStringLiteral("⚠️ El sistema está ocupado...")
                                        : QStringLiteral("⚠️ System is busy..."));
}

QString networkError(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("🌐 Netzwerkfehler", "🌐 Erreur réseau")
  return isTurkish(languageCode)
             ? QStringLiteral("🌐 İnternet Hatası")
             : (isSpanish(languageCode) ? QStringLiteral("🌐 Error de red")
                                        : QStringLiteral("🌐 Network Error"));
}

QString wrongPasswordShort(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("❌ Falsches Passwort!", "❌ Mot de passe incorrect !")
  return isTurkish(languageCode)
             ? QStringLiteral("❌ Şifre Yanlış!")
             : (isSpanish(languageCode) ? QStringLiteral("❌ ¡Contraseña incorrecta!")
                                        : QStringLiteral("❌ Wrong Password!"));
}

QString wrongPasswordDetail(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Sie haben ein falsches Passwort eingegeben oder besitzen keine sudo-Berechtigung!", "Vous avez saisi un mot de passe incorrect ou vous n'avez pas les droits sudo !")
  return isTurkish(languageCode)
             ? QStringLiteral("Yanlış şifre girdiniz veya sudo yetkiniz yok!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Contraseña incorrecta o sin permisos de sudo!")
                                        : QStringLiteral("Wrong password or no sudo permission!"));
}

QString checkingLibraries(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Wird geprüft ...", "Vérification en cours ...")
  return isTurkish(languageCode)
             ? QStringLiteral("Kontrol ediliyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Comprobando...")
                                        : QStringLiteral("Checking..."));
}

QString updateCheckFailed(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Aktualisierungsprüfung fehlgeschlagen.", "Échec de la vérification des mises à jour.")
  return isTurkish(languageCode)
             ? QStringLiteral("Güncelleme denetimi başarısız oldu.")
             : (isSpanish(languageCode) ? QStringLiteral("La comprobación de actualizaciones falló.")
                                        : QStringLiteral("Update check failed."));
}

QString updateAvailable(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Systemaktualisierung verfügbar", "Mise à jour du système disponible")
  return isTurkish(languageCode)
             ? QStringLiteral("Sistem Güncellemesi Mevcut")
             : (isSpanish(languageCode) ? QStringLiteral("Actualización del Sistema Disponible")
                                        : QStringLiteral("System Update Available"));
}

QString systemUpToDate(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Ihr System ist auf dem neuesten Stand!", "Votre système est à jour !")
  return isTurkish(languageCode)
             ? QStringLiteral("Sisteminiz şu anda güncel!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Su sistema está actualizado!")
                                        : QStringLiteral("Your system is currently up to date!"));
}

QString downloading(const QString &languageCode, int percent) {
  const QString prefix =
      isTurkish(languageCode)
          ? QStringLiteral("İndiriliyor...")
          : (isSpanish(languageCode) ? QStringLiteral("Descargando...")
                                     : (isGerman(languageCode) ? QStringLiteral("Wird heruntergeladen ...")
                                                                : (isFrench(languageCode) ? QStringLiteral("Téléchargement ...")
                                                                                         : QStringLiteral("Downloading..."))));
  return QStringLiteral("%1 %2%").arg(prefix).arg(percent);
}

QString installingPackages(const QString &languageCode, int current, int total) {
  const QString prefix =
      isTurkish(languageCode)
          ? QStringLiteral("Paketler kuruluyor...")
          : (isSpanish(languageCode) ? QStringLiteral("Instalando paquetes...")
                                     : (isGerman(languageCode) ? QStringLiteral("Pakete werden installiert ...")
                                                                : (isFrench(languageCode) ? QStringLiteral("Installation des paquets ...")
                                                                                         : QStringLiteral("Installing packages..."))));
  return QStringLiteral("%1 (%2/%3)").arg(prefix).arg(current).arg(total);
}

QString componentFailedToStart(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Komponente konnte nicht gestartet werden.", "Le composant n'a pas pu démarrer.")
  return isTurkish(languageCode)
             ? QStringLiteral("Bileşen başlatılamadı.")
             : (isSpanish(languageCode) ? QStringLiteral("El componente no pudo iniciarse.")
                                        : QStringLiteral("Component failed to start."));
}

QString componentCrashed(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Komponente ist abgestürzt.", "Le composant a cessé de fonctionner.")
  return isTurkish(languageCode)
             ? QStringLiteral("Bileşen çöktü.")
             : (isSpanish(languageCode) ? QStringLiteral("El componente falló.")
                                        : QStringLiteral("Component crashed."));
}

QString criticalErrorPrefix(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Kritischer Fehler: ", "Erreur critique : ")
  return isTurkish(languageCode)
             ? QStringLiteral("Kritik Hata: ")
             : (isSpanish(languageCode) ? QStringLiteral("Error crítico: ")
                                        : QStringLiteral("Critical Error: "));
}

QString processCompleted(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("✅ Vorgang erfolgreich abgeschlossen!", "✅ Opération terminée avec succès !")
  return isTurkish(languageCode)
             ? QStringLiteral("✅ İşlem Başarıyla Tamamlandı!")
             : (isSpanish(languageCode) ? QStringLiteral("✅ ¡Completado con Éxito!")
                                        : QStringLiteral("✅ Process Completed!"));
}

#undef RETURN_ADDITIONAL_TRANSLATION

} // namespace RoAssist::UiTexts
