#!/bin/bash

set -ueo pipefail

ROOT="$(dirname $(readlink -f "$0"))/.."

cd $ROOT && run-clang-tidy -fix -quiet -p build "^$(pwd)/(src|test)/"
