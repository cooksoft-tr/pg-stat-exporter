# Prometheus Installation and Configuration

This guide shows how to quickly set up a simple Prometheus environment on Linux for testing pg-stat-exporter with PostgreSQL.

## Installation

Download the Prometheus package:

```bash
wget https://github.com/prometheus/prometheus/releases/download/v3.5.0/prometheus-3.5.0.linux-amd64.tar.gz
```

Extract the archive:

```bash
tar -xf prometheus-3.5.0.linux-amd64.tar.gz
```

Create a symbolic link for easier access:

```bash
ln -s prometheus-3.5.0.linux-amd64 prometheus
chmod 770 prometheus/prometheus
```

## Configuration

Open the Prometheus configuration file:

```bash
vim /etc/prometheus/prometheus.yml
```

Example configuration:

```yml
global:
  scrape_interval: 5s
  evaluation_interval: 5s

scrape_configs:
  - job_name: "prometheus"
    static_configs:
      - targets: ["localhost:9187"]
        labels:
          app: "pg-stat-exporter"
      - targets: ["localhost:9100"]
        labels:
          app: "node-exporter"
```

### Explanation

* `scrape_interval`: Frequency of collecting metrics from targets.
* `evaluation_interval`: Frequency of evaluating rules and alerts.
* `job_name`: Name of the monitoring job.
* `targets`: Endpoints to monitor.
* `labels`: Tags for filtering in queries.

## Start Prometheus

Run Prometheus with the following command:

```bash
./prometheus/prometheus
```

Prometheus will start and begin collecting metrics from the specified targets.
