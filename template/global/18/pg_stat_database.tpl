{{# . }}
# HELP pg_stat_database_numbackends Number of backends connected to this database
# TYPE pg_stat_database_numbackends gauge
pg_stat_database_numbackends{datname="{{ datname }}"} {{ numbackends }}

# HELP pg_stat_database_xact_commit_count Committed transactions
# TYPE pg_stat_database_xact_commit_count counter
pg_stat_database_xact_commit_count{datname="{{ datname }}"} {{ xact_commit }}

# HELP pg_stat_database_xact_rollback_count Rolled back transactions
# TYPE pg_stat_database_xact_rollback_count counter
pg_stat_database_xact_rollback_count{datname="{{ datname }}"} {{ xact_rollback }}

# HELP pg_stat_database_blks_read_count Disk blocks read
# TYPE pg_stat_database_blks_read_count counter
pg_stat_database_blks_read_count{datname="{{ datname }}"} {{ blks_read }}

# HELP pg_stat_database_blks_hit_count Buffer hits
# TYPE pg_stat_database_blks_hit_count counter
pg_stat_database_blks_hit_count{datname="{{ datname }}"} {{ blks_hit }}

# HELP pg_stat_database_tup_returned_count Rows returned by sequential/index scans
# TYPE pg_stat_database_tup_returned_count counter
pg_stat_database_tup_returned_count{datname="{{ datname }}"} {{ tup_returned }}

# HELP pg_stat_database_tup_fetched_count Rows fetched by index scans
# TYPE pg_stat_database_tup_fetched_count counter
pg_stat_database_tup_fetched_count{datname="{{ datname }}"} {{ tup_fetched }}

# HELP pg_stat_database_tup_inserted_count Rows inserted
# TYPE pg_stat_database_tup_inserted_count counter
pg_stat_database_tup_inserted_count{datname="{{ datname }}"} {{ tup_inserted }}

# HELP pg_stat_database_tup_updated_count Rows updated
# TYPE pg_stat_database_tup_updated_count counter
pg_stat_database_tup_updated_count{datname="{{ datname }}"} {{ tup_updated }}

# HELP pg_stat_database_tup_deleted_count Rows deleted
# TYPE pg_stat_database_tup_deleted_count counter
pg_stat_database_tup_deleted_count{datname="{{ datname }}"} {{ tup_deleted }}

# HELP pg_stat_database_conflicts_count Recovery conflicts
# TYPE pg_stat_database_conflicts_count counter
pg_stat_database_conflicts_count{datname="{{ datname }}"} {{ conflicts }}

# HELP pg_stat_database_temp_files_total Temporary files created
# TYPE pg_stat_database_temp_files_total counter
pg_stat_database_temp_files_total{datname="{{ datname }}"} {{ temp_files }}

# HELP pg_stat_database_temp_bytes_total Size of temporary files created
# TYPE pg_stat_database_temp_bytes_total counter
pg_stat_database_temp_bytes_total{datname="{{ datname }}"} {{ temp_bytes }}

# HELP pg_stat_database_deadlocks_count Deadlocks detected
# TYPE pg_stat_database_deadlocks_count counter
pg_stat_database_deadlocks_count{datname="{{ datname }}"} {{ deadlocks }}

# HELP pg_stat_database_blk_read_time_total Time spent reading data file blocks, in milliseconds
# TYPE pg_stat_database_blk_read_time_total counter
pg_stat_database_blk_read_time_total{datname="{{ datname }}"} {{ blk_read_time }}

# HELP pg_stat_database_blk_write_time_total Time spent writing data file blocks, in milliseconds
# TYPE pg_stat_database_blk_write_time_total counter
pg_stat_database_blk_write_time_total{datname="{{ datname }}"} {{ blk_write_time }}

# HELP pg_stat_database_session_time_seconds_total Total session time spent in this database, in seconds
# TYPE pg_stat_database_session_time_seconds_total counter
pg_stat_database_session_time_seconds_total{datname="{{ datname }}"} {{ session_time_seconds }}

# HELP pg_stat_database_active_time_seconds_total Time spent executing queries in this database, in seconds
# TYPE pg_stat_database_active_time_seconds_total counter
pg_stat_database_active_time_seconds_total{datname="{{ datname }}"} {{ active_time_seconds }}

# HELP pg_stat_database_idle_in_xact_time_seconds_total Time spent idle in transaction in this database, in seconds
# TYPE pg_stat_database_idle_in_xact_time_seconds_total counter
pg_stat_database_idle_in_xact_time_seconds_total{datname="{{ datname }}"} {{ idle_in_transaction_time_seconds }}

# HELP pg_stat_database_sessions_abandoned_count Sessions ended due to client disconnect
# TYPE pg_stat_database_sessions_abandoned_count counter
pg_stat_database_sessions_abandoned_count{datname="{{ datname }}"} {{ sessions_abandoned }}

# HELP pg_stat_database_sessions_fatal_count Sessions terminated by fatal errors
# TYPE pg_stat_database_sessions_fatal_count counter
pg_stat_database_sessions_fatal_count{datname="{{ datname }}"} {{ sessions_fatal }}

# HELP pg_stat_database_sessions_killed_count Sessions terminated by admin command
# TYPE pg_stat_database_sessions_killed_count counter
pg_stat_database_sessions_killed_count{datname="{{ datname }}"} {{ sessions_killed }}

{{/ . }}
