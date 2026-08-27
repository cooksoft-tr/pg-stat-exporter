# Grafana Installation and Configuration

This guide explains how to set up a minimal Grafana environment on Linux for visualizing PostgreSQL and pg-stat-exporter metrics collected by Prometheus.

## Installation

Add Grafana’s GPG key:

```bash
sudo wget -O /etc/apt/keyrings/repo.grafana.asc https://packages.grafana.com/gpg.key
```

Add the Grafana APT repository:

```bash
echo "deb [signed-by=/etc/apt/keyrings/repo.grafana.asc] https://packages.grafana.com/oss/deb stable main" \
  | sudo tee /etc/apt/sources.list.d/grafana.list
```

Update the package index and install Grafana:

```bash
sudo apt update
sudo apt install grafana
```

Configure Listening Address and Port for localhost only by editing `/etc/grafana/grafana.ini`:

```ini
[server]
http_addr = ::1
http_port = 3000
```

Enable and start the Grafana service:

```bash
sudo systemctl daemon-reload
sudo systemctl enable grafana-server
sudo systemctl start grafana-server
```

Check service status:

```bash
systemctl status grafana-server
```

## Accessing Grafana

Open your browser and navigate to:

<http://localhost:3000/>

Default credentials:

* **Username:** admin
* **Password:** admin

You will be prompted to change the password on first login.

## Add Prometheus as a Data Source

Once logged in:

1. Go to **Connections → Data Sources**
2. Click **Add data source**
3. Select **Prometheus**
4. Set the URL:

```ini
http://localhost:9090
```

5. Click **Save & Test** to verify the connection.

Grafana should display *"Data source is working"* if the configuration is correct.

## Verify pg-stat-exporter Metrics

Ensure Prometheus is scraping pg-stat-exporter:

```
http://localhost:9187/metrics
```

All PostgreSQL statistics exposed by the exporter should be visible on this page.

## Creating Dashboards

To create a dashboard:

* Navigate to **Create → Dashboard**
* Click **Add visualization**
* Select the Prometheus data source
* Enter PromQL queries to visualize metrics

You can also import existing dashboards:

**Dashboards → Import**

Upload any `.json` dashboard files prepared for pg-stat-exporter.

## Summary

With this setup:

* Grafana is installed and running
* Prometheus is configured as a data source
* Metrics from pg-stat-exporter are accessible in Grafana
* Dashboards can be created or imported to visualize PostgreSQL internals
