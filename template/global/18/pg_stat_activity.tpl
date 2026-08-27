{{# . }}
# HELP pg_stat_activity_total Total number of client backends
# TYPE pg_stat_activity_total gauge
pg_stat_activity_total {{ total }}

# HELP pg_stat_activity_active Active client backends
# TYPE pg_stat_activity_active gauge
pg_stat_activity_active {{ active }}

# HELP pg_stat_activity_idle Idle client backends
# TYPE pg_stat_activity_idle gauge
pg_stat_activity_idle {{ idle }}

# HELP pg_stat_activity_idle_in_transaction Idle in transaction client backends
# TYPE pg_stat_activity_idle_in_transaction gauge
pg_stat_activity_idle_in_transaction {{ idle_in_transaction }}

# HELP pg_stat_activity_idle_in_transaction_aborted Idle in transaction aborted
# TYPE pg_stat_activity_idle_in_transaction_aborted gauge
pg_stat_activity_idle_in_transaction_aborted {{ idle_in_transaction_aborted }}

# HELP pg_stat_activity_waiting Number of backends waiting on locks
# TYPE pg_stat_activity_waiting gauge
pg_stat_activity_waiting {{ waiting }}

# HELP pg_stat_activity_autovacuum_workers Number of autovacuum workers
# TYPE pg_stat_activity_autovacuum_workers gauge
pg_stat_activity_autovacuum_workers {{ autovacuum_workers }}

# HELP pg_stat_activity_logical_replication_workers Number of logical replication workers
# TYPE pg_stat_activity_logical_replication_workers gauge
pg_stat_activity_logical_replication_workers {{ logical_replication_workers }}

# HELP pg_stat_activity_parallel_workers Number of parallel workers
# TYPE pg_stat_activity_parallel_workers gauge
pg_stat_activity_parallel_workers {{ parallel_workers }}

# HELP pg_stat_activity_wal_sender Number of Wal senders
# TYPE pg_stat_activity_wal_sender gauge
pg_stat_activity_wal_sender {{ wal_sender }}
{{/ . }}
