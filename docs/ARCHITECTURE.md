# Architecture & Maintenance Workflow

**ro-Assist** is a single-process C++17 / Qt Widgets desktop application. It
keeps UI concerns, local system inspection, and privileged package actions
separate.

---

## 1. High-Level Architecture

```text
┌──────────────────────────────────────────────────────┐
│ Qt Widgets UI                                         │
│ Welcome flow · dashboard · updates · setup · privacy  │
└───────────────────────┬──────────────────────────────┘
                        │
┌───────────────────────▼──────────────────────────────┐
│ MainWindow                                             │
│ UI state · QSettings · QProcess output · localization │
└───────────────┬──────────────────────┬───────────────┘
                │                      │
┌───────────────▼────────────┐ ┌───────▼────────────────┐
│ SystemUpdateService         │ │ SystemRiskService       │
│ DNF / Flatpak / Snap plan   │ │ disk, reboot, GPU state │
└───────────────┬────────────┘ └───────┬────────────────┘
                │                      │
┌───────────────▼──────────────────────▼───────────────┐
│ External system boundary                              │
│ pkexec · dnf · flatpak · snap · lspci · lsmod         │
│ needs-restarting · ro-control · KDE utility commands  │
└──────────────────────────────────────────────────────┘
```

## 2. Source Layout

| Area | Responsibility |
| :--- | :--- |
| `src/main.cpp` | CLI parsing, application metadata, KDE/Breeze setup, first-login gating. |
| `src/mainwindow.cpp` | Widgets UI, navigation, localization, preferences, and asynchronous process handling. |
| `src/system_services.cpp` | Update-plan construction, risk collection, and ro-Control availability/launch integration. |
| `src/update_helpers.cpp` | DNF output and transaction-progress parsing. |
| `src/ui_texts.cpp` | Runtime text catalogue for Turkish, English, Spanish, German, and French. |
| `tests/` | Unit tests plus offscreen integration tests. |

## 3. Update Workflow

1. The app runs `dnf check-update` to establish initial update status.
2. It detects whether Flatpak and Snap commands are installed.
3. `SystemRiskService` collects available root-disk space, reboot status,
   NVIDIA presence, `nouveau` module status, session type, and ro-Control
   availability.
4. On user confirmation, the update plan sequentially launches DNF, Flatpak,
   and Snap commands. Privileged DNF/Snap commands use `pkexec`.
5. Output is parsed for transaction/download progress and rendered in the UI.

No graphics driver transaction is performed by ro-Assist. The
NVIDIA-plus-nouveau warning offers a ro-Control handoff instead.

## 4. Responsive UI Boundary

`QStackedWidget` owns the top-level pages. Long dashboard, printer-support,
and privacy/telemetry content is hosted in `QScrollArea` containers so a short
display (including 1366×768 systems) retains access to every action instead of
overlapping controls.

## 5. Persistence and Privacy

The application uses the Qt INI `QSettings` backend under the Project-Ro-ASD
organization. It persists onboarding, printer-support, and telemetry choices
locally. The telemetry screen records a future-sharing preference; it does not
create a network telemetry client in this release.
