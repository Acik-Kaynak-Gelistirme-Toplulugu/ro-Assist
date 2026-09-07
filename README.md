# ro-Assist

**First-run, maintenance, and safe update center for Ro-ASD systems.**

---

## Overview

**ro-Assist** is the Qt 6 desktop companion for Fedora 44 KDE Plasma-based
Ro-ASD installations. It turns first-login guidance, routine updates, optional
setup actions, and project resources into a single, local desktop workflow.

It deliberately does **not** install or remove graphics drivers. When an NVIDIA
GPU is detected with the `nouveau` module active, ro-Assist highlights the risk
and hands the user to [ro-Control](https://github.com/Project-Ro-ASD/ro-Control)
for driver and hardware management.

### Core Capabilities

- **Controlled update workflow:** Runs DNF updates through Polkit and adds
  Flatpak or Snap update steps only when those tools are installed.
- **Pre-update safety checks:** Detects low root-disk space, pending restarts,
  and the NVIDIA-plus-nouveau graphics state before an update begins.
- **First-run and maintenance hub:** Presents onboarding once, then opens a
  responsive dashboard on subsequent launches.
- **Optional setup:** Offers GameMode, MangoHud, and Vulkan utilities, plus
  opt-in printer and scanner support through `ro-printer-support`.
- **Privacy preference:** Stores a local telemetry preference only; version
  0.2.1 does not transmit telemetry data.
- **Desktop integration:** Uses KDE Breeze when available, follows the desktop
  language/theme where possible, and supplies a first-login autostart entry.

---

## Command-Line Quick Reference

Launching `ro-assist` with no options opens the desktop application.

| Command | Description |
| :--- | :--- |
| `ro-assist --help` | Prints the available command-line options. |
| `ro-assist --version` | Prints the application version. |
| `ro-assist --autostart` | Starts the first-run flow only if it has not already been completed. |
| `ro-assist --reset-welcome` | Clears the completed-onboarding flag; the next launch shows onboarding again. |
| `ro-assist --smoke-test` | Constructs the UI, processes pending events, and exits. Useful for packaging checks. |

---

## Update Safety Model

The maintenance workflow runs these steps in order when available:

1. `pkexec dnf upgrade -y`
2. `flatpak update -y`
3. `pkexec snap refresh`

Before this sequence, ro-Assist evaluates root free space, reboot state, and
the NVIDIA/nouveau combination. A warning is informational and does not change
packages by itself. Privileged package work always goes through Polkit.

---

## Documentation

Guides for users, maintainers, and contributors:

- 📦 **[Installation & System Integration](docs/INSTALL.md):** RPM/source
  installation, installed files, autostart behavior, and removal.
- 🛠️ **[Build & Testing Guide](docs/BUILD.md):** Fedora prerequisites,
  CMake/Ninja builds, CTest, and RPM validation.
- 🏗️ **[Architecture & Maintenance Workflow](docs/ARCHITECTURE.md):** Qt
  Widgets layers, safety checks, process boundaries, and integration points.
- ⚙️ **[Configuration Reference](docs/CONFIGURATION.md):** Local QSettings
  path, preferences, defaults, and reset behavior.
- 🇹🇷 **[Türkçe README](docs/README.tr.md):** Turkish project overview.
- 🤝 **[Contributing Guidelines](CONTRIBUTING.md):** Contribution workflow and
  review expectations.
- 🔒 **[Security Policy](SECURITY.md):** Vulnerability reporting guidance.

---

## License

ro-Assist is open-source software licensed under the **GNU General Public
License v3.0 or later** (GPL-3.0-or-later). See [LICENSE](LICENSE) for details.
