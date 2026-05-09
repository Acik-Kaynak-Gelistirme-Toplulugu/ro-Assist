# ro-Assist

Dil: Turkce | [English](../README.md)

![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Fedora_Linux-green.svg)
![License](https://img.shields.io/badge/license-GPLv3+-success.svg)

ro-Assist, ozellikle Fedora KDE Plasma sistemleri icin gelistirilmis Qt6 tabanli bir masaustu yardimcisidir. Guncelleme islemlerini calistirir ve mumkun oldugunca masaustu ile uyumlu davranis koruyarak ilerleme durumunu sade bir arayuzde gosterir.

## Ozellikler

- Komut ciktilarini ayrisarak gercek zamanli guncelleme ilerlemesi gosterebilir.
- Fedora KDE Plasma hedefi icin uyarlanmis Qt6 tabanli masaustu arayuzu.
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

Fedora ornegi:

```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel
```

## Derleme

```bash
cmake -S . -B build
cmake --build build
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Calistirma

```bash
./build/ro-assist
```

## Paketleme

RPM metaverisi hem `CMakeLists.txt` (CPack) hem de `packaging/rpm/ro-assist.spec` dosyasinda tanimlidir.

## Proje Yapisi

```text
.
|- docs/
|- include/roassist/
|- packaging/rpm/
|- resources/icons/
|- src/
|- CMakeLists.txt
`- .github/
```

## Katki

Katkilar memnuniyetle karsilanir. Pull request acmadan once `CONTRIBUTING.md` dosyasini inceleyin.

## Davranis Kurallari

Bu proje `CODE_OF_CONDUCT.md` dosyasindaki davranis kurallarini takip eder.

## Lisans

Bu proje GPL-3.0-or-later ile lisanslanmistir. Ayrintilar icin `LICENSE` dosyasina bakin.
