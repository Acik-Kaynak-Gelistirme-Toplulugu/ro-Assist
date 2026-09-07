# Installation & System Integration

This guide covers installing **ro-Assist** on Fedora 44 KDE Plasma and
Ro-ASD-based systems.

---

## 1. Install an RPM

Install a release RPM with DNF so dependencies are resolved automatically:

```bash
sudo dnf install -y ./ro-assist-<version>-<release>.<arch>.rpm
```

The package targets `x86_64` and `aarch64` Fedora 44 systems. It requires a
KDE session for the first-login autostart entry; the main application can still
be started manually with `ro-assist`.

## 2. Installed Files

| Component | Path | Purpose |
| :--- | :--- | :--- |
| Launcher | `/usr/bin/ro-assist` | Sets Qt platform defaults, then starts the application. |
| Application | `/usr/libexec/ro-assist/ro-assist` | Qt desktop binary. |
| Desktop entry | `/usr/share/applications/ro-assist.desktop` | Menu/Discover integration. |
| Autostart entry | `/etc/xdg/autostart/ro-assist-autostart.desktop` | First KDE login launch. |
| AppStream metadata | `/usr/share/metainfo/io.github.project_ro_asd.ro_assist.metainfo.xml` | Discover metadata. |
| Icon | `/usr/share/icons/hicolor/scalable/apps/ro-assist.svg` | Application icon. |

## 3. First-Login Behavior

KDE launches `ro-assist --autostart` after the panel is available. The command
opens the onboarding UI only when `welcome/completed` is false; later autostart
runs exit without showing a window.

To show onboarding again:

```bash
ro-assist --reset-welcome
ro-assist
```

## 4. Optional Integrations

- **ro-Control:** Provides the separate graphics-driver and hardware-management
  workflow. ro-Assist opens it only when installed and required by a detected
  graphics risk.
- **Discover:** The app-store card attempts `plasma-discover`, then `discover`.
- **Printer support:** Selecting installation runs `pkexec dnf install -y
  ro-printer-support`.
- **KDE printer/scanner tools:** ro-Assist opens `kcmshell6 kcm_printer_manager`
  and `skanpage` when those utilities are available.

## 5. Removal

```bash
sudo dnf remove -y ro-assist
```

Removing the RPM does not need to remove user preferences. To reset them
manually, delete `~/.config/Project-Ro-ASD/ro-assist.conf` while the app is not
running.
