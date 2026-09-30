WITH s AS (
  SELECT
      current_database() AS datname,
      schemaname,
      relname,
      seq_scan,
      seq_tup_read,
      COALESCE(idx_scan, 0) AS idx_scan,
      COALESCE(idx_tup_fetch, 0) AS idx_tup_fetch,
      n_tup_ins,
      n_tup_upd,
      n_tup_del,
      n_tup_hot_upd,
      n_live_tup,
      n_dead_tup,
      n_mod_since_analyze,
      EXTRACT(EPOCH FROM COALESCE(last_vacuum, '1970-01-01Z'::timestamptz))      AS last_vacuum,
      EXTRACT(EPOCH FROM COALESCE(last_autovacuum, '1970-01-01Z'::timestamptz))  AS last_autovacuum,
      EXTRACT(EPOCH FROM COALESCE(last_analyze, '1970-01-01Z'::timestamptz))     AS last_analyze,
      EXTRACT(EPOCH FROM COALESCE(last_autoanalyze, '1970-01-01Z'::timestamptz)) AS last_autoanalyze,
      vacuum_count,
      autovacuum_count,
      analyze_count,
      autoanalyze_count,
      pg_indexes_size(relid) AS index_size_bytes,
      pg_table_size(relid)   AS table_size_bytes
    FROM pg_stat_user_tables
)
SELECT jsonb_agg(to_jsonb(s)) AS pg_stat_user_tables
FROM s;
