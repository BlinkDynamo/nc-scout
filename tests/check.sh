#!/usr/bin/env bash

# Copyright (c) 2024-2025 Josh Hayden (@BlinkDynamo)

# This file should only be executed by the Makefile because it defines these variables.
if [ -z "${BUILD_DIR}" ] && [ -z "${TESTS_DIR}" ]; then
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

# Compares the lines of output of test_command to n_expected_correct.
function check_search()
{
    (( tests_executed++ ))
    
    local test_command="$1"         # The command to execute, capturing it's lines of output.
    local n_expected_correct="$2"   # The expected number of lines of output from the command.
     
    # Firstly, check if the test_command exits with a non-zero exit code. If so, print an error message.
    if ! $test_command>/dev/null; then
        printf "%b %s\n" "[${RED}!${RESET}]" "Error: '$test_command' exited abnormally."
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

# Compares the lines of output of test_command to n_expected_correct.
function check_analyze()
{
    (( tests_executed++ ))
    
    local test_command="$1"         # The command to execute, capturing it's percentage of present matches.
    local p_expected_correct="$2"   # The expected percentage of matches.
     
    # Firstly, check if the test_command exits with a non-zero exit code. If so, print an error message.
    if ! $test_command>/dev/null; then
        printf "%b %s\n" "[${RED}!${RESET}]" "Error: '$test_command' exited abnormally."

    elif [ "$($test_command | grep -o "make up ${p_expected_correct}% of" | wc -l)" -eq 1 ]; then
        
        printf "%b %s\n" \
            "[${GREEN}✓${RESET}]" \
            "Pass: $test_command"
        (( tests_passed++ ))

    else
        printf "%b %s -> %s\n" \
            "[${RED}X${RESET}]" \
            "Fail: $test_command" \
            "Percentage observed did not equal ${p_expected_correct}"
    fi
}

#----------------------------------------------------------------------------------------------#
# Test Execution
#----------------------------------------------------------------------------------------------#
printf "\n${ORANGE}Testing 'search' subcommand. Expecting 10 matches per directory depth.${RESET}\n\n"

printf "${ORANGE}Simple tests for strict matches:${RESET}\n"
check_search "./$BUILD_DIR/nc-scout search -s flatcase $TESTS_DIR/flatcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -s camelcase $TESTS_DIR/camelcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -s pascalcase $TESTS_DIR/pascalcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -s snakecase $TESTS_DIR/snakecase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -s constantcase $TESTS_DIR/constantcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -s kebabcase $TESTS_DIR/kebabcase_strict_matches" 10
check_search "./$BUILD_DIR/nc-scout search -s cobolcase $TESTS_DIR/cobolcase_strict_matches" 10
printf "\n"

printf "${ORANGE}Simple tests for lenient matches:${RESET}\n"
check_search "./$BUILD_DIR/nc-scout search flatcase $TESTS_DIR/flatcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search camelcase $TESTS_DIR/camelcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search pascalcase $TESTS_DIR/pascalcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search snakecase $TESTS_DIR/snakecase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search constantcase $TESTS_DIR/constantcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search kebabcase $TESTS_DIR/kebabcase_lenient_matches" 10
check_search "./$BUILD_DIR/nc-scout search cobolcase $TESTS_DIR/cobolcase_lenient_matches" 10
printf "\n"

printf "${ORANGE}Recursive tests for strict matches:${RESET}\n"
check_search "./$BUILD_DIR/nc-scout search -sR flatcase $TESTS_DIR/flatcase_strict_matches" 100
check_search "./$BUILD_DIR/nc-scout search -sR camelcase $TESTS_DIR/camelcase_strict_matches" 100
check_search "./$BUILD_DIR/nc-scout search -sR pascalcase $TESTS_DIR/pascalcase_strict_matches" 100
check_search "./$BUILD_DIR/nc-scout search -sR snakecase $TESTS_DIR/snakecase_strict_matches" 100
check_search "./$BUILD_DIR/nc-scout search -sR constantcase $TESTS_DIR/constantcase_strict_matches" 100
check_search "./$BUILD_DIR/nc-scout search -sR kebabcase $TESTS_DIR/kebabcase_strict_matches" 100
check_search "./$BUILD_DIR/nc-scout search -sR cobolcase $TESTS_DIR/cobolcase_strict_matches" 100
printf "\n"

printf "${ORANGE}Recursive tests for lenient matches:${RESET}\n"
check_search "./$BUILD_DIR/nc-scout search -R flatcase $TESTS_DIR/flatcase_lenient_matches" 100
check_search "./$BUILD_DIR/nc-scout search -R camelcase $TESTS_DIR/camelcase_lenient_matches" 100
check_search "./$BUILD_DIR/nc-scout search -R pascalcase $TESTS_DIR/pascalcase_lenient_matches" 100
check_search "./$BUILD_DIR/nc-scout search -R snakecase $TESTS_DIR/snakecase_lenient_matches" 100
check_search "./$BUILD_DIR/nc-scout search -R constantcase $TESTS_DIR/constantcase_lenient_matches" 100
check_search "./$BUILD_DIR/nc-scout search -R kebabcase $TESTS_DIR/kebabcase_lenient_matches" 100
check_search "./$BUILD_DIR/nc-scout search -R cobolcase $TESTS_DIR/cobolcase_lenient_matches" 100
printf "\n"

printf "\n${ORANGE}Testing 'analyze' subcommand. Expecting 100.000%% matches for each test.${RESET}\n\n"

printf "${ORANGE}Simple tests for strict matches:${RESET}\n"
check_analyze "./$BUILD_DIR/nc-scout analyze flatcase $TESTS_DIR/flatcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze camelcase $TESTS_DIR/camelcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze pascalcase $TESTS_DIR/pascalcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze snakecase $TESTS_DIR/snakecase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze constantcase $TESTS_DIR/constantcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze kebabcase $TESTS_DIR/kebabcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze cobolcase $TESTS_DIR/cobolcase_strict_matches" 100.000
printf "\n"

printf "${ORANGE}Simple tests for lenient matches:${RESET}\n"
check_analyze "./$BUILD_DIR/nc-scout analyze flatcase $TESTS_DIR/flatcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze camelcase $TESTS_DIR/camelcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze pascalcase $TESTS_DIR/pascalcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze snakecase $TESTS_DIR/snakecase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze constantcase $TESTS_DIR/constantcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze kebabcase $TESTS_DIR/kebabcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze cobolcase $TESTS_DIR/cobolcase_lenient_matches" 100.000
printf "\n"

printf "${ORANGE}Recursive tests for strict matches:${RESET}\n"
check_analyze "./$BUILD_DIR/nc-scout analyze -R flatcase $TESTS_DIR/flatcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R camelcase $TESTS_DIR/camelcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R pascalcase $TESTS_DIR/pascalcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R snakecase $TESTS_DIR/snakecase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R constantcase $TESTS_DIR/constantcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R kebabcase $TESTS_DIR/kebabcase_strict_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R cobolcase $TESTS_DIR/cobolcase_strict_matches" 100.000
printf "\n"

printf "${ORANGE}Recursive tests for lenient matches:${RESET}\n"
check_analyze "./$BUILD_DIR/nc-scout analyze -R flatcase $TESTS_DIR/flatcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R camelcase $TESTS_DIR/camelcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R pascalcase $TESTS_DIR/pascalcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R snakecase $TESTS_DIR/snakecase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R constantcase $TESTS_DIR/constantcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R kebabcase $TESTS_DIR/kebabcase_lenient_matches" 100.000
check_analyze "./$BUILD_DIR/nc-scout analyze -R cobolcase $TESTS_DIR/cobolcase_lenient_matches" 100.000
printf "\n"

printf "\nTesting completed. %s/%s tests were successful.\n\n" "$tests_passed" "$tests_executed"
