# Makefile Reference

This Makefile automates building, installing, and cleaning the pg_stat_exporter project.

## Targets

- **release**  
  Build an optimized release binary at `release/pg-stat-exporter`.

  ```bash
  make release
  ```

- **debug**  
  Build a debug binary with symbols at `debug/pg-stat-exporter`.

  ```bash
  make debug
  ```

- **all**  
  Build both debug and release binaries.

  ```bash
  make all
  ```

- **install**  
  Installs the binary, systemd service, config, templates, and SQL files.  
  Creates service user/group if missing, sets permissions, and reloads systemd.

  ```bash
  sudo make install
  ```

- **clean**  
  Remove build directories (`debug`, `release`).

  ```bash
  make clean
  ```

- **help**  
  Show help message with available targets.
  
  ```bash
  make help
  ```

## Install Details

- Binary installed to `/usr/bin/pg-stat-exporter`
- Systemd unit files to `/usr/lib/systemd/system/`
- Config to `/etc/default/pg-stat-exporter`
- Templates and SQL files to `/usr/lib/pg-stat-exporter/` and `/etc/pg-stat-exporter/`
- Service user/group: `pg-stat-exporter`
- Permissions and ownership set for service user

## Service Management

After install, manage the service with systemd:

```bash
systemctl start pg-stat-exporter.service
systemctl status pg-stat-exporter.service
```

## Notes

- Requires Civetweb object file at `lib/civetweb.o`
- Links with `libpq`, `libjson-c`, and `libmustach`
- Custom template and SQL directories are supported via defines in the build
