#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stdio.h>
#include "config.h"
#include "errors.h"

ErrorCode pg_load_buffers(PostgresMetricsConfig *cfg,
                          char *sql_buffers_global[],
                          char *tpl_buffers_global[],
                          char *sql_buffers_database[],
                          char *tpl_buffers_database[]);

#endif
