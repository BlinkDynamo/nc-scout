#!/usr/bin/env bash

# Copyright (c) 2024-2025 Josh Hayden (@BlinkDynamo)

# This file should only be executed by the Makefile because it defines these variables.
if [ -z "${BUILD_DIR}" ] && [ -z "${TESTS_DIR}" ]; then
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
function check_test_dependencies()
{
    status=0
    
    for dependency in $@; do 
        if ! command -v $dependency 2>&1 >/dev/null; then
            printf "%b %s\n" \
                "[${RED}X${RESET}]" \
                "Missing: $dependency"
            status=1
        else
            printf "%b %s\n" \
                "[${GREEN}✓${RESET}]" \
                "Found: $dependency"
        fi
    done

    if [ "$status" -eq 1 ]; then
        printf "\nTest dependencies were missing. Exiting.\n"
        exit $status
    fi
}

#----------------------------------------------------------------------------------------------#
# Build
#----------------------------------------------------------------------------------------------#
printf "\nChecking for test dependencies...\n\n"
check_test_dependencies "valgrind" "grep" "wc"
