#ifndef CONFIG_H
#define CONFIG_H

#define SUPPORTED_PG_VERSIONS_COUNT 1
static const int SUPPORTED_PG_VERSIONS[SUPPORTED_PG_VERSIONS_COUNT] = {18};

typedef struct
{
    const char *dsn;                  // PostgreSQL connection string (DSN)
    int postgres_version;             // PostgreSQL server version
    
    char **global_metrics;            // List of global metrics to collect
    int global_metrics_count;
    
    char **database_metrics;          // List of database-specific metrics to collect
    int database_metrics_count;
    
    char **included_databases;        // Databases to include when collecting database metrics
    char **included_database_dsn;     // DSN list built for included databases
    int included_database_count;

    char *excluded_databases;         // Comma-separated list of database names to exclude from collection
    
    int use_http_server;              // 1: use HTTP server, 0: output to stdout
    char **http_server_options;       // CivetWeb server configuration options (see https://civetweb.github.io/civetweb/UserManual.html)
    
} PostgresMetricsConfig;

PostgresMetricsConfig *load_config(int argc, char **argv);

#endif
