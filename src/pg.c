#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <postgresql/libpq-fe.h>

#include "util.h"
#include "pg.h"
#include "config.h"
#include "errors.h"

// Connects to the PostgreSQL database using the DSN from config
PGconn *pg_connect_db(const char *dsn)
{
  PGconn *conn = PQconnectdb(dsn);

  if (PQstatus(conn) != CONNECTION_OK)
  {
    fprintf(stderr, "Error: Connection to database failed: %s\n", PQerrorMessage(conn));
    PQfinish(conn);
    return NULL;
  }

  return conn;
}

ErrorCode pg_get_major_version(PGconn *conn, int *postgres_version)
{
  if (!conn || !postgres_version)
    return ERR_INVALID_ARG;

  PGresult *res = PQexec(conn, "SHOW server_version_num;");
  if (!res)
    return PG_DB_ERROR; // connection problem?

  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    fprintf(stderr, "Error: Failed to get PostgreSQL version: %s\n",
            PQerrorMessage(conn));
    PQclear(res);
    return PG_DB_ERROR;
  }

  const char *ver_str = PQgetvalue(res, 0, 0);
  if (!ver_str)
  {
    PQclear(res);
    return PG_DB_ERROR;
  }

  int ver_num = atoi(ver_str);
  if (ver_num <= 0)
  {
    PQclear(res);
    return PG_DB_ERROR;
  }

  *postgres_version = ver_num / 10000; // major version
  PQclear(res);
  return ERR_OK;
}

ErrorCode pg_build_conninfo_for_database(PGconn *conn,
                                         const char *dbname,
                                         char **conninfo_out)
{
  if (!conn || !dbname || !conninfo_out)
    return ERR_INVALID_ARG;

  if (*conninfo_out != NULL)
    return ERR_INVALID_ARG;

  PQconninfoOption *opts = PQconninfo(conn);
  if (!opts)
    return PG_DB_ERROR;

  size_t len = 1; /* '\0' */

  for (PQconninfoOption *o = opts; o && o->keyword; o++)
  {
    const char *value =
        (strcmp(o->keyword, "dbname") == 0)
            ? dbname
            : o->val;

    if (!value || !*value)
      continue;

    len += strlen(o->keyword);
    len += strlen(value) * 2; /* worst-case escaping */
    len += 8;
  }

  char *conninfo = malloc(len);
  if (!conninfo)
  {
    PQconninfoFree(opts);
    return ERR_NO_MEMORY;
  }

  char *p = conninfo;

  for (PQconninfoOption *o = opts; o && o->keyword; o++)
  {
    const char *value =
        (strcmp(o->keyword, "dbname") == 0)
            ? dbname
            : o->val;

    if (!value || !*value)
      continue;

    p += sprintf(p, "%s='", o->keyword);

    for (const char *v = value; *v; v++)
    {
      if (*v == '\'' || *v == '\\')
        *p++ = '\\';

      *p++ = *v;
    }

    *p++ = '\'';
    *p++ = ' ';
  }

  *p = '\0';

  PQconninfoFree(opts);

  *conninfo_out = conninfo;

  return ERR_OK;
}

ErrorCode pg_get_included_databases(PGconn *conn,
                                    const char *excluded_databases,
                                    char ***included_databases,
                                    int *included_database_count,
                                    char ***included_database_dsn)
{

  if (!conn)
    return ERR_INVALID_ARG;

  if (!included_databases || *included_databases != NULL)
  {
    fprintf(stderr, "Error: included_databases pointer is NULL or not empty\n");
    return ERR_INVALID_ARG;
  }

  if (!included_database_count || *included_database_count != 0)
  {
    fprintf(stderr, "Error: included_database_count pointer is NULL or not zero\n");
    return ERR_INVALID_ARG;
  }

  if (!included_database_dsn || *included_database_dsn != NULL)
  {
    fprintf(stderr, "Error: included_database_dsn pointer is NULL or not empty\n");
    return ERR_INVALID_ARG;
  }

  const char *sql =
      "SELECT "
      "    d.datname "
      "  FROM pg_database d "
      "  WHERE "
      "    d.datallowconn AND NOT d.datistemplate AND "
      "    has_database_privilege(d.datname, 'CONNECT') AND "
      "    datname <> current_database() AND "
      "    datname <> ALL(string_to_array($1, ',')::name[]) "
      "    ORDER BY 1;";

  const char *paramValues[1] = {excluded_databases ? excluded_databases : ""};

  PGresult *res = PQexecParams(conn,
                               sql,
                               1,    /* one param */
                               NULL, /* let the backend deduce param type */
                               paramValues,
                               NULL, /* don't need param lengths since text */
                               NULL, /* default to all text params */
                               0);   /* ask for text results */

  if (!res)
    return PG_DB_ERROR;

  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    fprintf(stderr, "Error: Failed to query databases: %s\n", PQerrorMessage(conn));
    PQclear(res);
    return PG_DB_ERROR;
  }

  int nrows = PQntuples(res);
  if (nrows == 0)
  {
    PQclear(res);
    return ERR_OK; // no databases found
  }

  char **db_list = calloc(nrows + 1, sizeof(char *)); // +1 for NULL terminator
  char **dsn_list = calloc(nrows + 1, sizeof(char *));

  if (!db_list || !dsn_list)
  {
    free(db_list);
    free(dsn_list);
    PQclear(res);
    return ERR_NO_MEMORY;
  }

  int valid_count = 0;

  for (int i = 0; i < nrows; i++)
  {
    const char *dbname = PQgetvalue(res, i, 0);

    char *dsn_database = NULL;

    ErrorCode rc =
        pg_build_conninfo_for_database(conn,
                                       dbname,
                                       &dsn_database);

    if (rc != ERR_OK)
    {
      free_string_array(db_list);
      free_string_array(dsn_list);
      PQclear(res);
      return rc;
    }

    PGconn *test_conn = PQconnectdb(dsn_database);

    if (!test_conn)
    {
      free(dsn_database);
      free_string_array(db_list);
      free_string_array(dsn_list);
      PQclear(res);
      return PG_DB_ERROR;
    }

    if (PQstatus(test_conn) == CONNECTION_OK)
    {
      db_list[valid_count] = strdup(dbname);
      if (!db_list[valid_count])
      {
        PQfinish(test_conn);
        free(dsn_database);

        free_string_array(db_list);
        free_string_array(dsn_list);
        PQclear(res);
        return ERR_NO_MEMORY;
      }
      
      fprintf(stderr, "Info: Adding database: '%s'\n", dbname);

      dsn_list[valid_count] = dsn_database;
      valid_count++;
    }
    else
    {
      fprintf(stderr,
              "Skipping database '%s': %s\n",
              dbname,
              PQerrorMessage(test_conn));

      free(dsn_database);
    }

    PQfinish(test_conn);
  }

  // NULL-terminate the array
  db_list[valid_count] = NULL;
  dsn_list[valid_count] = NULL;

  *included_databases = db_list;
  *included_database_dsn = dsn_list;
  *included_database_count = valid_count;

  PQclear(res);

  return ERR_OK;
}

ErrorCode pg_check_postgres_config(PostgresMetricsConfig *cfg)
{
  if (!cfg)
    return ERR_INVALID_ARG;

  PGconn *conn = pg_connect_db(cfg->dsn);
  if (!conn)
    return PG_DB_ERROR; // connection error

  ErrorCode err = pg_get_major_version(conn, &cfg->postgres_version);
  if (err != ERR_OK)
  {
    fprintf(stderr, "Error: Failed to get PostgreSQL version\n");
    PQfinish(conn);
    return err;
  }
  fprintf(stderr, "Info: Connected PostgreSQL. Major version is %d\n", cfg->postgres_version);

  // Check if the version is supported
  int supported = 0;
  for (int i = 0; i < SUPPORTED_PG_VERSIONS_COUNT; i++)
  {
    if (cfg->postgres_version == SUPPORTED_PG_VERSIONS[i])
    {
      supported = 1;
      break;
    }
  }

  if (!supported)
  {
    fprintf(stderr, "Error: Unsupported PostgreSQL version: %d\n", cfg->postgres_version);
    PQfinish(conn);
    return PG_DB_ERROR;
  }

  err = pg_get_included_databases(conn,
                                  cfg->excluded_databases,
                                  &cfg->included_databases,
                                  &cfg->included_database_count,
                                  &cfg->included_database_dsn);
  if (err != ERR_OK)
  {
    fprintf(stderr, "Error: Failed to get database list\n");
    PQfinish(conn);
    return err;
  }

  PQfinish(conn);

  return ERR_OK; // success
}

// Executes the given SQL and returns the first column of the first row as a string
char *exec_sql_return_string(PGconn *conn, const char *sql)
{
  PGresult *res = PQexec(conn, sql);
  if (PQresultStatus(res) != PGRES_TUPLES_OK)
  {
    fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
    PQclear(res);
    return NULL;
  }

  if (PQntuples(res) < 1)
  {
    PQclear(res);
    return NULL;
  }

  char *value = PQgetvalue(res, 0, 0);

  if (value[0] == '\0')
  {
    PQclear(res);
    return NULL;
  }

  char *result = malloc(strlen(value) + 1);
  if (!result)
  {
    PQclear(res);
    return NULL;
  }
  strcpy(result, value);

  PQclear(res);
  return result;
}
