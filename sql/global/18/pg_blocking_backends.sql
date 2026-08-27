WITH blocking_backends AS (
  SELECT
      (
        SELECT sa.datname
        FROM pg_stat_activity sa
        WHERE
          sa.pid = COALESCE(blocking_t.pid, blocking_x.pid, blocking_r.pid)
      ) datname,
      blocking_t.pid AS blocking_transaction,
      blocking_x.pid AS blocking_virtualxid,
      blocking_r.pid AS blocking_relation
    FROM pg_locks blocked
    LEFT JOIN pg_locks blocking_t ON
        blocked.locktype = 'transactionid' AND
        blocking_t.locktype = 'transactionid' AND
        blocking_t.transactionid = blocked.transactionid AND
        blocking_t.granted
    LEFT JOIN pg_locks blocking_x ON
        blocked.virtualxid = 'virtualxid' AND
        blocking_x.locktype = 'virtualxid' AND
        blocking_x.virtualxid = blocked.virtualxid AND
        blocking_x.granted
    LEFT JOIN pg_locks blocking_r ON
        blocking_r.locktype = 'relation' AND
        blocked.locktype = 'relation' AND
        blocking_r.database = blocked.database AND
        blocking_r.relation = blocked.relation AND
        blocking_r.granted
    WHERE
      NOT blocked.granted
), 
blocking_backend_counts AS (
  SELECT
      datname,
      count(blocking_transaction) blocking_transaction_count,
      count(blocking_virtualxid) blocking_virtualxid_count,
      count(blocking_relation) blocking_relation_count
    FROM blocking_backends
    GROUP BY 1
)
SELECT jsonb_agg(to_jsonb(blocking_backend_counts)) AS blocking_backend_counts
FROM blocking_backend_counts;
