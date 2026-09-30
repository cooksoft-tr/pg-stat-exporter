# HELP pg_stat_user_tables_seq_scan Number of sequential scans initiated on this table
# TYPE pg_stat_user_tables_seq_scan counter

{{# .}}
pg_stat_user_tables_seq_scan{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ seq_scan }}

# HELP pg_stat_user_tables_seq_tup_read Number of live rows fetched by sequential scans
# TYPE pg_stat_user_tables_seq_tup_read counter
pg_stat_user_tables_seq_tup_read{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ seq_tup_read }}

# HELP pg_stat_user_tables_idx_scan Number of index scans initiated on this table
# TYPE pg_stat_user_tables_idx_scan counter
pg_stat_user_tables_idx_scan{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ idx_scan }}

# HELP pg_stat_user_tables_idx_tup_fetch Number of live rows fetched by index scans
# TYPE pg_stat_user_tables_idx_tup_fetch counter
pg_stat_user_tables_idx_tup_fetch{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ idx_tup_fetch }}

# HELP pg_stat_user_tables_n_tup_ins Number of rows inserted
# TYPE pg_stat_user_tables_n_tup_ins counter
pg_stat_user_tables_n_tup_ins{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ n_tup_ins }}

# HELP pg_stat_user_tables_n_tup_upd Number of rows updated
# TYPE pg_stat_user_tables_n_tup_upd counter
pg_stat_user_tables_n_tup_upd{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ n_tup_upd }}

# HELP pg_stat_user_tables_n_tup_del Number of rows deleted
# TYPE pg_stat_user_tables_n_tup_del counter
pg_stat_user_tables_n_tup_del{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ n_tup_del }}

# HELP pg_stat_user_tables_n_tup_hot_upd Number of rows HOT updated (i.e., with no separate index update required)
# TYPE pg_stat_user_tables_n_tup_hot_upd counter
pg_stat_user_tables_n_tup_hot_upd{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ n_tup_hot_upd }}

# HELP pg_stat_user_tables_n_live_tup Estimated number of live rows
# TYPE pg_stat_user_tables_n_live_tup gauge
pg_stat_user_tables_n_live_tup{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ n_live_tup }}

# HELP pg_stat_user_tables_n_dead_tup Estimated number of dead rows
# TYPE pg_stat_user_tables_n_dead_tup gauge
pg_stat_user_tables_n_dead_tup{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ n_dead_tup }}

# HELP pg_stat_user_tables_n_mod_since_analyze Estimated number of rows changed since last analyze
# TYPE pg_stat_user_tables_n_mod_since_analyze gauge
pg_stat_user_tables_n_mod_since_analyze{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ n_mod_since_analyze }}

# HELP pg_stat_user_tables_last_vacuum Last time at which this table was manually vacuumed (not counting VACUUM FULL)
# TYPE pg_stat_user_tables_last_vacuum gauge
pg_stat_user_tables_last_vacuum{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ last_vacuum }}

# HELP pg_stat_user_tables_last_autovacuum Last time at which this table was vacuumed by the autovacuum daemon
# TYPE pg_stat_user_tables_last_autovacuum gauge
pg_stat_user_tables_last_autovacuum{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ last_autovacuum }}

# HELP pg_stat_user_tables_last_analyze Last time at which this table was manually analyzed
# TYPE pg_stat_user_tables_last_analyze gauge
pg_stat_user_tables_last_analyze{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ last_analyze }}

# HELP pg_stat_user_tables_last_autoanalyze Last time at which this table was analyzed by the autovacuum daemon
# TYPE pg_stat_user_tables_last_autoanalyze gauge
pg_stat_user_tables_last_autoanalyze{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ last_autoanalyze }}

# HELP pg_stat_user_tables_vacuum_count Number of times this table has been manually vacuumed (not counting VACUUM FULL)
# TYPE pg_stat_user_tables_vacuum_count counter
pg_stat_user_tables_vacuum_count{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ vacuum_count }}

# HELP pg_stat_user_tables_autovacuum_count Number of times this table has been vacuumed by the autovacuum daemon
# TYPE pg_stat_user_tables_autovacuum_count counter
pg_stat_user_tables_autovacuum_count{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ autovacuum_count }}

# HELP pg_stat_user_tables_analyze_count Number of times this table has been manually analyzed
# TYPE pg_stat_user_tables_analyze_count counter
pg_stat_user_tables_analyze_count{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ analyze_count }}

# HELP pg_stat_user_tables_autoanalyze_count Number of times this table has been analyzed by the autovacuum daemon
# TYPE pg_stat_user_tables_autoanalyze_count counter
pg_stat_user_tables_autoanalyze_count{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ autoanalyze_count }}

# HELP pg_stat_user_tables_index_size_bytes Total disk space used by this index, in bytes
# TYPE pg_stat_user_tables_index_size_bytes gauge
pg_stat_user_tables_index_size_bytes{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ index_size_bytes }}

# HELP pg_stat_user_tables_table_size_bytes Total disk space used by this table, in bytes
# TYPE pg_stat_user_tables_table_size_bytes gauge
pg_stat_user_tables_table_size_bytes{datname="{{ datname }}",schemaname="{{ schemaname }}",relname="{{ relname }}"} {{ table_size_bytes }}
{{/ .}}
