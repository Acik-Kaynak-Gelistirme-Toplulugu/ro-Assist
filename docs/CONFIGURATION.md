# Configuration Reference

ro-Assist uses Qt's INI `QSettings` backend with organization
`Project-Ro-ASD` and application name `ro-assist`. On Fedora, its user-scope
settings are normally stored at:

```text
~/.config/Project-Ro-ASD/ro-assist.conf
```

All values are local to the current user. Edit the file only while ro-Assist is
not running; the UI is the supported configuration surface.

---

## Stored Preferences

| Key | Type | Default | Meaning |
| :--- | :--- | :--- | :--- |
| `welcome/completed` | bool | `false` | Whether the first-run walkthrough has finished. |
| `autostart/welcomeShown` | bool | legacy fallback | Read only as a compatibility fallback for older installations. |
| `telemetry/level` | int | `1` | Selected sharing level: `0` off, `1` count, `2` basic, `3` extended. |
| `telemetry/levelName` | string | `count` | Human-readable equivalent of `telemetry/level`. |
| `printer/supportPreference` | string | unset | User choice: `later`, `disabled`, or `installed`. |

## Resetting Preferences

Reset only the onboarding flow:

```bash
ro-assist --reset-welcome
```

Reset every local preference:

```bash
rm ~/.config/Project-Ro-ASD/ro-assist.conf
```

The next application launch recreates missing keys with their built-in defaults.

## Environment Variables Used by Tests

The following variables are test hooks, not user configuration. They allow the
test suite to replace host inspection results without invoking system commands:

| Variable | Purpose |
| :--- | :--- |
| `RO_ASSIST_SKIP_SYSTEM_CHECKS` | Skips the initial `dnf check-update` call. |
| `RO_ASSIST_TEST_LSPCI` | Replaces `lspci` output. |
| `RO_ASSIST_TEST_LSMOD` | Replaces `lsmod` output. |
| `RO_ASSIST_TEST_REBOOT_REQUIRED` | Overrides reboot detection. |
| `RO_ASSIST_TEST_RO_CONTROL_AVAILABLE` | Overrides ro-Control availability. |

Do not set these variables in desktop launchers or production deployments.
