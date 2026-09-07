# ro-Assist

**Ro-ASD için ilk kurulum, bakım ve güvenli güncelleme merkezi.**

ro-Assist, Fedora 44 KDE Plasma tabanlı Ro-ASD kurulumlarında ilk açılış
rehberini, rutin güncellemeleri, isteğe bağlı hazırlık araçlarını ve proje
kaynaklarını tek bir yerel masaüstü akışında birleştiren Qt 6 uygulamasıdır.

---

## Temel Yetenekler

- DNF güncellemelerini Polkit üzerinden yürütür; Flatpak ve Snap adımlarını
  yalnızca ilgili araçlar kuruluysa ekler.
- Güncelleme öncesinde düşük disk alanını, bekleyen yeniden başlatmayı ve
  NVIDIA kart üzerinde etkin `nouveau` modülünü kontrol eder.
- İlk açılışta rehber akışını gösterir; daha sonraki açılışlarda responsive
  bakım panelini açar.
- GameMode, MangoHud ve Vulkan araçlarını; ayrıca `ro-printer-support` ile
  isteğe bağlı yazıcı/tarayıcı desteğini sunar.
- Telemetri tercihini yalnızca yerelde saklar. 0.2.1 sürümü telemetri verisi
  göndermez.
- Grafik sürücüsü kurmaz veya kaldırmaz; riskli grafik durumlarında donanım ve
  sürücü yönetimi için [ro-Control](https://github.com/Project-Ro-ASD/ro-Control)
  uygulamasına yönlendirir.

## Komut Satırı Hızlı Başvuru

| Komut | Açıklama |
| :--- | :--- |
| `ro-assist --help` | Kullanılabilir seçenekleri listeler. |
| `ro-assist --version` | Uygulama sürümünü gösterir. |
| `ro-assist --autostart` | Hoş geldin akışını yalnızca henüz tamamlanmadıysa başlatır. |
| `ro-assist --reset-welcome` | Hoş geldin tamamlanma kaydını siler. |
| `ro-assist --smoke-test` | Arayüzü oluşturur ve çıkar; paketleme doğrulaması içindir. |

## Belgeler

- [Kurulum ve sistem entegrasyonu](INSTALL.md)
- [Derleme ve test](BUILD.md)
- [Mimari ve bakım akışı](ARCHITECTURE.md)
- [Yapılandırma başvurusu](CONFIGURATION.md)

Detaylı teknik belgeler İngilizcedir; uygulama Türkçe, İngilizce, İspanyolca,
Almanca ve Fransızca arayüz metinleri sunar.
