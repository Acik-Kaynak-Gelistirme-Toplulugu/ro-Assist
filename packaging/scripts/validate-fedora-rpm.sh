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

test "$(rpm -E '%{fedora}')" = "44"
test "$(basename "$rpm_path")" = "$(rpm -qp --qf '%{NVRA}.rpm' "$rpm_path")"
test "$(rpm -qp --qf '%{RELEASE}' "$rpm_path")" = "1.fc44"

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

if grep -q '_PRIVATE_API' <<< "$requirements"; then
  echo "RPM must not depend on private Qt ABI symbols." >&2
  exit 1
fi
