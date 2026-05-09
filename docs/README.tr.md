# ro-Assist

Dil: Turkce | [English](../README.md)

![Version](https://img.shields.io/badge/version-0.1.1-blue.svg)
![Platform](https://img.shields.io/badge/platform-Fedora_43_KDE-green.svg)
![License](https://img.shields.io/badge/license-GPLv3+-success.svg)

ro-Assist, ozellikle Fedora 43 KDE Plasma sistemleri icin gelistirilmis Qt6 tabanli bir masaustu yardimcisidir. Guncelleme islemlerini calistirir ve mumkun oldugunca masaustu ile uyumlu davranis koruyarak ilerleme durumunu sade bir arayuzde gosterir.

## Ozellikler

- Komut ciktilarini ayrisarak gercek zamanli guncelleme ilerlemesi gosterebilir.
- Fedora 43 KDE Plasma hedefi icin uyarlanmis Qt6 tabanli masaustu arayuzu.
- Fedora odakli paket yonetimi is akislarina entegrasyon.
- KDE icinde calisirken Breeze stil ve tema ikon entegrasyonu.

## Teknoloji Yigini

- C++17
- Qt6 (`Widgets`, `Network`)
- CMake (CPack RPM destegi ile)

## Gereksinimler

- CMake 3.16+
- C++17 uyumlu derleyici (`gcc` veya `clang`)
- Qt6 gelistirme paketleri

Fedora 43 ornegi:

```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel
```

## Derleme

```bash
cmake --preset default
cmake --build --preset default
```

## Test

```bash
ctest --preset default
```

## Calistirma

```bash
./build/ninja/ro-assist
```

## Paketleme

RPM metaverisi hem `CMakeLists.txt` (CPack) hem de `packaging/rpm/ro-assist.spec` dosyasinda tanimlidir.

GitHub Actions, RPM paketlerini Fedora 41 uyumluluk tabaninda derler ve Fedora 43 uzerinde dogrular. Iki sade cikti uretir:
- `ro-assist-x64`
- `ro-assist-arm64`

RPM workflow su kontrolleri de zorunlu olarak yapar:
- uretilen repo uzerinden `dnf install ro-assist`
- `/usr/bin/ro-assist` dogrulamasi
- `ldd -r /usr/bin/ro-assist`
- `Qt_6.10` ve `Qt_6.10_PRIVATE_API` bagimlilik denetimi

Paket, Fedora 43 KDE uzerinde ilk oturum acilisinda `ro-assist --autostart` calistiran bir autostart girdisi de icerir. Uygulama bu acilisi yalnizca ilk giriste gosterir.

## Proje Yapisi

```text
.
|- docs/
|- include/roassist/
|- packaging/linux/
|- packaging/rpm/
|- resources/icons/
|- src/
|- tests/
|- CMakeLists.txt
`- .github/
```

## Katki

Katkilar memnuniyetle karsilanir. Pull request acmadan once `CONTRIBUTING.md` dosyasini inceleyin.

## Davranis Kurallari

Bu proje `CODE_OF_CONDUCT.md` dosyasindaki davranis kurallarini takip eder.

## Guvenlik

Guvenlik bildirimi sureci `SECURITY.md` dosyasinda tanimlidir.

## Lisans

Bu proje GPL-3.0-or-later ile lisanslanmistir. Ayrintilar icin `LICENSE` dosyasina bakin.
