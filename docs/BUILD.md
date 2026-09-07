# Build & Testing Guide

This guide describes building, testing, and validating **ro-Assist** on Fedora
44 or a compatible Ro-ASD development system.

---

## 1. Prerequisites

- CMake 3.16 or newer
- A C++17 compiler (`gcc-c++` or Clang)
- Ninja (recommended by the bundled CMake presets)
- Qt 6.6 or newer: Core, Widgets, Network, Test; LinguistTools is optional for
  generating translation files during a local build

### Fedora 44

```bash
sudo dnf install -y \
  cmake ninja-build gcc-c++ \
  qt6-qtbase-devel qt6-qttools-devel \
  desktop-file-utils appstream rpm-build
```

---

## 2. Build from Source

```bash
git clone https://github.com/Project-Ro-ASD/ro-Assist.git
cd ro-Assist

cmake --preset default
cmake --build --preset default
```

The default preset writes build output to `build/ninja`. To use an independent
build tree instead:

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Run the application from a preset build with:

```bash
./build/ninja/ro-assist
```

---

## 3. Tests

The project uses QtTest and CTest. UI tests run using Qt's offscreen platform
and skip host-system checks, so they do not start package transactions.

```bash
ctest --preset default
```

For a manually configured build tree:

```bash
QT_QPA_PLATFORM=offscreen RO_ASSIST_SKIP_SYSTEM_CHECKS=1 \
ctest --test-dir build --output-on-failure
```

The suite covers update-output parsing, risk detection, welcome-flow state,
language changes, printer/telemetry preferences, and compact-window scrolling.

---

## 4. Packaging Validation

RPM packaging is defined in `packaging/rpm/ro-assist.spec`; CPack metadata is
also kept in `CMakeLists.txt`. Fedora 44 package acceptance requirements are
documented in [fedora44-packaging-note.md](fedora44-packaging-note.md).

Before publishing an RPM, validate the built artifact in a clean Fedora 44
environment:

```bash
rpm -qpl ro-assist-*.rpm
rpm -qpR ro-assist-*.rpm
```

The package must expose `/usr/bin/ro-assist` and
`/usr/libexec/ro-assist/ro-assist`; private Qt ABI dependencies are rejected.
