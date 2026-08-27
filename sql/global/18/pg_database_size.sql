WITH s AS (
  SELECT
    datname,
    pg_database_size(datname) AS pg_database_size
  FROM pg_stat_database
  WHERE 
    datname NOT IN ('template0', 'template1')
)
SELECT jsonb_agg(to_jsonb(s)) AS pg_database_size
FROM s;
