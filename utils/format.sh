#!/bin/bash

set -ueo pipefail

ROOT="$(dirname $(readlink -f "$0"))/.."

# Output the filenames by separating them with '\0'
# and then tell xargs to expect this input (with '-0')
#
# '-i' edits the file in-place as opposed to outputting to stdout
cd $ROOT && git ls-files -z '*.cpp' '*.hpp' | xargs -0 clang-format -i
