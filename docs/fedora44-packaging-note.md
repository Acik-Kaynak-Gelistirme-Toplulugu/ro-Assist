# Ro Assist Packaging Note

This package is expected to install inside the Ro-ASD Fedora 44 KDE ISO directly from `ro-repo`.

## Target Environment

- Fedora 44 KDE Plasma
- Installation source: active `ro-repo`

Expected install path during ISO build:

```bash
dnf -y --refresh --setopt=install_weak_deps=False install ro-assist
rpm -q ro-assist
command -v ro-assist
```

If any command fails, the ISO build must stop.

## Packaging Rules

- `ro-assist` must be built in a Fedora 44 environment.
- Builds from Fedora releases other than 44 or from Rawhide are not accepted.
- The package must provide the `/usr/bin/ro-assist` launcher and `/usr/libexec/ro-assist/ro-assist` Qt binary.
- The package must install an autostart entry for the first KDE login.
- The package must remain compatible with Fedora 44 Qt libraries.
- Repository metadata must be regenerated after publishing the RPM.

## Required Acceptance Checks

These checks must pass in a clean Fedora 44 KDE, chroot, or mock environment:

```bash
dnf clean all
dnf --refresh install ro-assist
rpm -q ro-assist
command -v ro-assist
test -x /usr/libexec/ro-assist/ro-assist
ldd -r /usr/libexec/ro-assist/ro-assist
rpm -qpR ro-assist-*.rpm
```

If `rpm -qpR` shows any `Qt_*_PRIVATE_API` dependency, the package is considered incompatible for Fedora 44.

## CI Enforcement

GitHub Actions enforces this policy by:

- building RPM artifacts directly in Fedora 44
- validating the desktop file and AppStream metadata
- validating installation inside `fedora:44`
- creating a temporary repo from the built RPM
- installing `ro-assist` through `dnf install ro-assist`
- verifying `/usr/bin/ro-assist` and `/usr/libexec/ro-assist/ro-assist`
- checking `ldd -r /usr/libexec/ro-assist/ro-assist`
- rejecting RPMs that require private Qt ABI symbols

## First Login Behavior

- The package installs `/etc/xdg/autostart/ro-assist-autostart.desktop`.
- KDE launches `ro-assist --autostart`.
- `ro-assist` records that the welcome launch has already happened and exits immediately on later autostart runs.
