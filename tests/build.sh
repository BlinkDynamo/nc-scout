#!/usr/bin/env bash

# Copyright (c) 2024-2025 Josh Hayden (@BlinkDynamo)

# This file should only be executed by the Makefile because it defines these variables.
if [ -z ${BUILD_DIR} ] && [ -z ${TESTS_DIR} ]; then
    printf "%s\n%s\n" \
           "Error: build-tests.sh is a child script of the Makefile." \
           "It should not be executed manually."
    exit 1
fi

# Source the data file with filename arrays.
source "tests/data.sh.inc"

#----------------------------------------------------------------------------------------------#
# Definitions 
#----------------------------------------------------------------------------------------------#
build_test_directory() {
    # The name of test being executed,  as well as the array name of it's needed data in tests/filenames.inc.
    local testname="$1"
    # The directory depth of tests to create.
    local depth="$2"

    # The current working directory of the function.
    local cwd="${TESTS_DIR}/${testname}"

    mkdir -p "${cwd}"
    printf "Built test directory '%s'.\n" "${testname}"

    # Construct the variable name dynamically.

    # Use eval to access the array.
    eval "values=(\"\${${testname}[@]}\")"
    if eval "[[ -z \"\${${testname}[*]}\" ]]" 2>/dev/null; then
        echo "Unable to evaluate $testname to an array."
        return
    fi

    # For a number of iterations "depth", create values[0] as a directory, values[1] through
    # values[-1] as files, and cd inside of values[0].
    for i in $(seq 1 "${depth}"); do
        # Make the first member a directory.
        mkdir "${cwd}/${values[0]}"

        # Make the remaining members files.
        for item in "${values[@]:1}"; do
            touch "${cwd}/$item"
        done

        cwd="${cwd}/${values[0]}"
    done
}

#----------------------------------------------------------------------------------------------#
# Build
#----------------------------------------------------------------------------------------------#
printf "\nBuilding test directories...\n\n"

# Strict matches.
build_test_directory "flatcase_strict_matches" 10
build_test_directory "camelcase_strict_matches" 10
build_test_directory "pascalcase_strict_matches" 10
build_test_directory "kebabcase_strict_matches" 10
build_test_directory "cobolcase_strict_matches" 10
build_test_directory "snakecase_strict_matches" 10
build_test_directory "constantcase_strict_matches" 10

# Lenient matches.
build_test_directory "flatcase_lenient_matches" 10
build_test_directory "camelcase_lenient_matches" 10
build_test_directory "pascalcase_lenient_matches" 10
build_test_directory "kebabcase_lenient_matches" 10
build_test_directory "cobolcase_lenient_matches" 10
build_test_directory "snakecase_lenient_matches" 10
build_test_directory "constantcase_lenient_matches" 10

printf "\nTest directories built successfully.\n\n"
