#!/usr/bin/env bash
set -euo pipefail

if [[ $# -ne 1 ]]; then
  echo "Usage: $0 <rpm-path>" >&2
  exit 1
fi

rpm_path="$1"

if [[ ! -f "$rpm_path" ]]; then
  echo "RPM not found: $rpm_path" >&2
  exit 1
fi

work_dir="$(mktemp -d)"
repo_dir="$work_dir/repo"
repo_file="/etc/yum.repos.d/ro-assist-local.repo"

cleanup() {
  rm -f "$repo_file"
  rm -rf "$work_dir"
}
trap cleanup EXIT

mkdir -p "$repo_dir"
cp "$rpm_path" "$repo_dir/"
createrepo_c "$repo_dir" >/dev/null

cat > "$repo_file" <<EOF
[ro-assist-local]
name=ro-assist local
baseurl=file://$repo_dir
enabled=1
gpgcheck=0
repo_gpgcheck=0
EOF

dnf clean all

if rpm -q ro-assist >/dev/null 2>&1; then
  dnf -y remove ro-assist
fi

dnf -y --refresh --setopt=install_weak_deps=False install ro-assist
rpm -q ro-assist
binary_path="$(command -v ro-assist)"
printf '%s\n' "$binary_path"
test -n "$binary_path"
test -x /usr/bin/ro-assist
test "$(realpath "$binary_path")" = "/usr/bin/ro-assist"
test -x /usr/libexec/ro-assist/ro-assist
test -f /etc/xdg/autostart/ro-assist-autostart.desktop
ldd -r /usr/libexec/ro-assist/ro-assist

requirements="$(rpm -qpR "$rpm_path")"
printf '%s\n' "$requirements"

fedora_major="$(rpm -E '%{fedora}' 2>/dev/null || echo 43)"
# Fedora N ships Qt 6.(N-32). Fedora 43 → Qt 6.11.
qt_threshold="$(( fedora_major - 32 ))"

while IFS= read -r req; do
  if [[ $req =~ Qt_6\.[0-9]+(_PRIVATE_API)? ]]; then
    match="${BASH_REMATCH[0]}"
    ver="${match#Qt_6.}"
    ver="${ver%_PRIVATE_API}"
    if (( ver >= qt_threshold )); then
      echo "RPM requires Qt 6.${ver} symbols; Fedora ${fedora_major} only provides up to Qt 6.${qt_threshold}." >&2
      exit 1
    fi
  fi
done <<< "$requirements"
