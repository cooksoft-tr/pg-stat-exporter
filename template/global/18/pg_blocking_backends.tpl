{{# . }}
{{#blocking_transaction_count}}
# HELP pg_blocking_transactions_total Number of blocking transactions per database
# TYPE pg_blocking_transactions_total gauge
pg_blocking_transactions_total{datname="{{datname}}"} {{blocking_transaction_count}}
{{/blocking_transaction_count}}
{{#blocking_virtualxid_count}}
# HELP pg_blocking_virtualxid_total Number of blocking virtualxid locks per database
# TYPE pg_blocking_virtualxid_total gauge
pg_blocking_virtualxid_total{datname="{{datname}}"} {{blocking_virtualxid_count}}
{{/blocking_virtualxid_count}}
{{#blocking_relation_count}}
# HELP pg_blocking_relations_total Number of blocking relation locks per database
# TYPE pg_blocking_relations_total gauge
pg_blocking_relations_total{datname="{{datname}}"} {{blocking_relation_count}}
{{/blocking_relation_count}}
{{/ . }}