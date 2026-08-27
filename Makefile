VERSION_STRING := $(shell git describe --tags --abbrev=0 2>/dev/null || echo "0.0.0")
CIVETWEB_OBJ := $(wildcard lib/civetweb.o)

# Service user/group
SERVICE_USER=pg-stat-exporter
SERVICE_GROUP=pg-stat-exporter

SRC=$(wildcard src/*.c) $(CIVETWEB_OBJ)

INCLUDES=-Iinclude -I/usr/include -I/usr/local/include
LDFLAGS=-lpq -ljson-c -lmustach -lsystemd

CFLAGS_DEBUG=-O0 -Wall $(INCLUDES) -g
CFLAGS_RELEASE=-O2 -Wall $(INCLUDES)
CFLAGS += -DTEMPLATE_DIR=\"template\" \
					-DSQL_DIR=\"sql\" \
					-DCUSTOM_TEMPLATE_DIR=\"/etc/pg-stat-exporter/template\" \
					-DCUSTOM_SQL_DIR=\"/etc/pg-stat-exporter/sql\" \

# Debug build
debug:
	mkdir -p debug
	gcc $(CFLAGS_DEBUG) $(CFLAGS) $(SRC) -o debug/pg-stat-exporter $(LDFLAGS)

# Release build
release: include/version.h
	mkdir -p release
	gcc $(CFLAGS_RELEASE) $(CFLAGS) $(SRC) -o release/pg-stat-exporter $(LDFLAGS)

version:
	sed 's/@VERSION_STRING@/$(VERSION_STRING)/' resources/version.h.in > include/version.h

# Default build
all: version debug release

# Install files
install:
	# Create service user and group if they don't exist
	getent group $(SERVICE_GROUP) >/dev/null || groupadd -r $(SERVICE_GROUP)
	getent passwd $(SERVICE_USER) >/dev/null || \
			useradd -r -g $(SERVICE_GROUP) -d /var/lib/$(SERVICE_USER) \
			-s /sbin/nologin -c "PostgreSQL Stat Exporter Service User" $(SERVICE_USER)

	# Create required directories
	install -D -m 644 package/pg-stat-exporter.defaults /etc/default/pg-stat-exporter
	install -D -m 644 package/pg-stat-exporter.service /usr/lib/systemd/system/pg-stat-exporter.service
	install -D -m 755 release/pg-stat-exporter /usr/bin/pg-stat-exporter
	
	find sql -type d -exec install -d -m 755 -o $(SERVICE_USER) -g $(SERVICE_GROUP) /usr/lib/pg-stat-exporter/{} \;
	find sql -type f -exec install -m 644 -o $(SERVICE_USER) -g $(SERVICE_GROUP) {} /usr/lib/pg-stat-exporter/{} \;
	
	find template -type d -exec install -d -m 755 -o $(SERVICE_USER) -g $(SERVICE_GROUP) /usr/lib/pg-stat-exporter/{} \;
	find template -type f -exec install -m 644 -o $(SERVICE_USER) -g $(SERVICE_GROUP) {} /usr/lib/pg-stat-exporter/{} \;
	
	install -d -m 755 -o $(SERVICE_USER) -g $(SERVICE_GROUP) /etc/pg-stat-exporter
	find sql -type d -exec install -d -m 755 -o $(SERVICE_USER) -g $(SERVICE_GROUP) /etc/pg-stat-exporter/{} \;
	find template -type d -exec install -d -m 755 -o $(SERVICE_USER) -g $(SERVICE_GROUP) /etc/pg-stat-exporter/{} \;

	# Set permissions
	chown $(SERVICE_USER):$(SERVICE_GROUP) /usr/bin/pg-stat-exporter

	# Reload systemd
	systemctl daemon-reload
	systemctl start pg-stat-exporter.service

# Temizlik
clean:
	rm -rf debug release include/version.h

# Show help
help:
	@echo "pg_stat_exporter Makefile targets:"
	@echo "  release   - Build optimized release binary"
	@echo "  debug     - Build debug binary with symbols"
	@echo "  version   - Generate version header from <git tags>"
	@echo "  all       - Build both debug and release binaries"
	@echo "  install   - Install binaries, configs, systemd units, templates, and SQL files"
	@echo "  clean     - Remove build directories"
	@echo "  help      - Show this help message"

.PHONY: debug release version clean install help
