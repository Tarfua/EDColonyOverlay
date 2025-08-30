#!/usr/bin/env bash
set -euo pipefail

PREFIX=${PREFIX:-$HOME/.local}
BIN_DIR="$PREFIX/bin"
STATE_DIR="${XDG_STATE_HOME:-$HOME/.local/state}/edcolony"

mkdir -p "$BIN_DIR" "$STATE_DIR"
install -m 0755 "build/daemon/edcolony_daemon" "$BIN_DIR/edcolony_daemon"

UNIT_DIR="${XDG_CONFIG_HOME:-$HOME/.config}/systemd/user"
mkdir -p "$UNIT_DIR"
cat > "$UNIT_DIR/edcolony-daemon.service" <<EOF
[Unit]
Description=EDColony Daemon
After=network-online.target

[Service]
Environment=EDCOLONY_JOURNAL_DIR=
Environment=EDCOLONY_SVC_URI=
Environment=EDCOLONY_CMDR=
ExecStart=%h/.local/bin/edcolony_daemon
Restart=on-failure

[Install]
WantedBy=default.target
EOF

systemctl --user daemon-reload || true
echo "Installed edcolony_daemon to $BIN_DIR and user unit to $UNIT_DIR"

