SELECT jsonb_build_object(
  'pg_postmaster_start_time_seconds', extract(epoch from pg_postmaster_start_time())
) AS pg_postmaster_start_time_seconds;
