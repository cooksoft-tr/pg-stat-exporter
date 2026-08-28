#!/usr/bin/env bash

set -euo pipefail

# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------

PACKAGE_NAME="pg-stat-exporter"
VERSION="$(release/pg-stat-exporter --version)"
ARCH="$(dpkg --print-architecture)"

SERVICE_USER="pg-stat-exporter"
SERVICE_GROUP="pg-stat-exporter"

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/build/deb"
PKG_DIR="${BUILD_DIR}/${PACKAGE_NAME}"
DEB_FILE="${ROOT_DIR}/build/${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"

# -----------------------------------------------------------------------------
# Helpers
# -----------------------------------------------------------------------------

log() {
    echo "[+] $*"
}

error() {
    echo "[!] $*" >&2
    exit 1
}

require_file() {
    local file="$1"

    if [[ ! -f "${ROOT_DIR}/${file}" ]]; then
        error "Required file not found: ${file}"
    fi
}

require_dir() {
    local dir="$1"

    if [[ ! -d "${ROOT_DIR}/${dir}" ]]; then
        error "Required directory not found: ${dir}"
    fi
}

# -----------------------------------------------------------------------------
# Validation
# -----------------------------------------------------------------------------

command -v dpkg-deb >/dev/null 2>&1 \
    || error "dpkg-deb is required. Install Debian packaging tools."

require_file "release/pg-stat-exporter"
require_file "package/pg-stat-exporter.defaults"
require_file "package/pg-stat-exporter.service"
require_dir "sql"
require_dir "template"

# -----------------------------------------------------------------------------
# Clean build directory
# -----------------------------------------------------------------------------

log "Cleaning build directory..."

rm -rf "${PKG_DIR}"
mkdir -p "${PKG_DIR}"

# -----------------------------------------------------------------------------
# Debian control directory
# -----------------------------------------------------------------------------

mkdir -p "${PKG_DIR}/DEBIAN"

cat > "${PKG_DIR}/DEBIAN/control" <<EOF
Package: ${PACKAGE_NAME}
Version: ${VERSION}
Section: database
Priority: optional
Architecture: ${ARCH}
Maintainer: PostgreSQL Stat Exporter Maintainers
Depends: systemd, adduser
Homepage: https://github.com/cooksoft-tr/pg-stat-exporter
Description: PostgreSQL statistics exporter
 PostgreSQL statistics exporter service.
EOF

# -----------------------------------------------------------------------------
# Package directories
# -----------------------------------------------------------------------------

log "Creating package directories..."

mkdir -p \
    "${PKG_DIR}/usr/bin" \
    "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/sql" \
    "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/template" \
    "${PKG_DIR}/usr/lib/systemd/system" \
    "${PKG_DIR}/etc/default" \
    "${PKG_DIR}/etc/${PACKAGE_NAME}/sql" \
    "${PKG_DIR}/etc/${PACKAGE_NAME}/template"

# -----------------------------------------------------------------------------
# Binary
# -----------------------------------------------------------------------------

log "Installing binary..."

install -D -m 0755 \
    "${ROOT_DIR}/release/pg-stat-exporter" \
    "${PKG_DIR}/usr/bin/pg-stat-exporter"

# -----------------------------------------------------------------------------
# Default configuration
# -----------------------------------------------------------------------------

log "Installing default configuration..."

install -D -m 0644 \
    "${ROOT_DIR}/package/pg-stat-exporter.defaults" \
    "${PKG_DIR}/etc/default/pg-stat-exporter"

# -----------------------------------------------------------------------------
# Systemd service
# -----------------------------------------------------------------------------

log "Installing systemd service..."

install -D -m 0644 \
    "${ROOT_DIR}/package/pg-stat-exporter.service" \
    "${PKG_DIR}/usr/lib/systemd/system/pg-stat-exporter.service"

# -----------------------------------------------------------------------------
# SQL files
# -----------------------------------------------------------------------------

log "Installing SQL files..."

cp -a \
    "${ROOT_DIR}/sql/." \
    "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/sql/"

find "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/sql" \
    -type d \
    -exec chmod 0755 {} \;

find "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/sql" \
    -type f \
    -exec chmod 0644 {} \;

find "${PKG_DIR}/etc/${PACKAGE_NAME}/sql" \
    -type d \
    -exec chmod 0755 {} \;

# -----------------------------------------------------------------------------
# Template files
# -----------------------------------------------------------------------------

log "Installing template files..."

cp -a \
    "${ROOT_DIR}/template/." \
    "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/template/"

find "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/template" \
    -type d \
    -exec chmod 0755 {} \;

find "${PKG_DIR}/usr/lib/${PACKAGE_NAME}/template" \
    -type f \
    -exec chmod 0644 {} \;

find "${PKG_DIR}/etc/${PACKAGE_NAME}/template" \
    -type d \
    -exec chmod 0755 {} \;

# -----------------------------------------------------------------------------
# postinst
# -----------------------------------------------------------------------------

log "Creating postinst..."

cat > "${PKG_DIR}/DEBIAN/postinst" <<EOF
#!/bin/sh
set -e

SERVICE_USER="${SERVICE_USER}"
SERVICE_GROUP="${SERVICE_GROUP}"

# Create service group if it does not exist.
if ! getent group "\${SERVICE_GROUP}" >/dev/null 2>&1; then
    groupadd --system "\${SERVICE_GROUP}"
fi

# Create service user if it does not exist.
if ! getent passwd "\${SERVICE_USER}" >/dev/null 2>&1; then
    useradd \
        --system \
        --gid "\${SERVICE_GROUP}" \
        --home-dir "/var/lib/\${SERVICE_USER}" \
        --shell /usr/sbin/nologin \
        --comment "PostgreSQL Stat Exporter Service User" \
        "\${SERVICE_USER}"
fi

# Create required directories.
install -d \
    -m 0755 \
    -o "\${SERVICE_USER}" \
    -g "\${SERVICE_GROUP}" \
    "/var/lib/\${SERVICE_USER}"

install -d \
    -m 0755 \
    -o "\${SERVICE_USER}" \
    -g "\${SERVICE_GROUP}" \
    "/etc/${PACKAGE_NAME}"

# Service data.
chown -R \
    "\${SERVICE_USER}:\${SERVICE_GROUP}" \
    "/usr/lib/${PACKAGE_NAME}"

# Reload systemd.
systemctl daemon-reload

# Enable service.
systemctl enable ${PACKAGE_NAME}.service >/dev/null 2>&1 || true

# Start/restart service.
if systemctl is-active --quiet ${PACKAGE_NAME}.service; then
    systemctl restart ${PACKAGE_NAME}.service
else
    systemctl start ${PACKAGE_NAME}.service
fi

exit 0
EOF

chmod 0755 "${PKG_DIR}/DEBIAN/postinst"

# -----------------------------------------------------------------------------
# prerm
# -----------------------------------------------------------------------------

log "Creating prerm..."

cat > "${PKG_DIR}/DEBIAN/prerm" <<EOF
#!/bin/sh
set -e

case "\$1" in
    remove|deconfigure)
        systemctl stop ${PACKAGE_NAME}.service >/dev/null 2>&1 || true
        systemctl disable ${PACKAGE_NAME}.service >/dev/null 2>&1 || true
        ;;
esac

exit 0
EOF

chmod 0755 "${PKG_DIR}/DEBIAN/prerm"

# -----------------------------------------------------------------------------
# postrm
# -----------------------------------------------------------------------------

log "Creating postrm..."

cat > "${PKG_DIR}/DEBIAN/postrm" <<EOF
#!/bin/sh
set -e

systemctl daemon-reload >/dev/null 2>&1 || true

exit 0
EOF

chmod 0755 "${PKG_DIR}/DEBIAN/postrm"

# -----------------------------------------------------------------------------
# Permissions
# -----------------------------------------------------------------------------

log "Setting permissions..."

chmod 0755 "${PKG_DIR}/usr/bin/pg-stat-exporter"

chmod 0644 \
    "${PKG_DIR}/etc/default/pg-stat-exporter" \
    "${PKG_DIR}/usr/lib/systemd/system/pg-stat-exporter.service"

# -----------------------------------------------------------------------------
# Build package
# -----------------------------------------------------------------------------

log "Building Debian package..."

rm -f "${DEB_FILE}"

dpkg-deb \
    --build \
    --root-owner-group \
    "${PKG_DIR}" \
    "${DEB_FILE}"

# -----------------------------------------------------------------------------
# Verify
# -----------------------------------------------------------------------------

log "Checking package..."

dpkg-deb --info "${DEB_FILE}"
echo
dpkg-deb --contents "${DEB_FILE}"

echo
log "Debian package created:"
echo "    ${DEB_FILE}"