# pg_stat_exporter

A C-based exporter for PostgreSQL.
It can run as a standalone CLI tool.

## Introduction

pg_stat_exporter is designed to collect statistics data from PostgreSQL and serve them as Prometheus-compatible metrics over an HTTP endpoint. It is lightweight and Linux-oriented.
Additionally, it can be used as a command-line tool to output metrics directly to stdout.

## For Developers

- **OS:** Debian 13 (recommended)
- **IDE:** VSCode
- **Dependencies:**
  - `build-essential`, `gcc`, `make`, `libpq-dev`, `libsystemd-dev`, `libjson-c-dev`, `pkg-config`
  - Civetweb library (for HTTP server)

  ```bash
  sudo apt install build-essential gcc make libpq-dev libsystemd-dev libjson-c-dev pkg-config
  sudo ldconfig

  cd ..
  git clone https://github.com/civetweb/civetweb.git
  cd civetweb
  git checkout v1.16
  make help
  make lib
  mkdir -p ../pg-stat-exporter/lib
  cp out/src/civetweb.o ../pg-stat-exporter/lib/.
  cp include/civetweb.h ../pg-stat-exporter/include/.
  ```

## Compilation

See [Makefile.md](Makefile.md) for build and install instructions.
