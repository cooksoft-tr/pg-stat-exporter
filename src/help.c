#include <stdio.h>

#include "help.h"

void print_help()
{
  printf(
      "pg-stat-exporter - PostgreSQL metrics exporter\n"
      "\n"
      "Usage:\n"
      "  pg-stat-exporter [OPTIONS]\n"
      "\n"
      "Options:\n"
      "  --dsn=<DSN>\n"
      "      PostgreSQL connection string (DSN).\n"
      "      ENV: PG_STAT_EXPORTER_DSN\n"
      "\n"
      "  --global-metrics=<METRICS>\n"
      "      Comma-separated list of global metrics to collect.\n"
      "      ENV: PG_STAT_EXPORTER_GLOBAL_METRICS\n"
      "\n"
      "  --database-metrics=<METRICS>\n"
      "      Comma-separated list of database-specific metrics to collect.\n"
      "      ENV: PG_STAT_EXPORTER_DATABASE_METRICS\n"
      "\n"
      "  --excluded-databases=<DATABASES>\n"
      "      Comma-separated list of databases to exclude from collection.\n"
      "      ENV: PG_STAT_EXPORTER_EXCLUDED_DATABASES\n"
      "\n"
      "  --http-server\n"
      "      Enable the HTTP server instead of writing metrics to stdout.\n"
      "      ENV: PG_STAT_EXPORTER_HTTP_SERVER\n"
      "\n"
      "  --http-server-options=<OPTIONS>\n"
      "      Comma-separated list of CivetWeb server configuration options.\n"
      "      ENV: PG_STAT_EXPORTER_HTTP_SERVER_OPTIONS\n"
      "\n"
      "  -h, --help\n"
      "      Show this help message and exit.\n"
      "\n");
}