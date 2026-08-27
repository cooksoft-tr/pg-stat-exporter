#ifndef PATH_UTIL_H
#define PATH_UTIL_H

#include "config.h"
#include "errors.h"

/* path string’ini heap’e kopyalar ve pointer döner.
   Caller free ile temizlemeli. */
char *path_to_buffer(const char *path);

char *load_template_to_buffer(const PostgresMetricsConfig *cfg, const char *metric_name);

char *load_sql_to_buffer(const PostgresMetricsConfig *cfg, const char *metric_name);

ErrorCode load_file_to_buffer(const char *custom_base_dir,
                              const char *base_dir,
                              const char *subdir,
                              int postgres_version,
                              const char *metric_name,
                              const char *ext,
                              char **out_buf);

#endif
