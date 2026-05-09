# ro-Assist

Language: English | [Turkish](docs/README.tr.md)

![Version](https://img.shields.io/badge/version-0.1.0-blue.svg)
![Platform](https://img.shields.io/badge/platform-Fedora_Linux-green.svg)
![License](https://img.shields.io/badge/license-GPLv3+-success.svg)

ro-Assist is a Qt6 desktop assistant built specifically for Fedora KDE Plasma systems. It executes update workflows and reports progress through a simple GUI while preserving native desktop behavior where possible.

## Features

- Real-time update progress tracking from command output parsing.
- Qt6-based desktop interface tuned for Fedora KDE Plasma.
- Package management integration centered on Fedora workflows.
- Breeze style and themed icon integration when running inside KDE.

## Tech Stack

- C++17
- Qt6 (`Widgets`, `Network`)
- CMake (with CPack RPM support)

## Requirements

- CMake 3.16+
- C++17 compatible compiler (`gcc` or `clang`)
- Qt6 development packages

Fedora example:

```bash
sudo dnf install cmake gcc-c++ qt6-qtbase-devel
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## Run

```bash
./build/ro-assist
```

## Packaging

RPM metadata is configured in both `CMakeLists.txt` (CPack) and `packaging/rpm/ro-assist.spec`.

## Project Structure

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

## Contributing

Contributions are welcome. Please read `CONTRIBUTING.md` before opening a pull request.

## Code of Conduct

This project follows the code of conduct in `CODE_OF_CONDUCT.md`.

## License

This project is licensed under GPL-3.0-or-later. See `LICENSE` for details.
