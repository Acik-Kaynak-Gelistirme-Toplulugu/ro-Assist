# Ro Assist Packaging Note

This package is expected to install inside the Ro-ASD Fedora 43 KDE ISO directly from `ro-repo`.

## Target Environment

- Fedora 43 KDE Plasma
- Installation source: active `ro-repo`

Expected install path during ISO build:

```bash
dnf -y --refresh --setopt=install_weak_deps=False install ro-assist
rpm -q ro-assist
command -v ro-assist
```

If any command fails, the ISO build must stop.

## Packaging Rules

- `ro-assist` must be built in a Fedora 43 environment.
- Builds from Fedora 44, Rawhide, or Qt 6.10 environments are not accepted.
- The package must provide `/usr/bin/ro-assist`.
- The package must install an autostart entry for the first KDE login.
- The package must remain compatible with Fedora 43 Qt libraries.
- Repository metadata must be regenerated after publishing the RPM.

## Required Acceptance Checks

These checks must pass in a clean Fedora 43 KDE, chroot, or mock environment:

```bash
dnf clean all
dnf --refresh install ro-assist
rpm -q ro-assist
command -v ro-assist
ldd -r /usr/bin/ro-assist
rpm -qpR ro-assist-*.rpm
```

If `rpm -qpR` shows either of the following, the package is still considered incompatible for Fedora 43:

- `Qt_6.10`
- `Qt_6.10_PRIVATE_API`

## CI Enforcement

GitHub Actions enforces this policy by:

- building RPM artifacts inside `fedora:43`
- validating the desktop file and AppStream metadata
- creating a temporary repo from the built RPM
- installing `ro-assist` through `dnf install ro-assist`
- verifying `/usr/bin/ro-assist`
- checking `ldd -r /usr/bin/ro-assist`
- rejecting RPMs that require `Qt_6.10` or `Qt_6.10_PRIVATE_API`

## First Login Behavior

- The package installs `/etc/xdg/autostart/ro-assist-autostart.desktop`.
- KDE launches `ro-assist --autostart`.
- `ro-assist` records that the welcome launch has already happened and exits immediately on later autostart runs.
