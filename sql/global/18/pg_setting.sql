WITH s AS (
  SELECT
      name,
      vartype,
      CASE
        WHEN unit IN ('ms','s','min') THEN 'seconds'
        WHEN unit IN ('B','kB','MB','8kB') THEN 'bytes'
        ELSE COALESCE(unit,'')
      END AS unit,
      CASE
        WHEN vartype = 'bool'
          THEN (setting = 'on')::int::text
        WHEN unit IN ('B', 'kB', 'MB') 
          THEN pg_size_bytes(concat(setting,unit))::text
        WHEN unit = '8kB'
          THEN pg_size_bytes(concat(setting::bigint * 8,'kB'))::text
        WHEN unit IN ('ms','s','min')
          THEN extract(epoch FROM (concat(setting, unit))::interval)::text
        ELSE setting
      END AS setting,
      short_desc
    FROM pg_settings
    WHERE
      name IN (
        'autovacuum',
        'autovacuum_max_workers',
        'autovacuum_worker_slots',
        'bgwriter_delay',
        'bgwriter_lru_maxpages',
        'bgwriter_lru_multiplier',
        'checkpoint_timeout',
        'cluster_name',
        'config_file',
        'data_checksums',
        'data_directory',
        'default_statistics_target',
        'effective_cache_size',
        'effective_io_concurrency',
        'hba_file',
        'hot_standby_feedback',
        'jit',
        'maintenance_io_concurrency',
        'maintenance_work_mem',
        'max_connections',
        'max_logical_replication_workers',
        'max_parallel_workers',
        'max_replication_slots',
        'max_slot_wal_keep_size',
        'max_wal_senders',
        'max_wal_size',
        'max_worker_processes',
        'min_wal_size',
        'reserved_connections',
        'server_version',
        'server_version_num',
        'shared_buffers',
        'shared_memory_size',
        'ssl',
        'superuser_reserved_connections',
        'temp_buffers',
        'wal_compression',
        'work_mem'
      )
)
SELECT
  json_build_object (
      'value', 
      jsonb_agg(to_jsonb(s)) FILTER (WHERE vartype NOT IN ('string','enum')),
      'attribute',
      jsonb_agg(to_jsonb(s)) FILTER (WHERE vartype IN ('string','enum'))
  ) 
  AS pg_settings
FROM s;
