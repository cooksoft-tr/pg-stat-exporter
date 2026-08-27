WITH s AS (
  SELECT 
      count(*) FILTER( WHERE backend_type = 'client backend' and state = 'active') AS active,
      count(*) FILTER( WHERE backend_type = 'client backend' and state = 'idle') AS idle,
      count(*) FILTER( WHERE backend_type = 'client backend' and state = 'idle in transaction') AS idle_in_transaction,
      count(*) FILTER( WHERE backend_type = 'client backend' and state = 'idle in transaction aborted') AS idle_in_transaction_aborted,
      count(*) FILTER( WHERE backend_type = 'client backend') AS total,
      count(*) FILTER( WHERE wait_event_type = 'Lock') as waiting,
      count(*) FILTER( WHERE backend_type = 'autovacuum worker') AS autovacuum_workers,
      count(*) FILTER( WHERE backend_type = 'logical replication worker') AS logical_replication_workers,
      count(*) FILTER( WHERE backend_type = 'parallel worker') AS parallel_workers,
      count(*) FILTER( WHERE backend_type = 'walsender') AS wal_sender
    FROM pg_stat_activity sa
  )
SELECT to_jsonb(s) AS pg_database_size
FROM s;
