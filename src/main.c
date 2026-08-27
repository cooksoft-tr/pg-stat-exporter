#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <json-c/json.h>
#include <mustach/mustach-json-c.h>
#include <postgresql/libpq-fe.h>
#include <systemd/sd-daemon.h>

#include "civetweb.h"

#include "config.h"
#include "errors.h"
#include "help.h"
#include "path_util.h"
#include "pg.h"
#include "template.h"

typedef struct
{
  PostgresMetricsConfig *cfg;
  char **sql_buffers_global;
  char **tpl_buffers_global;
  char **sql_buffers_database;
  char **tpl_buffers_database;
} MgContext;

static volatile sig_atomic_t keep_running = 1;

static void handle_shutdown_signal(int signum)
{
  (void)signum;
  keep_running = 0;
}

static int render_template_to_string(const char *tpl_buffer,
                                     const char *json,
                                     char **rendered,
                                     size_t *rendered_length)
{
  json_object *jobj = json_tokener_parse(json);
  int result;

  if (!jobj)
  {
    fprintf(stderr, "Error: Failed to parse JSON payload\n");
    return -1;
  }

  result = mustach_json_c_mem(tpl_buffer, 0, jobj, 0, rendered, rendered_length);
  json_object_put(jobj);

  return result;
}

static void stream_metric_buffers(PGconn *pg_conn,
                                  int metric_count,
                                  char **sql_buffers,
                                  char **tpl_buffers,
                                  struct mg_connection *mg_conn)
{
  for (int i = 0; i < metric_count; i++)
  {
    char *json = exec_sql_return_string(pg_conn, sql_buffers[i]);
    if (!json)
      continue;

    char *rendered = NULL;
    size_t rendered_length = 0;
    int result = render_template_to_string(tpl_buffers[i], json, &rendered, &rendered_length);
    free(json);

    if (result == 0 && rendered)
    {
      mg_send_chunk(mg_conn, rendered, (long long)rendered_length);
      free(rendered);
    }
  }
}

static void print_metric_buffers(PGconn *pg_conn,
                                 int metric_count,
                                 char **sql_buffers,
                                 char **tpl_buffers)
{
  for (int i = 0; i < metric_count; i++)
  {
    char *json = exec_sql_return_string(pg_conn, sql_buffers[i]);
    if (!json)
      continue;

    char *rendered = NULL;
    size_t rendered_length = 0;
    int result = render_template_to_string(tpl_buffers[i], json, &rendered, &rendered_length);
    free(json);

    if (result == 0 && rendered)
    {
      fwrite(rendered, 1, rendered_length, stdout);
      free(rendered);
    }
  }
}

static int metrics_handler(struct mg_connection *mg_conn, void *ignored)
{
  MgContext *ctx = (MgContext *)mg_get_user_context_data(mg_conn);
  PostgresMetricsConfig *cfg = ctx->cfg;
  PGconn *pg_conn_global = pg_connect_db(cfg->dsn);

  (void)ignored;

  if (!pg_conn_global)
    return 1;

  mg_send_http_ok(mg_conn, "text/plain; version=0.0.4", -1);

  stream_metric_buffers(pg_conn_global,
                        cfg->global_metrics_count,
                        ctx->sql_buffers_global,
                        ctx->tpl_buffers_global,
                        mg_conn);

  if (cfg->database_metrics_count > 0)
  {
    stream_metric_buffers(pg_conn_global,
                          cfg->database_metrics_count,
                          ctx->sql_buffers_database,
                          ctx->tpl_buffers_database,
                          mg_conn);
  }

  PQfinish(pg_conn_global);

  if (cfg->database_metrics_count > 0 && cfg->included_database_count > 0)
  {

    for (int i = 0; i < cfg->included_database_count; i++)
    {
      const char *dsn_database = cfg->included_database_dsn[i];
      PGconn *pg_conn_database = pg_connect_db(dsn_database);

      if (!pg_conn_database)
        continue;

      stream_metric_buffers(pg_conn_database,
                            cfg->database_metrics_count,
                            ctx->sql_buffers_database,
                            ctx->tpl_buffers_database,
                            mg_conn);

      PQfinish(pg_conn_database);
    }
  }

  mg_send_chunk(mg_conn, 0, 0);

  return 200;
}


static int mg_log_message(const struct mg_connection *conn, const char *msg)
{
    (void)conn;
    fprintf(stderr, "CivetWeb: %s\n", msg);
    return 0;
}

int main(int argc, char **argv)
{
  if (argc == 1)
  {
    print_help(argv[0]);
    return 0;
  }
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      print_help();
      return 0;
    }
  }

  PostgresMetricsConfig *cfg = load_config(argc, argv);
  if (!cfg)
  {
    fprintf(stderr, "Error: Configuration error\n");
    return ERR_CONFIG;
  }

  ErrorCode err = pg_check_postgres_config(cfg);
  if (err != ERR_OK)
    return err;

  char **sql_buffers_global = calloc(cfg->global_metrics_count + 1, sizeof(char *));
  char **tpl_buffers_global = calloc(cfg->global_metrics_count + 1, sizeof(char *));
  char **sql_buffers_database = calloc(cfg->database_metrics_count + 1, sizeof(char *));
  char **tpl_buffers_database = calloc(cfg->database_metrics_count + 1, sizeof(char *));

  if (!sql_buffers_global || !tpl_buffers_global || !sql_buffers_database || !tpl_buffers_database)
  {
    fprintf(stderr, "Error: Failed to allocate metric buffers\n");
    return ERR_NO_MEMORY;
  }

  err = pg_load_buffers(cfg,
                        sql_buffers_global,
                        tpl_buffers_global,
                        sql_buffers_database,
                        tpl_buffers_database);
  if (err != ERR_OK)
    return err;

  if (cfg->use_http_server == 1)
  {
    static MgContext my_ctx;
    my_ctx.cfg = cfg;
    my_ctx.sql_buffers_global = sql_buffers_global;
    my_ctx.tpl_buffers_global = tpl_buffers_global;
    my_ctx.sql_buffers_database = sql_buffers_database;
    my_ctx.tpl_buffers_database = tpl_buffers_database;

    struct mg_callbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.log_message = mg_log_message;

    mg_init_library(0);

    struct mg_context *ctx = mg_start(&callbacks, &my_ctx, (const char **)cfg->http_server_options);
    if (!ctx)
    {
      fprintf(stderr, "Error: Failed to start HTTP server\n");
      mg_exit_library();
      return ERR_CONFIG;
    }

    mg_set_request_handler(ctx, "/metrics", metrics_handler, 0);
    sd_notify(0, "READY=1");

    signal(SIGINT, handle_shutdown_signal);
    signal(SIGTERM, handle_shutdown_signal);

    while (keep_running)
      pause();

    /* Stop the use_http_server */
    mg_stop(ctx);

    /* Un-initialize the library */
    mg_exit_library();
  }
  else
  {
    PGconn *pg_conn = pg_connect_db(cfg->dsn);
    if (!pg_conn)
      return PG_DB_ERROR;

    print_metric_buffers(pg_conn,
                         cfg->global_metrics_count,
                         sql_buffers_global,
                         tpl_buffers_global);
    print_metric_buffers(pg_conn,
                         cfg->database_metrics_count,
                         sql_buffers_database,
                         tpl_buffers_database);

    PQfinish(pg_conn);
  }

  free_string_array(sql_buffers_global);
  free_string_array(tpl_buffers_global);
  free_string_array(sql_buffers_database);
  free_string_array(tpl_buffers_database);

  return ERR_OK;
}
