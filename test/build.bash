#!/usr/bin/env bash
# Configure, build and run the cmdp tests.
set -euo pipefail

src_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${src_dir}/build"

# Clean build: pass --clean
if [ "${1:-}" = "--clean" ]; then
    rm -rf "${build_dir}"
fi

# Generate build system
cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    -DCMAKE_BUILD_TYPE:STRING=Debug \
    -S "${src_dir}" \
    -B "${build_dir}" \
    -G "Unix Makefiles"

# Build lib and test app
cmake --build "${build_dir}"

# Run tests
cd "${build_dir}" && ctest --output-on-failure
