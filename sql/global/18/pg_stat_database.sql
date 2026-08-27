WITH s AS (
  SELECT
    datname,
    numbackends,
    xact_commit,
    xact_rollback,
    blks_read,
    blks_hit,
    tup_returned,
    tup_fetched,
    tup_inserted,
    tup_updated,
    tup_deleted,
    conflicts,
    temp_files,
    temp_bytes,
    deadlocks,
    blk_read_time,
    blk_write_time,
    session_time              AS session_time_seconds,
    active_time               AS active_time_seconds,
    idle_in_transaction_time  AS idle_in_transaction_time_seconds,
    sessions_abandoned,
    sessions_fatal,
    sessions_killed
  FROM pg_stat_database
  WHERE 
    datname NOT IN ('template0', 'template1')
)
SELECT jsonb_agg(to_jsonb(s)) AS pg_stat_database
FROM s;
