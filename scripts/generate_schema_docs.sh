#!/bin/bash

set -e

cd $(dirname $(dirname $0))

mkdir -p doc/sphinx/src/configurations/schemas

poetry run jsonschema2md schemas/config_schema_v1.json doc/sphinx/src/configurations/schemas/config_schema_v1.md
