#include <stdlib.h>
#include <string.h>

#include "path_util.h"
#include "template.h"
#include "errors.h"

// Default directory for templates
#ifndef TEMPLATE_DIR
#define TEMPLATE_DIR "template"
#endif

// Default directory for SQL files
#ifndef SQL_DIR
#define SQL_DIR "sql"
#endif

// Directory for custom templates
#ifndef CUSTOM_TEMPLATE_DIR
#define CUSTOM_TEMPLATE_DIR "custom-template"
#endif

// Directory for custom SQL files
#ifndef CUSTOM_SQL_DIR
#define CUSTOM_SQL_DIR "custom-sql"
#endif

// Subdirectory name for global metrics
#ifndef GLOBAL_DIR_NAME
#define GLOBAL_DIR_NAME "global"
#endif

// Subdirectory name for database-related metrics
#ifndef DATABASE_DIR_NAME
#define DATABASE_DIR_NAME "database"
#endif

#ifndef SQL_FILE_SUFFIX
#define SQL_FILE_SUFFIX ".sql"
#endif

// Subdirectory name for database-related metrics
#ifndef TEMPLATE_FILE_SUFFIX
#define TEMPLATE_FILE_SUFFIX ".tpl"
#endif

// Loads SQL and template buffers for each metric in the config
ErrorCode pg_load_buffers(PostgresMetricsConfig *cfg,
                          char *sql_buffers_global[],
                          char *tpl_buffers_global[],
                          char *sql_buffers_database[],
                          char *tpl_buffers_database[])
{
  for (int i = 0; i < cfg->global_metrics_count; i++)
  {

    ErrorCode error_code = load_file_to_buffer(
        CUSTOM_SQL_DIR,
        SQL_DIR,
        GLOBAL_DIR_NAME,
        cfg->postgres_version,
        cfg->global_metrics[i],
        ".sql",
        &sql_buffers_global[i]);

    if (error_code != ERR_OK)
    {
      return error_code;
    }

    error_code = load_file_to_buffer(
        CUSTOM_TEMPLATE_DIR,
        TEMPLATE_DIR,
        GLOBAL_DIR_NAME,
        cfg->postgres_version,
        cfg->global_metrics[i],
        ".tpl",
        &tpl_buffers_global[i]);

    if (error_code != ERR_OK)
    {
      return error_code;
    }
  }

  for (int i = 0; i < cfg->database_metrics_count; i++)
  {
    ErrorCode error_code = load_file_to_buffer(
        CUSTOM_SQL_DIR,
        SQL_DIR,
        DATABASE_DIR_NAME,
        cfg->postgres_version,
        cfg->database_metrics[i],
        ".sql",
        &sql_buffers_database[i]);

    if (error_code != ERR_OK)
    {
      return error_code;
    }

    error_code = load_file_to_buffer(
        CUSTOM_TEMPLATE_DIR,
        TEMPLATE_DIR,
        DATABASE_DIR_NAME,
        cfg->postgres_version,
        cfg->database_metrics[i],
        ".tpl",
        &tpl_buffers_database[i]);

    if (error_code != ERR_OK)
    {
      return error_code;
    }
  }
  return ERR_OK;
}
