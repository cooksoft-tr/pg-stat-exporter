{{#value}}
# HELP pg_setting_{{name}}{{#unit}}_{{unit}}{{/unit}} {{short_desc}}
# TYPE pg_setting_{{name}}{{#unit}}_{{unit}}{{/unit}} gauge
pg_setting_{{name}}{{#unit}}_{{unit}}{{/unit}} {{setting}}

{{/value}}

{{#attribute}}
# HELP pg_setting_{{name}} {{short_desc}}
# TYPE pg_setting_{{name}} gauge
pg_setting_{{name}}{setting="{{setting}}"} 1

{{/attribute}}
