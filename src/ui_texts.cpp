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
      tr ? QStringLiteral("Sistemi güvenli biçimde hazır tutmak için bir bakım adımı seçin.")
         : (es ? QStringLiteral("Elige un paso de mantenimiento para mantener el sistema preparado.")
               : QStringLiteral("Choose a maintenance step to keep the system ready."));
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
      tr ? QStringLiteral("Oyun için temel araçlar\nGameMode, MangoHud ve Vulkan araçları")
         : (es ? QStringLiteral("Herramientas básicas de juego\nGameMode, MangoHud y herramientas Vulkan")
               : QStringLiteral("Basic gaming tools\nGameMode, MangoHud and Vulkan tools"));
  bundle.dashboardPrinterCard =
      tr ? QStringLiteral("Yazıcı ve Tarayıcı\nSürücü ve tarama desteğini hazırlayın")
         : (es ? QStringLiteral("Impresora y escáner\nPrepara controladores y soporte de escaneo")
               : QStringLiteral("Printer and scanner\nPrepare driver and scanning support"));
  bundle.dashboardTelemetryCard =
      tr ? QStringLiteral("Gizlilik ve Telemetri\nVeri paylaşım tercihinizi yönetin")
         : (es ? QStringLiteral("Privacidad y telemetría\nGestiona tu preferencia de datos")
               : QStringLiteral("Privacy and telemetry\nManage your data sharing preference"));
  bundle.slide1Title = tr ? QStringLiteral("Sisteminizi Kontrollü Güncelleyin")
                          : (es ? QStringLiteral("Actualiza el sistema con control")
                                : QStringLiteral("Update Your System With Control"));
  bundle.slide1Description =
      tr ? QStringLiteral("DNF, Flatpak ve Snap adımlarını ayrı izleyerek güncelleme risklerini açıkça gösterir.")
         : (es ? QStringLiteral("Muestra los riesgos y sigue DNF, Flatpak y Snap como pasos separados.")
               : QStringLiteral("Shows update risks and tracks DNF, Flatpak and Snap as separate steps."));
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
  bundle.slide3Title = tr ? QStringLiteral("Önerilen Uygulamaları Keşfedin")
                          : (es ? QStringLiteral("Descubre aplicaciones recomendadas")
                                : QStringLiteral("Discover Recommended Apps"));
  bundle.slide3Description =
      tr ? QStringLiteral("Ro-ASD için önerilen uygulama kategorilerine göz atın; kurulum için Discover veya dağıtım kaynakları kullanılabilir.")
         : (es ? QStringLiteral("Revise categorías recomendadas para Ro-ASD; la instalación puede hacerse con Discover o fuentes de la distribución.")
               : QStringLiteral("Browse recommended categories for Ro-ASD; installation can use Discover or distribution sources."));
  bundle.appStoreSlideButton =
      tr ? QStringLiteral("Mağazaya Git")
         : (es ? QStringLiteral("Ir a Tienda de Apps")
               : QStringLiteral("Go to App Store"));
  bundle.appStoreTitle =
      tr ? QStringLiteral("Önerilen Uygulamalar")
         : (es ? QStringLiteral("Aplicaciones recomendadas")
               : QStringLiteral("Recommended Applications"));
  bundle.appStoreOpenButton =
      tr ? QStringLiteral("Discover'ı Aç")
         : (es ? QStringLiteral("Abrir Discover")
               : QStringLiteral("Open Discover"));
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
  bundle.slide5Title = tr ? QStringLiteral("Oyun için Temel Araçlar")
                          : (es ? QStringLiteral("Herramientas básicas de juego")
                                : QStringLiteral("Basic Gaming Tools"));
  bundle.slide5Description =
      tr ? QStringLiteral("GameMode, MangoHud ve Vulkan araçlarını kurabilir veya güncelleyebilirsiniz. Sürücü kurulumu ro Control'e bırakılır.")
         : (es ? QStringLiteral("Puede instalar o actualizar GameMode, MangoHud y herramientas Vulkan. Los controladores se dejan a ro Control.")
               : QStringLiteral("Install or update GameMode, MangoHud and Vulkan tools. Driver handling stays in ro Control."));
  bundle.librarySlideButton =
      tr ? QStringLiteral("Oyun Araçlarını Aç")
         : (es ? QStringLiteral("Abrir herramientas de juego")
               : QStringLiteral("Open Gaming Tools"));
  bundle.slide6Title =
      tr ? QStringLiteral("Gizlilik ve Telemetri")
         : (es ? QStringLiteral("Privacidad y telemetría")
               : QStringLiteral("Privacy and telemetry"));
  bundle.slide6Description =
      tr ? QStringLiteral("Ro-ASD'nin sorunları teşhis etmesine yardımcı olacak veri paylaşım seviyesini seçin.")
         : (es ? QStringLiteral("Elige el nivel de datos que ayudará a Ro-ASD a diagnosticar problemas.")
               : QStringLiteral("Choose the data sharing level that will help Ro-ASD diagnose problems."));
  bundle.libraryStatusIdle = tr ? QStringLiteral("GameMode, MangoHud ve Vulkan araçları hazırlanabilir.")
                                : (es ? QStringLiteral("GameMode, MangoHud y herramientas Vulkan se pueden preparar.")
                                      : QStringLiteral("GameMode, MangoHud and Vulkan tools can be prepared."));
  bundle.logPlaceholder =
      tr ? QStringLiteral("Log kayıtları / Logs...")
         : (es ? QStringLiteral("Registros / Logs...") : QStringLiteral("Logs..."));
  bundle.websiteButton =
      tr ? QStringLiteral("Web Sitesi")
         : (es ? QStringLiteral("Sitio Web") : QStringLiteral("Website"));
  bundle.libraryActionButton =
      libraryInstalled
          ? (tr ? QStringLiteral("Araçları Güncelle")
                : (es ? QStringLiteral("Actualizar herramientas")
                      : QStringLiteral("Update Tools")))
          : (tr ? QStringLiteral("Oyun Araçlarını Kur")
                : (es ? QStringLiteral("Instalar herramientas")
                      : QStringLiteral("Install Tools")));
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
  bundle.telemetryTitle =
      tr ? QStringLiteral("Gizlilik ve Telemetri")
         : (es ? QStringLiteral("Privacidad y telemetría")
               : QStringLiteral("Privacy and telemetry"));
  bundle.telemetryIntro =
      tr ? QStringLiteral("Ro-ASD'nin temel amacı sorunları teşhis edip çözmektir. Bu sürüm veri göndermez; tercihiniz gelecekteki telemetri modülü için kaydedilir.")
         : (es ? QStringLiteral("El propósito principal de Ro-ASD es diagnosticar y resolver problemas. Esta versión no envía datos; guarda tu preferencia para un futuro módulo de telemetría.")
               : QStringLiteral("Ro-ASD's primary purpose is to diagnose and fix problems. This version does not send data; it records your preference for a future telemetry module."));
  bundle.telemetryPurposeTitle =
      tr ? QStringLiteral("Verileri neden isteriz?")
         : (es ? QStringLiteral("¿Por qué pediríamos estos datos?")
               : QStringLiteral("Why would we ask for data?"));
  bundle.telemetryPurposeText =
      tr ? QStringLiteral("Hangi sürümlerde hata yaşandığını, hangi masaüstü ve donanım sınıflarında desteğe ihtiyaç olduğunu ve kaç sistemin Ro-ASD kullandığını anlamak için.")
         : (es ? QStringLiteral("Para entender qué versiones tienen errores, qué clases de escritorio y hardware necesitan soporte y cuántos sistemas usan Ro-ASD.")
               : QStringLiteral("To understand which versions hit errors, which desktop and hardware classes need support, and how many systems use Ro-ASD."));
  bundle.telemetryLevelsTitle =
      tr ? QStringLiteral("Seviyeler")
         : (es ? QStringLiteral("Niveles") : QStringLiteral("Levels"));
  bundle.telemetryLevelsText =
      tr ? QStringLiteral("Kapalı: hiçbir şey gönderilmez.\nSayım: yalnızca anonim kullanım/kurulum sayımı.\nTemel: sürüm, mimari, masaüstü oturumu, dil ve hata kategorisi.\nGenişletilmiş: temel verilere ek olarak yuvarlanmış donanım ve ekran sınıfı.")
         : (es ? QStringLiteral("Desactivado: no se envía nada.\nConteo: solo conteo anónimo de uso/instalación.\nBásico: versión, arquitectura, sesión de escritorio, idioma y categoría de error.\nAmpliado: añade clase aproximada de hardware y pantalla.")
               : QStringLiteral("Off: nothing is sent.\nCount: anonymous usage/install count only.\nBasic: version, architecture, desktop session, language and error category.\nExtended: basic data plus rounded hardware and display class."));
  bundle.telemetryDoesNotCollectTitle =
      tr ? QStringLiteral("Toplamayız")
         : (es ? QStringLiteral("No recopilamos")
               : QStringLiteral("We do not collect"));
  bundle.telemetryDoesNotCollectText =
      tr ? QStringLiteral("Kişisel dosyalar, dosya adları, URL'ler, komut geçmişi, kullanıcı adı, e-posta, konum, makine kimliği veya benzersiz cihaz kimliği.")
         : (es ? QStringLiteral("Archivos personales, nombres de archivo, URL, historial de comandos, nombre de usuario, correo, ubicación, ID de máquina o ID único de dispositivo.")
               : QStringLiteral("Personal files, file names, URLs, command history, user name, email, location, machine ID or unique device ID."));
  bundle.telemetrySliderTitle =
      tr ? QStringLiteral("Paylaşım seviyesi")
         : (es ? QStringLiteral("Nivel de uso compartido")
               : QStringLiteral("Sharing level"));
  bundle.telemetryCurrentLevel =
      tr ? QStringLiteral("Seçili seviye: %1")
         : (es ? QStringLiteral("Nivel seleccionado: %1")
               : QStringLiteral("Selected level: %1"));
  bundle.telemetryLevelOff =
      tr ? QStringLiteral("Kapalı")
         : (es ? QStringLiteral("Desactivado") : QStringLiteral("Off"));
  bundle.telemetryLevelCount =
      tr ? QStringLiteral("Sayım")
         : (es ? QStringLiteral("Conteo") : QStringLiteral("Count"));
  bundle.telemetryLevelBasic =
      tr ? QStringLiteral("Temel")
         : (es ? QStringLiteral("Básico") : QStringLiteral("Basic"));
  bundle.telemetryLevelExtended =
      tr ? QStringLiteral("Genişletilmiş")
         : (es ? QStringLiteral("Ampliado") : QStringLiteral("Extended"));
  bundle.telemetryDescriptionOff =
      tr ? QStringLiteral("Hiçbir telemetri veya sayım gönderilmez.")
         : (es ? QStringLiteral("No se envía telemetría ni conteo.")
               : QStringLiteral("No telemetry or count signal is sent."));
  bundle.telemetryDescriptionCount =
      tr ? QStringLiteral("Varsayılan: ileride yalnızca anonim kurulum/kullanım sayımı yapılır; kişiyi veya cihazı tanımlayan bilgi tutulmaz.")
         : (es ? QStringLiteral("Predeterminado: en el futuro solo se contará uso/instalación de forma anónima; no se guarda nada que identifique a la persona o el dispositivo.")
               : QStringLiteral("Default: in the future only anonymous install/usage counts are recorded; nothing identifying a person or device is kept."));
  bundle.telemetryDescriptionBasic =
      tr ? QStringLiteral("Sorunları teşhis etmek için sürüm ve ortam bilgileri paylaşılır.")
         : (es ? QStringLiteral("Comparte versión y entorno para diagnosticar problemas.")
               : QStringLiteral("Shares version and environment details to diagnose problems."));
  bundle.telemetryDescriptionExtended =
      tr ? QStringLiteral("Daha ayrıntılı teşhis sinyali sağlar; kullanıcı içeriği ve benzersiz kimlikler yine toplanmaz.")
         : (es ? QStringLiteral("Aporta señales de diagnóstico más detalladas; no se recopila contenido del usuario ni identificadores únicos.")
               : QStringLiteral("Provides more detailed diagnostic signals; user content and unique identifiers are still not collected."));

  if (de) {
    bundle.languageButton = QStringLiteral("🇩🇪 Deutsch");
    bundle.themeToggle = darkTheme ? QStringLiteral("🌙 Dunkel") : QStringLiteral("☀️ Hell");
    bundle.networkStatus = QStringLiteral("⚠ Keine Internetverbindung");
    bundle.backButton = QStringLiteral("⬅ Zurück");
    bundle.toggleLogs = updateLogsVisible ? QStringLiteral("Ausblenden ⬆") : QStringLiteral("Protokolle anzeigen ⬇");
    bundle.toggleLibraryLogs = libraryLogsVisible ? QStringLiteral("Ausblenden ⬆") : QStringLiteral("Protokolle anzeigen ⬇");
    bundle.updateButton = QStringLiteral("System aktualisieren");
    bundle.welcomeNext = QStringLiteral("Weiter");
    bundle.welcomeFinish = QStringLiteral("ro-Assist öffnen");
    bundle.welcomeProgress = QStringLiteral("Schritt %1 von %2");
    bundle.dashboardGreeting = QStringLiteral("Willkommen bei Ro-ASD");
    bundle.dashboardDescription = QStringLiteral("Wählen Sie einen Wartungsschritt, um das System vorbereitet zu halten.");
    bundle.dashboardUpdateCard = QStringLiteral("Systemaktualisierung\nPakete und Anwendungen aktualisieren");
    bundle.dashboardSocialCard = QStringLiteral("Links und Unterstützung\nWebsite und Quellcode");
    bundle.dashboardStoreCard = QStringLiteral("Anwendungsspeicher\nEmpfohlene Anwendungen entdecken");
    bundle.dashboardCommunityCard = QStringLiteral("Gemeinschaft\nDie Ro-ASD-Gemeinschaft entdecken");
    bundle.dashboardLibraryCard = QStringLiteral("Grundlegende Spielwerkzeuge\nGameMode, MangoHud und Vulkan-Werkzeuge");
    bundle.dashboardPrinterCard = QStringLiteral("Drucker und Scanner\nTreiber- und Scanunterstützung einrichten");
    bundle.dashboardTelemetryCard = QStringLiteral("Datenschutz und Telemetrie\nDatenfreigabe verwalten");
    bundle.slide1Title = QStringLiteral("System kontrolliert aktualisieren");
    bundle.slide1Description = QStringLiteral("Zeigt Aktualisierungsrisiken und verfolgt DNF, Flatpak und Snap als getrennte Schritte.");
    bundle.updateSlideButton = QStringLiteral("🚀 Zur Aktualisierung");
    bundle.slide2Title = QStringLiteral("Links und Unterstützung");
    bundle.slide2Description = QStringLiteral("Hilfe, Dokumentation und Quellcode an einem Ort.");
    bundle.slide3Title = QStringLiteral("Empfohlene Anwendungen entdecken");
    bundle.slide3Description = QStringLiteral("Durchsuchen Sie empfohlene Kategorien für Ro-ASD; die Installation kann über Discover oder Distributionsquellen erfolgen.");
    bundle.appStoreSlideButton = QStringLiteral("Zum Store");
    bundle.appStoreTitle = QStringLiteral("Empfohlene Anwendungen");
    bundle.appStoreOpenButton = QStringLiteral("Discover öffnen");
    bundle.slide4Title = QStringLiteral("Wer sind wir? Entdecken Sie unsere Gemeinschaft");
    bundle.slide4Description = QStringLiteral("Als Open-Source-Softwareentwicklungsclub der Yozgat-Bozok-Universität arbeiten wir am Ro-ASD-Projekt.");
    bundle.communityButton = QStringLiteral("Dem Club beitreten");
    bundle.slide5Title = QStringLiteral("Grundlegende Spielwerkzeuge");
    bundle.slide5Description = QStringLiteral("GameMode, MangoHud und Vulkan-Werkzeuge installieren oder aktualisieren. Treiber bleiben Aufgabe von ro Control.");
    bundle.librarySlideButton = QStringLiteral("Spielwerkzeuge öffnen");
    bundle.slide6Title = QStringLiteral("Datenschutz und Telemetrie");
    bundle.slide6Description = QStringLiteral("Wählen Sie die Datenfreigabe, die Ro-ASD bei der Fehlerdiagnose hilft.");
    bundle.libraryStatusIdle = QStringLiteral("GameMode, MangoHud und Vulkan-Werkzeuge können vorbereitet werden.");
    bundle.logPlaceholder = QStringLiteral("Protokolle ...");
    bundle.websiteButton = QStringLiteral("Website");
    bundle.libraryActionButton = libraryInstalled ? QStringLiteral("Werkzeuge aktualisieren") : QStringLiteral("Werkzeuge installieren");
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
    bundle.telemetryTitle = QStringLiteral("Datenschutz und Telemetrie");
    bundle.telemetryIntro = QStringLiteral("Ro-ASD soll vor allem Probleme diagnostizieren und beheben. Diese Version sendet keine Daten; sie speichert nur Ihre Auswahl für ein künftiges Telemetriemodul.");
    bundle.telemetryPurposeTitle = QStringLiteral("Warum würden wir Daten anfragen?");
    bundle.telemetryPurposeText = QStringLiteral("Um zu verstehen, welche Versionen Fehler haben, welche Desktop- und Hardwareklassen Unterstützung brauchen und wie viele Systeme Ro-ASD nutzen.");
    bundle.telemetryLevelsTitle = QStringLiteral("Stufen");
    bundle.telemetryLevelsText = QStringLiteral("Aus: nichts wird gesendet.\nZählung: nur anonyme Nutzungs-/Installationszählung.\nBasis: Version, Architektur, Desktop-Sitzung, Sprache und Fehlerkategorie.\nErweitert: Basisdaten plus gerundete Hardware- und Bildschirmklasse.");
    bundle.telemetryDoesNotCollectTitle = QStringLiteral("Wir sammeln nicht");
    bundle.telemetryDoesNotCollectText = QStringLiteral("Persönliche Dateien, Dateinamen, URLs, Befehlsverlauf, Benutzername, E-Mail, Standort, Maschinen-ID oder eindeutige Geräte-ID.");
    bundle.telemetrySliderTitle = QStringLiteral("Freigabestufe");
    bundle.telemetryCurrentLevel = QStringLiteral("Ausgewählte Stufe: %1");
    bundle.telemetryLevelOff = QStringLiteral("Aus");
    bundle.telemetryLevelCount = QStringLiteral("Zählung");
    bundle.telemetryLevelBasic = QStringLiteral("Basis");
    bundle.telemetryLevelExtended = QStringLiteral("Erweitert");
    bundle.telemetryDescriptionOff = QStringLiteral("Es wird keine Telemetrie und keine Zählung gesendet.");
    bundle.telemetryDescriptionCount = QStringLiteral("Standard: künftig wird nur anonym gezählt; es wird nichts gespeichert, das Person oder Gerät identifiziert.");
    bundle.telemetryDescriptionBasic = QStringLiteral("Teilt Versions- und Umgebungsdaten zur Fehlerdiagnose.");
    bundle.telemetryDescriptionExtended = QStringLiteral("Liefert detailliertere Diagnosesignale; Nutzerinhalte und eindeutige IDs werden weiterhin nicht gesammelt.");
  } else if (fr) {
    bundle.languageButton = QStringLiteral("🇫🇷 Français");
    bundle.themeToggle = darkTheme ? QStringLiteral("🌙 Sombre") : QStringLiteral("☀️ Clair");
    bundle.networkStatus = QStringLiteral("⚠ Pas de connexion Internet");
    bundle.backButton = QStringLiteral("⬅ Retour");
    bundle.toggleLogs = updateLogsVisible ? QStringLiteral("Masquer ⬆") : QStringLiteral("Afficher les journaux ⬇");
    bundle.toggleLibraryLogs = libraryLogsVisible ? QStringLiteral("Masquer ⬆") : QStringLiteral("Afficher les journaux ⬇");
    bundle.updateButton = QStringLiteral("Mettre le système à jour");
    bundle.welcomeNext = QStringLiteral("Suivant");
    bundle.welcomeFinish = QStringLiteral("Ouvrir ro-Assist");
    bundle.welcomeProgress = QStringLiteral("Étape %1 sur %2");
    bundle.dashboardGreeting = QStringLiteral("Bienvenue dans Ro-ASD");
    bundle.dashboardDescription = QStringLiteral("Choisissez une étape de maintenance pour garder le système prêt.");
    bundle.dashboardUpdateCard = QStringLiteral("Mise à jour du système\nMettre à jour les paquets et les applications");
    bundle.dashboardSocialCard = QStringLiteral("Liens et assistance\nSite web et code source");
    bundle.dashboardStoreCard = QStringLiteral("Magasin d'applications\nDécouvrir les applications recommandées");
    bundle.dashboardCommunityCard = QStringLiteral("Communauté\nDécouvrir la communauté Ro-ASD");
    bundle.dashboardLibraryCard = QStringLiteral("Outils de jeu de base\nGameMode, MangoHud et outils Vulkan");
    bundle.dashboardPrinterCard = QStringLiteral("Imprimante et scanner\nPréparer les pilotes et la numérisation");
    bundle.dashboardTelemetryCard = QStringLiteral("Confidentialité et télémétrie\nGérer le partage des données");
    bundle.slide1Title = QStringLiteral("Mettre le système à jour avec contrôle");
    bundle.slide1Description = QStringLiteral("Affiche les risques et suit DNF, Flatpak et Snap comme étapes séparées.");
    bundle.updateSlideButton = QStringLiteral("🚀 Ouvrir la mise à jour");
    bundle.slide2Title = QStringLiteral("Liens et assistance");
    bundle.slide2Description = QStringLiteral("Accédez à l'aide, la documentation et au code source depuis un seul endroit.");
    bundle.slide3Title = QStringLiteral("Découvrir les applications recommandées");
    bundle.slide3Description = QStringLiteral("Parcourez les catégories recommandées pour Ro-ASD ; l'installation peut passer par Discover ou les sources de la distribution.");
    bundle.appStoreSlideButton = QStringLiteral("Accéder au magasin");
    bundle.appStoreTitle = QStringLiteral("Applications recommandées");
    bundle.appStoreOpenButton = QStringLiteral("Ouvrir Discover");
    bundle.slide4Title = QStringLiteral("Qui sommes-nous ? Découvrez notre communauté");
    bundle.slide4Description = QStringLiteral("Nous travaillons sur le projet Ro-ASD au sein du club de développement de logiciels libres de l'université Yozgat Bozok.");
    bundle.communityButton = QStringLiteral("Rejoindre le club");
    bundle.slide5Title = QStringLiteral("Outils de jeu de base");
    bundle.slide5Description = QStringLiteral("Installez ou mettez à jour GameMode, MangoHud et les outils Vulkan. Les pilotes restent dans ro Control.");
    bundle.librarySlideButton = QStringLiteral("Ouvrir les outils de jeu");
    bundle.slide6Title = QStringLiteral("Confidentialité et télémétrie");
    bundle.slide6Description = QStringLiteral("Choisissez le niveau de partage qui aidera Ro-ASD à diagnostiquer les problèmes.");
    bundle.libraryStatusIdle = QStringLiteral("GameMode, MangoHud et les outils Vulkan peuvent être préparés.");
    bundle.logPlaceholder = QStringLiteral("Journaux ...");
    bundle.websiteButton = QStringLiteral("Site web");
    bundle.libraryActionButton = libraryInstalled ? QStringLiteral("Mettre à jour les outils") : QStringLiteral("Installer les outils");
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
    bundle.telemetryTitle = QStringLiteral("Confidentialité et télémétrie");
    bundle.telemetryIntro = QStringLiteral("Ro-ASD sert d'abord à diagnostiquer et corriger les problèmes. Cette version n'envoie pas de données ; elle enregistre seulement votre choix pour un futur module de télémétrie.");
    bundle.telemetryPurposeTitle = QStringLiteral("Pourquoi demander ces données ?");
    bundle.telemetryPurposeText = QStringLiteral("Pour comprendre quelles versions rencontrent des erreurs, quelles classes de bureau et de matériel ont besoin d'aide, et combien de systèmes utilisent Ro-ASD.");
    bundle.telemetryLevelsTitle = QStringLiteral("Niveaux");
    bundle.telemetryLevelsText = QStringLiteral("Désactivé : rien n'est envoyé.\nComptage : seulement un comptage anonyme d'usage/installation.\nBasique : version, architecture, session de bureau, langue et catégorie d'erreur.\nÉtendu : données basiques plus classe matérielle et affichage arrondis.");
    bundle.telemetryDoesNotCollectTitle = QStringLiteral("Nous ne collectons pas");
    bundle.telemetryDoesNotCollectText = QStringLiteral("Fichiers personnels, noms de fichier, URL, historique de commandes, nom d'utilisateur, e-mail, localisation, ID machine ou ID unique d'appareil.");
    bundle.telemetrySliderTitle = QStringLiteral("Niveau de partage");
    bundle.telemetryCurrentLevel = QStringLiteral("Niveau sélectionné : %1");
    bundle.telemetryLevelOff = QStringLiteral("Désactivé");
    bundle.telemetryLevelCount = QStringLiteral("Comptage");
    bundle.telemetryLevelBasic = QStringLiteral("Basique");
    bundle.telemetryLevelExtended = QStringLiteral("Étendu");
    bundle.telemetryDescriptionOff = QStringLiteral("Aucune télémétrie ni signal de comptage n'est envoyé.");
    bundle.telemetryDescriptionCount = QStringLiteral("Par défaut : seul un comptage anonyme sera fait à l'avenir ; rien n'identifie une personne ou un appareil.");
    bundle.telemetryDescriptionBasic = QStringLiteral("Partage la version et l'environnement pour diagnostiquer les problèmes.");
    bundle.telemetryDescriptionExtended = QStringLiteral("Fournit des signaux de diagnostic plus détaillés ; le contenu utilisateur et les identifiants uniques restent exclus.");
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

QString versionText(const QString &languageCode, const QString &version) {
  if (isGerman(languageCode))
    return QStringLiteral("Aktuelle Version: %1").arg(version);
  if (isFrench(languageCode))
    return QStringLiteral("Version actuelle : %1").arg(version);
  return isTurkish(languageCode)
             ? QStringLiteral("Mevcut Sürüm: %1").arg(version)
             : (isSpanish(languageCode)
                    ? QStringLiteral("Versión Actual: %1").arg(version)
                    : QStringLiteral("Current Version: %1").arg(version));
}

QString storeVersionTitle(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Discover nicht gefunden", "Discover introuvable")
  return isTurkish(languageCode)
             ? QStringLiteral("Discover Bulunamadı")
             : (isSpanish(languageCode) ? QStringLiteral("Discover no encontrado")
                                        : QStringLiteral("Discover Not Found"));
}

QString storeVersionMessage(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Discover konnte nicht gestartet werden. Installieren oder öffnen Sie Anwendungen über die Paketverwaltung Ihrer Distribution.", "Discover n'a pas pu démarrer. Installez ou ouvrez des applications avec le gestionnaire de paquets de votre distribution.")
  return isTurkish(languageCode)
             ? QStringLiteral("Discover başlatılamadı. Uygulamaları dağıtımınızın paket yöneticisi üzerinden kurabilir veya açabilirsiniz.")
             : (isSpanish(languageCode) ? QStringLiteral("No se pudo iniciar Discover. Instale o abra aplicaciones con el gestor de paquetes de su distribución.")
                                        : QStringLiteral("Discover could not be started. Install or open applications through your distribution package manager."));
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
  RETURN_ADDITIONAL_TRANSLATION("Spielwerkzeuge werden installiert ...", "Installation des outils de jeu ...")
  return isTurkish(languageCode)
             ? QStringLiteral("Oyun araçları kuruluyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Instalando herramientas de juego...")
                                        : QStringLiteral("Installing gaming tools..."));
}

QString updateStarting(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Systemaktualisierung wird gestartet ...", "Démarrage de la mise à jour du système ...")
  return isTurkish(languageCode)
             ? QStringLiteral("Sistem güncellemesi başlatılıyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Comenzando actualización del sistema...")
                                        : QStringLiteral("System update starting..."));
}

QString librariesInstalled(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Spielwerkzeuge wurden installiert.", "Outils de jeu installés.")
  return isTurkish(languageCode)
             ? QStringLiteral("Oyun araçları kuruldu.")
             : (isSpanish(languageCode) ? QStringLiteral("Herramientas de juego instaladas.")
                                        : QStringLiteral("Gaming tools installed."));
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

QString maintenanceReady(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Wartungsprüfung bereit.", "Contrôle de maintenance prêt.")
  return isTurkish(languageCode)
             ? QStringLiteral("Bakım kontrolü hazır.")
             : (isSpanish(languageCode) ? QStringLiteral("Control de mantenimiento listo.")
                                        : QStringLiteral("Maintenance check ready."));
}

QString maintenancePlanSummary(const QString &languageCode,
                               bool flatpakAvailable, bool snapAvailable) {
  const QString flatpak =
      flatpakAvailable
          ? (isTurkish(languageCode) ? QStringLiteral("Flatpak açık")
                                     : (isSpanish(languageCode) ? QStringLiteral("Flatpak disponible")
                                                                : QStringLiteral("Flatpak available")))
          : (isTurkish(languageCode) ? QStringLiteral("Flatpak yok")
                                     : (isSpanish(languageCode) ? QStringLiteral("Flatpak no disponible")
                                                                : QStringLiteral("Flatpak not available")));
  const QString snap =
      snapAvailable
          ? (isTurkish(languageCode) ? QStringLiteral("Snap açık")
                                     : (isSpanish(languageCode) ? QStringLiteral("Snap disponible")
                                                                : QStringLiteral("Snap available")))
          : (isTurkish(languageCode) ? QStringLiteral("Snap yok")
                                     : (isSpanish(languageCode) ? QStringLiteral("Snap no disponible")
                                                                : QStringLiteral("Snap not available")));
  if (isGerman(languageCode))
    return QStringLiteral("Geplanter Ablauf: DNF-Systempakete, danach verfügbare App-Quellen. %1, %2.").arg(flatpak, snap);
  if (isFrench(languageCode))
    return QStringLiteral("Plan prévu : paquets système DNF, puis sources d'applications disponibles. %1, %2.").arg(flatpak, snap);
  return isTurkish(languageCode)
             ? QStringLiteral("Planlanan akış: DNF sistem paketleri, ardından mevcut uygulama kaynakları. %1, %2.").arg(flatpak, snap)
             : (isSpanish(languageCode)
                    ? QStringLiteral("Flujo previsto: paquetes DNF del sistema y luego fuentes de aplicaciones disponibles. %1, %2.").arg(flatpak, snap)
                    : QStringLiteral("Planned flow: DNF system packages, then available application sources. %1, %2.").arg(flatpak, snap));
}

QString maintenanceNoKnownRisk(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Keine bekannte Grafik- oder Neustartwarnung erkannt.", "Aucun avertissement graphique ou redémarrage connu détecté.")
  return isTurkish(languageCode)
             ? QStringLiteral("Bilinen grafik veya yeniden başlatma uyarısı algılanmadı.")
             : (isSpanish(languageCode) ? QStringLiteral("No se detectó advertencia gráfica o de reinicio conocida.")
                                        : QStringLiteral("No known graphics or reboot warning detected."));
}

QString nouveauRiskWarning(const QString &languageCode,
                           bool roControlAvailable) {
  if (isGerman(languageCode))
    return roControlAvailable
               ? QStringLiteral("NVIDIA-GPU mit nouveau erkannt. Kernel- oder Grafikaktualisierungen können nach dem Neustart ein schwarzes Bild auslösen. Prüfen Sie die Hardwaredetails in ro Control.")
               : QStringLiteral("NVIDIA-GPU mit nouveau erkannt. Kernel- oder Grafikaktualisierungen können nach dem Neustart ein schwarzes Bild auslösen. ro Control ist nicht installiert.");
  if (isFrench(languageCode))
    return roControlAvailable
               ? QStringLiteral("GPU NVIDIA avec nouveau détecté. Les mises à jour du noyau ou de la pile graphique peuvent provoquer un écran noir après redémarrage. Vérifiez les détails dans ro Control.")
               : QStringLiteral("GPU NVIDIA avec nouveau détecté. Les mises à jour du noyau ou de la pile graphique peuvent provoquer un écran noir après redémarrage. ro Control n'est pas installé.");
  if (isTurkish(languageCode)) {
    return roControlAvailable
               ? QStringLiteral("NVIDIA GPU ve nouveau algılandı. Kernel veya grafik yığını güncellemeleri yeniden başlatma sonrası siyah ekran riski taşıyabilir. Donanım ayrıntılarını ro Control'de kontrol edin.")
               : QStringLiteral("NVIDIA GPU ve nouveau algılandı. Kernel veya grafik yığını güncellemeleri yeniden başlatma sonrası siyah ekran riski taşıyabilir. ro Control kurulu değil.");
  }
  if (isSpanish(languageCode)) {
    return roControlAvailable
               ? QStringLiteral("Se detectó GPU NVIDIA con nouveau. Las actualizaciones del kernel o la pila gráfica pueden causar pantalla negra tras reiniciar. Revise los detalles en ro Control.")
               : QStringLiteral("Se detectó GPU NVIDIA con nouveau. Las actualizaciones del kernel o la pila gráfica pueden causar pantalla negra tras reiniciar. ro Control no está instalado.");
  }
  return roControlAvailable
             ? QStringLiteral("NVIDIA GPU with nouveau detected. Kernel or graphics stack updates can cause a black screen after reboot. Check hardware details in ro Control.")
             : QStringLiteral("NVIDIA GPU with nouveau detected. Kernel or graphics stack updates can cause a black screen after reboot. ro Control is not installed.");
}

QString lowDiskWarning(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Wenig freier Speicherplatz. Aktualisierungen können fehlschlagen.", "Espace disque faible. Les mises à jour peuvent échouer.")
  return isTurkish(languageCode)
             ? QStringLiteral("Diskte az boş alan var. Güncelleme işlemi başarısız olabilir.")
             : (isSpanish(languageCode) ? QStringLiteral("Poco espacio libre en disco. La actualización puede fallar.")
                                        : QStringLiteral("Low free disk space. Updates may fail."));
}

QString rebootRequiredWarning(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Ein Neustart steht bereits aus.", "Un redémarrage est déjà en attente.")
  return isTurkish(languageCode)
             ? QStringLiteral("Sistemde zaten bekleyen bir yeniden başlatma var.")
             : (isSpanish(languageCode) ? QStringLiteral("Ya hay un reinicio pendiente.")
                                        : QStringLiteral("A reboot is already pending."));
}

QString roControlButtonText(const QString &languageCode,
                            bool roControlAvailable) {
  if (!roControlAvailable)
    return roControlMissingMessage(languageCode);
  RETURN_ADDITIONAL_TRANSLATION("ro Control öffnen", "Ouvrir ro Control")
  return isTurkish(languageCode)
             ? QStringLiteral("ro Control'ü Aç")
             : (isSpanish(languageCode) ? QStringLiteral("Abrir ro Control")
                                        : QStringLiteral("Open ro Control"));
}

QString roControlMissingMessage(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("ro Control ist nicht installiert", "ro Control n'est pas installé")
  return isTurkish(languageCode)
             ? QStringLiteral("ro Control kurulu değil")
             : (isSpanish(languageCode) ? QStringLiteral("ro Control no está instalado")
                                        : QStringLiteral("ro Control is not installed"));
}

QString updateStepStarting(const QString &languageCode, const QString &label,
                           int current, int total) {
  if (isGerman(languageCode))
    return QStringLiteral("Aktualisierungsschritt %1/%2: %3").arg(current).arg(total).arg(label);
  if (isFrench(languageCode))
    return QStringLiteral("Étape de mise à jour %1/%2 : %3").arg(current).arg(total).arg(label);
  return isTurkish(languageCode)
             ? QStringLiteral("Güncelleme adımı %1/%2: %3").arg(current).arg(total).arg(label)
             : (isSpanish(languageCode)
                    ? QStringLiteral("Paso de actualización %1/%2: %3").arg(current).arg(total).arg(label)
                    : QStringLiteral("Update step %1/%2: %3").arg(current).arg(total).arg(label));
}

QString updatePartialFailure(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Einige Aktualisierungsschritte sind fehlgeschlagen. Prüfen Sie die Protokolle.", "Certaines étapes de mise à jour ont échoué. Vérifiez les journaux.")
  return isTurkish(languageCode)
             ? QStringLiteral("Bazı güncelleme adımları başarısız oldu. Logları kontrol edin.")
             : (isSpanish(languageCode) ? QStringLiteral("Algunos pasos de actualización fallaron. Revise los registros.")
                                        : QStringLiteral("Some update steps failed. Check the logs."));
}

QString updateNeedsReboot(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Aktualisierung abgeschlossen. Starten Sie neu, damit Kernel- oder Grafikänderungen aktiv werden.", "Mise à jour terminée. Redémarrez pour appliquer les changements du noyau ou de la pile graphique.")
  return isTurkish(languageCode)
             ? QStringLiteral("Güncelleme tamamlandı. Kernel veya grafik değişikliklerinin etkinleşmesi için yeniden başlatın.")
             : (isSpanish(languageCode) ? QStringLiteral("Actualización completada. Reinicie para aplicar cambios del kernel o gráficos.")
                                        : QStringLiteral("Update completed. Reboot to apply kernel or graphics changes."));
}

QString updateAborted(const QString &languageCode) {
  RETURN_ADDITIONAL_TRANSLATION("Aktualisierung abgebrochen.", "Mise à jour annulée.")
  return isTurkish(languageCode)
             ? QStringLiteral("Güncelleme iptal edildi.")
             : (isSpanish(languageCode) ? QStringLiteral("Actualización cancelada.")
                                        : QStringLiteral("Update cancelled."));
}

#undef RETURN_ADDITIONAL_TRANSLATION

} // namespace RoAssist::UiTexts
