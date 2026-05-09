#include "roassist/ui_texts.h"

namespace RoAssist::UiTexts {

namespace {

bool isTurkish(const QString &code) { return code == "tr"; }
bool isSpanish(const QString &code) { return code == "es"; }

} // namespace

Bundle buildBundle(const QString &languageCode, bool darkTheme,
                   bool updateLogsVisible, bool libraryLogsVisible,
                   bool libraryInstalled) {
  const bool tr = isTurkish(languageCode);
  const bool es = isSpanish(languageCode);

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
  bundle.versionLabel = tr ? QStringLiteral("Mevcut Sürüm: 0.1.0")
                           : (es ? QStringLiteral("Versión Actual: 0.1.0")
                                 : QStringLiteral("Current Version: 0.1.0"));
  bundle.updateButton = tr ? QStringLiteral("Sistemi Güncelle")
                           : (es ? QStringLiteral("Actualizar Sistema")
                                 : QStringLiteral("Update System"));
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
  return bundle;
}

QString aboutTitle(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Hakkında")
             : (isSpanish(languageCode) ? QStringLiteral("Acerca de")
                                        : QStringLiteral("About"));
}

QString aboutDescription(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Yozgat Bozok Üniversitesi Açık Kaynak Yazılım Geliştirme Kulübü ro-ASD projesi sistem yönetim aracı.")
             : (isSpanish(languageCode)
                    ? QStringLiteral("Herramienta de gestión del sistema del proyecto ro-ASD del Club de Desarrollo de Software de Código Abierto de la Universidad Yozgat Bozok.")
                    : QStringLiteral("Yozgat Bozok University Open Source Software Development Club ro-ASD project system management tool."));
}

QString developerLabel(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Geliştirici")
             : (isSpanish(languageCode) ? QStringLiteral("Desarrollador")
                                        : QStringLiteral("Developer"));
}

QString yearLabel(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Yıl")
             : (isSpanish(languageCode) ? QStringLiteral("Año")
                                        : QStringLiteral("Year"));
}

QString closeLabel(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Kapat")
             : (isSpanish(languageCode) ? QStringLiteral("Cerrar")
                                        : QStringLiteral("Close"));
}

QString storeVersionTitle(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Mağaza Sürümü")
             : (isSpanish(languageCode) ? QStringLiteral("Versión de la Tienda")
                                        : QStringLiteral("Store Version"));
}

QString storeVersionMessage(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Bu sistem yakında eklenecektir!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Integración próximamente!")
                                        : QStringLiteral("Integration coming soon!"));
}

QString errorTitle(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Hata")
             : (isSpanish(languageCode) ? QStringLiteral("Error")
                                        : QStringLiteral("Error"));
}

QString infoTitle(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Bilgi")
             : (isSpanish(languageCode) ? QStringLiteral("Información")
                                        : QStringLiteral("Info"));
}

QString criticalErrorTitle(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Kritik Hata")
             : (isSpanish(languageCode) ? QStringLiteral("Error crítico")
                                        : QStringLiteral("Critical Error"));
}

QString noInternetMessage(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("İnternet bağlantısı yok!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Sin conexión a internet!")
                                        : QStringLiteral("No internet connection!"));
}

QString operationRunningMessage(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Başka bir işlem zaten çalışıyor.")
             : (isSpanish(languageCode) ? QStringLiteral("Ya hay otro proceso en ejecución.")
                                        : QStringLiteral("Another operation is already running."));
}

QString librariesInstalling(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Kütüphaneler kuruluyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Instalando bibliotecas...")
                                        : QStringLiteral("Installing libraries..."));
}

QString updateStarting(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Sistem güncellemesi başlatılıyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Comenzando actualización del sistema...")
                                        : QStringLiteral("System update starting..."));
}

QString librariesInstalled(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Kütüphaneler kuruldu.")
             : (isSpanish(languageCode) ? QStringLiteral("Bibliotecas instaladas.")
                                        : QStringLiteral("Libraries installed."));
}

QString allOperationsCompleted(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Tüm işlemler başarıyla tamamlandı.")
             : (isSpanish(languageCode)
                    ? QStringLiteral("Todas las operaciones se completaron con éxito.")
                    : QStringLiteral("All operations completed successfully."));
}

QString processFailed(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("İşlem Başarısız!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Proceso fallido!")
                                        : QStringLiteral("Process Failed!"));
}

QString processFailedDetails(const QString &languageCode, bool libraryOperation) {
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
  return isTurkish(languageCode)
             ? QStringLiteral("⚠️ Sistem şu an başka bir işlemle meşgul...")
             : (isSpanish(languageCode) ? QStringLiteral("⚠️ El sistema está ocupado...")
                                        : QStringLiteral("⚠️ System is busy..."));
}

QString networkError(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("🌐 İnternet Hatası")
             : (isSpanish(languageCode) ? QStringLiteral("🌐 Error de red")
                                        : QStringLiteral("🌐 Network Error"));
}

QString wrongPasswordShort(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("❌ Şifre Yanlış!")
             : (isSpanish(languageCode) ? QStringLiteral("❌ ¡Contraseña incorrecta!")
                                        : QStringLiteral("❌ Wrong Password!"));
}

QString wrongPasswordDetail(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Yanlış şifre girdiniz veya sudo yetkiniz yok!")
             : (isSpanish(languageCode) ? QStringLiteral("¡Contraseña incorrecta o sin permisos de sudo!")
                                        : QStringLiteral("Wrong password or no sudo permission!"));
}

QString checkingLibraries(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Kontrol ediliyor...")
             : (isSpanish(languageCode) ? QStringLiteral("Comprobando...")
                                        : QStringLiteral("Checking..."));
}

QString updateCheckFailed(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Güncelleme denetimi başarısız oldu.")
             : (isSpanish(languageCode) ? QStringLiteral("La comprobación de actualizaciones falló.")
                                        : QStringLiteral("Update check failed."));
}

QString updateAvailable(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Sistem Güncellemesi Mevcut")
             : (isSpanish(languageCode) ? QStringLiteral("Actualización del Sistema Disponible")
                                        : QStringLiteral("System Update Available"));
}

QString systemUpToDate(const QString &languageCode) {
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
                                     : QStringLiteral("Downloading..."));
  return QStringLiteral("%1 %2%").arg(prefix).arg(percent);
}

QString installingPackages(const QString &languageCode, int current, int total) {
  const QString prefix =
      isTurkish(languageCode)
          ? QStringLiteral("Paketler kuruluyor...")
          : (isSpanish(languageCode) ? QStringLiteral("Instalando paquetes...")
                                     : QStringLiteral("Installing packages..."));
  return QStringLiteral("%1 (%2/%3)").arg(prefix).arg(current).arg(total);
}

QString componentFailedToStart(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Bileşen başlatılamadı.")
             : (isSpanish(languageCode) ? QStringLiteral("El componente no pudo iniciarse.")
                                        : QStringLiteral("Component failed to start."));
}

QString componentCrashed(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Bileşen çöktü.")
             : (isSpanish(languageCode) ? QStringLiteral("El componente falló.")
                                        : QStringLiteral("Component crashed."));
}

QString criticalErrorPrefix(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("Kritik Hata: ")
             : (isSpanish(languageCode) ? QStringLiteral("Error crítico: ")
                                        : QStringLiteral("Critical Error: "));
}

QString processCompleted(const QString &languageCode) {
  return isTurkish(languageCode)
             ? QStringLiteral("✅ İşlem Başarıyla Tamamlandı!")
             : (isSpanish(languageCode) ? QStringLiteral("✅ ¡Completado con Éxito!")
                                        : QStringLiteral("✅ Process Completed!"));
}

} // namespace RoAssist::UiTexts
