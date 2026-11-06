#!/usr/bin/env bash
# Installs tiled on the Ubuntu box from an uploaded folder (deploy_ubuntu.ps1 sends
# it to /tmp/tiled-upload): the server under /opt/tiled/bin as the service tiled
# (system user tiled), the web build under /opt/tiled/web, its MySQL database
# (made once and kept: database tiled, user tiled@localhost with a random password
# written only to /opt/tiled/bin/init.json, mode 600, owned by tiled; the schema
# applied every time, idempotent), Caddy's snippet in /etc/caddy/sites/kazin.dev.d/
# (Caddy restarted only when the snippet changed: its admin API is off).
#   sudo bash install.sh [<upload folder>]
set -euo pipefail
SRC="${1:-/tmp/tiled-upload}"
SNIPPET=/etc/caddy/sites/kazin.dev.d/tiled.caddy
CONFIG=/opt/tiled/bin/init.json

id tiled >/dev/null 2>&1 || useradd --system --no-create-home --shell /usr/sbin/nologin tiled

# MySQL's client library the server links (the box's MySQL 8.4 has it)
ldconfig -p | grep -q libmysqlclient.so.24 || DEBIAN_FRONTEND=noninteractive apt-get install -y -qq libmysqlclient24 >/dev/null

install -d -m 755 /opt/tiled /opt/tiled/bin
install -m 755 "$SRC/tiled_server" /opt/tiled/bin/tiled_server.new
mv -f /opt/tiled/bin/tiled_server.new /opt/tiled/bin/tiled_server

# The database: the password made once (kept in init.json), never printed. SQL goes
# through stdin, so the password is on no command line.
if [ ! -s "$CONFIG" ]; then
	PASS="$(head -c 24 /dev/urandom | od -An -tx1 | tr -d ' \n')"
	umask 077
	printf '{\n  "db_host": "localhost",\n  "db_user": "tiled",\n  "db_pass": "%s",\n  "db_name": "tiled"\n}\n' "$PASS" > "$CONFIG.new"
	mv -f "$CONFIG.new" "$CONFIG"
	umask 022
else
	PASS="$(sed -n 's/.*"db_pass": *"\([^"]*\)".*/\1/p' "$CONFIG")"
fi
chown tiled:tiled "$CONFIG"
chmod 600 "$CONFIG"
mysql <<SQL
CREATE DATABASE IF NOT EXISTS \`tiled\` CHARACTER SET utf8mb4;
CREATE USER IF NOT EXISTS 'tiled'@'localhost' IDENTIFIED BY '${PASS}';
ALTER USER 'tiled'@'localhost' IDENTIFIED BY '${PASS}';
GRANT SELECT, INSERT, UPDATE ON \`tiled\`.* TO 'tiled'@'localhost';
SQL
unset PASS
mysql tiled < "$SRC/schema.sql"

# The web build swapped whole (a visitor never gets half of two builds)
rm -rf /opt/tiled/web.new
cp -r "$SRC/web" /opt/tiled/web.new
chmod -R a+rX /opt/tiled/web.new
rm -rf /opt/tiled/web.old
[ -d /opt/tiled/web ] && mv /opt/tiled/web /opt/tiled/web.old
mv /opt/tiled/web.new /opt/tiled/web
rm -rf /opt/tiled/web.old

install -m 644 "$SRC/tiled.service" /etc/systemd/system/tiled.service
systemctl daemon-reload
systemctl enable tiled >/dev/null 2>&1
systemctl restart tiled

if ! cmp -s "$SRC/tiled.caddy" "$SNIPPET"; then
	[ -f "$SNIPPET" ] && cp "$SNIPPET" "$SNIPPET.bak"
	install -m 644 "$SRC/tiled.caddy" "$SNIPPET"
	if ! caddy validate --config /etc/caddy/Caddyfile --adapter caddyfile >/tmp/tiled-caddy.log 2>&1; then
		echo "(!) Caddy refused the snippet:"; tail -5 /tmp/tiled-caddy.log
		if [ -f "$SNIPPET.bak" ]; then mv -f "$SNIPPET.bak" "$SNIPPET"; else rm -f "$SNIPPET"; fi
		exit 1
	fi
	rm -f "$SNIPPET.bak"
	systemctl restart caddy
	echo "Caddy: snippet installed, restarted"
fi

sleep 1
systemctl is-active tiled
journalctl -u tiled -n 6 --no-pager -o cat
