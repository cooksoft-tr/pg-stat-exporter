#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>

#include "config.h"
#include "util.h"

static const char *default_metrics[] = {"pg_setting_cluster_name", NULL};

// 1. Returns a NULL-terminated dynamic array from CSV only
char **csv_to_array(const char *csv)
{
  if (!csv || !*csv)
    return NULL;

  char *tmp = strdup(csv);
  if (!tmp)
    return NULL;

  char **array = NULL;
  size_t count = 0;

  char *tok = strtok(tmp, ",");

  while (tok)
  {
    char **new_array = realloc(array, (count + 2) * sizeof(char *));
    if (!new_array)
    {
      free_string_array(array);
      free(tmp);
      return NULL;
    }

    array = new_array;

    array[count] = strdup(tok);
    if (!array[count])
    {
      free_string_array(array);
      free(tmp);
      return NULL;
    }

    count++;
    tok = strtok(NULL, ",");
  }

  if (array)
    array[count] = NULL;

  free(tmp);
  return array;
}

// 1. Function that assigns defaults
PostgresMetricsConfig init_config()
{
  PostgresMetricsConfig cfg = {0};

  int n = 0;
  while (default_metrics[n])
    n++; // Number of elements

  cfg.global_metrics = malloc((n + 1) * sizeof(char *)); // +1 for null termination
  if (!cfg.global_metrics)
    return cfg;

  for (int i = 0; i < n; i++)
    cfg.global_metrics[i] = strdup(default_metrics[i]);

  cfg.global_metrics[n] = NULL; // null terminate

  return cfg;
}

// 2. Function that reads environment variables
static void load_env_config(PostgresMetricsConfig *cfg)
{
  // TODO env'den gelen değerler validate edilip ilgili hatalar dönmeli
  //  cli metriklerinin hepsinin karşılığı burada da olmalı.
  const char *env_dsn = getenv("PG_STAT_EXPORTER_DSN");
  if (env_dsn)
    cfg->dsn = env_dsn;

  const char *env_global_metrics = getenv("PG_STAT_EXPORTER_GLOBAL_METRICS");
  if (env_global_metrics)
  {
    if (cfg->global_metrics)
      free_string_array(cfg->global_metrics);

    cfg->global_metrics = csv_to_array(env_global_metrics);
  }

  const char *env_database_metrics = getenv("PG_STAT_EXPORTER_DATABASE_METRICS");
  if (env_database_metrics)
  {
    if (cfg->database_metrics)
      free_string_array(cfg->database_metrics);

    cfg->database_metrics = csv_to_array(env_database_metrics);
  }

  const char *env_excluded_databases = getenv("PG_STAT_EXPORTER_EXCLUDED_DATABASES");
  if (env_excluded_databases)
  {
    if (cfg->excluded_databases)
      free(cfg->excluded_databases);

    cfg->excluded_databases = strdup(env_excluded_databases);
  }

  const char *env_http_server = getenv("PG_STAT_EXPORTER_HTTP_SERVER");
  if (env_http_server)
    cfg->use_http_server = (strcasecmp(env_http_server, "true") == 0) ? 1 : 0;

  const char *env_server_options = getenv("PG_STAT_EXPORTER_HTTP_SERVER_OPTIONS");
  if (env_server_options)
  {
    if (cfg->http_server_options)
      free_string_array(cfg->http_server_options);

    cfg->http_server_options = csv_to_array(env_server_options);
  }
}

// 3. Function that reads CLI parameters
static void load_cli_config(int argc, char **argv, PostgresMetricsConfig *cfg)
{
  for (int i = 1; i < argc; i++)
  {
    if (strncmp(argv[i], "--dsn=", 6) == 0)
    {
      cfg->dsn = argv[i] + 6;
    }
    else if (strncmp(argv[i], "--global-metrics=", 17) == 0)
    {
      char *tmp = strdup(argv[i] + 17);
      if (tmp)
      {
        if (cfg->global_metrics)
        {
          free_string_array(cfg->global_metrics);
          cfg->global_metrics = NULL;
        }
        cfg->global_metrics = csv_to_array(tmp);
        free(tmp);
      }
      else
      {
        fprintf(stderr, "Warning: Could't parse --global-metrics using defaults\n");
      }
    }
    else if (strncmp(argv[i], "--database-metrics=", 19) == 0)
    {
      char *tmp = strdup(argv[i] + 19);
      if (tmp)
      {
        if (cfg->database_metrics)
        {
          free_string_array(cfg->database_metrics);
          cfg->database_metrics = NULL;
        }
        cfg->database_metrics = csv_to_array(tmp);
        free(tmp);
      }
      else
      {
        fprintf(stderr, "Warning: Could't parse --database-metrics\n");
      }
    }
    else if (strncmp(argv[i], "--excluded-databases=", 21) == 0)
    {
      char *tmp = strdup(argv[i] + 21); 
      if (tmp)
      {
        if (cfg->excluded_databases)
        {
          free(cfg->excluded_databases);
          cfg->excluded_databases = NULL;
        }

        cfg->excluded_databases = tmp; 
      }
      else
      {
        fprintf(stderr, "Warning: Couldn't parse --excluded-databases\n");
      }
    }
    else if (strcmp(argv[i], "--http-server") == 0)
    {
      cfg->use_http_server = 1;
    }
    else if (strncmp(argv[i], "--http-server-options=", 22) == 0)
    {
      if (cfg->http_server_options)
      {
        free_string_array(cfg->http_server_options);
        cfg->http_server_options = NULL;
      }

      char *tmp = strdup(argv[i] + 22);
      cfg->http_server_options = csv_to_array(tmp);
      free(tmp);
    }
  }
}

// 4. Main function: init + env + CLI
PostgresMetricsConfig *load_config(int argc, char **argv)
{
  PostgresMetricsConfig *cfg = malloc(sizeof(PostgresMetricsConfig));
  if (!cfg)
    return NULL;

  *cfg = init_config();
  load_env_config(cfg);
  load_cli_config(argc, argv, cfg);
  cfg->global_metrics_count = array_count((const char **)cfg->global_metrics);
  cfg->database_metrics_count = array_count((const char **)cfg->database_metrics);

  if (!cfg->dsn || strlen(cfg->dsn) == 0)
  {
    free(cfg);
    fprintf(stderr, "Error: DSN not set via --dsn or PG_STAT_EXPORTER_DSN\n");
    return NULL; // main will return 500
  }

  if (!cfg->global_metrics || !cfg->global_metrics[0])
  {
    free(cfg);
    fprintf(stderr, "Error: Metric List not set via --global-metrics or PG_STAT_EXPORTER_GLOBAL_METRICS\n");
    return NULL;
  }

  if (cfg->use_http_server == 1)
  {
    if (!cfg->http_server_options || !cfg->http_server_options[0])
    {
      free(cfg);
      fprintf(stderr, "Error: Server Options not set via --http-server-options OR PG_STAT_EXPORTER_HTTP_SERVER_OPTIONS\n");
      return NULL;
    }
  }

  return cfg;
}
