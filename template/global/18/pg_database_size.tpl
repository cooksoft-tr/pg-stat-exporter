# HELP pg_database_size_bytes Database size in bytes.
# TYPE pg_database_size_bytes gauge
{{# . }}
pg_database_size_bytes{datname="{{ datname }}"} {{ pg_database_size }}
{{/ . }}

