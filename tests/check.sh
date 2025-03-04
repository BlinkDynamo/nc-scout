#!/usr/bin/env bash

# Copyright (c) 2024-2025 Josh Hayden (@BlinkDynamo)

# This file should only be executed by the Makefile because it defines these variables.
if [ -z ${BUILD_DIR} ] && [ -z ${TESTS_DIR} ]; then
    printf "%s\n%s\n" \
           "Error: build-tests.sh is a child script of the Makefile." \
           "It should not be executed manually."
    exit 1
fi

#----------------------------------------------------------------------------------------------#
# Definitions 
#----------------------------------------------------------------------------------------------#
GREEN="\033[0;32m"
RED="\033[0;31m"
ORANGE="\033[0;33m"
RESET="\033[0m"

# Counters incremented by test functions.
tests_passed=0
tests_executed=0

function check_search()
{
    (( tests_executed++ ))
    
    local test_command="$1"         # The command to execute, capturing it's lines of output.
    local n_expected_correct="$2"   # The expected number of lines of output from the command.
     
    # Firstly, check if the test_command exits with a non-zero exit code. If so, print an error message.
    if ! $test_command>/dev/null; then
        printf "%b %s\n" "[${RED}!${RESET}]" "Error: Test '$test_name' exited abnormally."
    else
        local n_observed_correct=$($test_command | wc -l)
        
        if [ "$n_observed_correct" -eq "$n_expected_correct" ]; then
            printf "%b %s\n" \
                "[${GREEN}✓${RESET}]" \
                "Pass: $test_command"
            (( tests_passed++ ))
        else
            printf "%b %s -> %s\n" \
                "[${RED}X${RESET}]" \
                "Fail: $test_command" \
                "Matches observed: $n_observed_correct"
        fi
    fi     
}

#----------------------------------------------------------------------------------------------#
# Test Execution
#----------------------------------------------------------------------------------------------#
printf "\nTesting 'search' subcommand. Expecting 10 matches for each test.\n\n"

printf "${ORANGE}Tests for strict matches:${RESET}\n"
check_search "./$BUILD_DIR/nc-scout search -sm flatcase $TESTS_DIR/flatcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -sm camelcase $TESTS_DIR/camelcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -sm pascalcase $TESTS_DIR/pascalcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -sm snakecase $TESTS_DIR/snakecase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -sm constantcase $TESTS_DIR/constantcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -sm kebabcase $TESTS_DIR/kebabcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -sm cobolcase $TESTS_DIR/cobolcase_strict_matches" 10
printf "\n"

printf "${ORANGE}Tests for lenient matches:${RESET}\n"
check_search "./$BUILD_DIR/nc-scout search -m flatcase $TESTS_DIR/flatcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search -m camelcase $TESTS_DIR/camelcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search -m pascalcase $TESTS_DIR/pascalcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search -m snakecase $TESTS_DIR/snakecase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search -m constantcase $TESTS_DIR/constantcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search -m kebabcase $TESTS_DIR/kebabcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search -m cobolcase $TESTS_DIR/cobolcase_lenient_matches" 10
printf "\n"

printf "\nTesting completed. %s/%s tests were successful.\n\n" "$tests_passed" "$tests_executed"
