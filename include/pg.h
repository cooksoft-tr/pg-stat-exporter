#ifndef PG_H
#define PG_H

#include <postgresql/libpq-fe.h>
#include "config.h"
#include "util.h"
#include "errors.h"

// Opens a connection to PostgreSQL using the given configuration
PGconn *pg_connect_db(const char *dsn);

// Retrieves the major version number of the connected PostgreSQL server
ErrorCode pg_get_major_version(PGconn *conn, int *postgres_version);

// Retrieves the list of included databases from PostgreSQL.
ErrorCode pg_get_included_databases(PGconn *conn,
                                    const char *excluded_databases,
                                    char ***included_databases,
                                    int *included_database_count,
                                    char ***included_database_dsn);

// Gets the major version in the config and checks if it's supported
// Updates PostgresMetricsConfig->included_databases
ErrorCode pg_check_postgres_config(PostgresMetricsConfig *cfg);

/* Converts all rows in a PGresult to db_table_t */
db_table_t *pgresult_to_db_table(PGresult *res);

/* Executes an SQL query and returns the value of the first cell as a string */
char *exec_sql_return_string(PGconn *conn, const char *sql);

#endif