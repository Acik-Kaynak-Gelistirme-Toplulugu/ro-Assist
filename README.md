# ro-Assist

Language: English | [Turkish](docs/README.tr.md)

![Version](https://img.shields.io/badge/version-0.1.2-blue.svg)
![Platform](https://img.shields.io/badge/platform-Fedora_43_KDE-green.svg)
![License](https://img.shields.io/badge/license-GPLv3+-success.svg)

ro-Assist is a Qt6 first-run and maintenance center built specifically for Fedora 43 KDE Plasma based Ro-ASD systems. It guides users through safe update workflows, recommended setup actions, and project resources while keeping hardware driver management delegated to ro Control.

## Features

- Controlled DNF, Flatpak, and Snap update steps with progress and logs.
- Pre-update safety checks for known risk states such as NVIDIA hardware using nouveau, low disk space, and pending reboots.
- ro Control handoff for hardware, GPU, and driver status instead of installing graphics drivers directly.
- Qt6-based desktop interface tuned for Fedora 43 KDE Plasma.
- Package management integration centered on Fedora workflows.
- First-run welcome flow followed by a page dashboard for later launches.
- Basic gaming tool setup for GameMode, MangoHud, and Vulkan utilities.
- Optional printer and scanner support through the `ro-printer-support` package.
- Breeze style and themed icon integration when running inside KDE.

## Tech Stack

- C++17
- Qt6 (`Widgets`, `Network`)
- CMake (with CPack RPM support)

## Requirements

- CMake 3.16+
- C++17 compatible compiler (`gcc` or `clang`)
- Qt6 6.6+ development packages

Fedora 43 example:

```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel
```

## Build

```bash
cmake --preset default
cmake --build --preset default
```

## Test

```bash
ctest --preset default
```

## Run

```bash
./build/ninja/ro-assist
```

## Packaging

RPM metadata is configured in both `CMakeLists.txt` (CPack) and `packaging/rpm/ro-assist.spec`.

GitHub Actions builds RPMs on a Fedora 41 compatibility baseline and validates them on Fedora 43. It produces two simple RPM artifacts:
- `ro-assist-x64`
- `ro-assist-arm64`

The RPM workflow also verifies that:
- `dnf install ro-assist` works from a generated repository
- `/usr/bin/ro-assist` launcher and `/usr/libexec/ro-assist/ro-assist` binary are present
- `ldd -r /usr/libexec/ro-assist/ro-assist` resolves
- RPM requirements do not contain `Qt_6.10` or `Qt_6.10_PRIVATE_API`

When installed on Fedora 43 KDE, the package also ships an autostart entry that launches `ro-assist --autostart` on the first login only.

## Project Structure

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

## Contributing

Contributions are welcome. Please read `CONTRIBUTING.md` before opening a pull request.

## Code of Conduct

This project follows the code of conduct in `CODE_OF_CONDUCT.md`.

## Security

Security reporting guidance is available in `SECURITY.md`.

## License

This project is licensed under GPL-3.0-or-later. See `LICENSE` for details.
