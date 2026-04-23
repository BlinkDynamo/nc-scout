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

    elif ! valgrind --error-exitcode=1 --leak-check=full $test_command > /dev/null 2>&1; then 
        printf "%b %s\n" "[${RED}!${RESET}]" "Error: valgrind ran '$test_command' and returned an error."

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

# Runs test_command and verifies its output lines against the expected key=value pairs.
# Each pair takes the form convention=count (e.g. flatcase=10, other=0, total=10).
# A test passes only if the command exits cleanly, valgrind reports no errors, and every
# expected pair matches the count parsed from the corresponding output line.
function check_analyze()
{
    (( tests_executed++ ))

    local test_command="$1"
    shift
    local expected_pairs=("$@")

    # Firstly, check if the test_command exits with a non-zero exit code.
    if ! $test_command>/dev/null; then
        printf "%b %s\n" \
            "[${RED}!${RESET}]" \
            "Error: '$test_command' exited abnormally."
        return
    fi

    if ! valgrind --error-exitcode=1 --leak-check=full $test_command > /dev/null 2>&1; then
        printf "%b %s\n" \
            "[${RED}!${RESET}]" \
            "Error: valgrind ran '$test_command' and returned an error."
        return
    fi

    local output
    output=$($test_command)

    local all_match=true
    local mismatch_details=""
    local pair key expected observed
    for pair in "${expected_pairs[@]}"; do
        key="${pair%%=*}"
        expected="${pair#*=}"
        observed=$(printf "%s\n" "$output" | awk -v k="$key" '$1 == k { print $2; exit }')
        if [ "$observed" != "$expected" ]; then
            all_match=false
            mismatch_details+=" ${key}: expected ${expected}, got ${observed:-<missing>};"
        fi
    done

    if $all_match; then
        printf "%b %s\n" \
            "[${GREEN}✓${RESET}]" \
            "Pass: $test_command"
        (( tests_passed++ ))
    else
        printf "%b %s ->%s\n" \
            "[${RED}X${RESET}]" \
            "Fail: $test_command" \
            "$mismatch_details"
    fi
}

#----------------------------------------------------------------------------------------------#
# Test Execution
#----------------------------------------------------------------------------------------------#
printf "\n${ORANGE}Testing 'search' subcommand. Expecting 10 matches per directory depth.${RESET}\n\n"

printf "${ORANGE}Simple tests for strict matches:${RESET}\n"
check_search "./$BUILD_DIR/nctool search -s flatcase $TESTS_DIR/flatcase_strict_matches" 10
check_search "./$BUILD_DIR/nctool search -s camelcase $TESTS_DIR/camelcase_strict_matches" 10
check_search "./$BUILD_DIR/nctool search -s pascalcase $TESTS_DIR/pascalcase_strict_matches" 10
check_search "./$BUILD_DIR/nctool search -s snakecase $TESTS_DIR/snakecase_strict_matches" 10
check_search "./$BUILD_DIR/nctool search -s constantcase $TESTS_DIR/constantcase_strict_matches" 10
check_search "./$BUILD_DIR/nctool search -s kebabcase $TESTS_DIR/kebabcase_strict_matches" 10
check_search "./$BUILD_DIR/nctool search -s cobolcase $TESTS_DIR/cobolcase_strict_matches" 10
printf "\n"

printf "${ORANGE}Simple tests for lenient matches:${RESET}\n"
check_search "./$BUILD_DIR/nctool search flatcase $TESTS_DIR/flatcase_lenient_matches" 10
check_search "./$BUILD_DIR/nctool search camelcase $TESTS_DIR/camelcase_lenient_matches" 10
check_search "./$BUILD_DIR/nctool search pascalcase $TESTS_DIR/pascalcase_lenient_matches" 10
check_search "./$BUILD_DIR/nctool search snakecase $TESTS_DIR/snakecase_lenient_matches" 10
check_search "./$BUILD_DIR/nctool search constantcase $TESTS_DIR/constantcase_lenient_matches" 10
check_search "./$BUILD_DIR/nctool search kebabcase $TESTS_DIR/kebabcase_lenient_matches" 10
check_search "./$BUILD_DIR/nctool search cobolcase $TESTS_DIR/cobolcase_lenient_matches" 10
printf "\n"

printf "${ORANGE}Recursive tests for strict matches:${RESET}\n"
check_search "./$BUILD_DIR/nctool search -sR flatcase $TESTS_DIR/flatcase_strict_matches" 100
check_search "./$BUILD_DIR/nctool search -sR camelcase $TESTS_DIR/camelcase_strict_matches" 100
check_search "./$BUILD_DIR/nctool search -sR pascalcase $TESTS_DIR/pascalcase_strict_matches" 100
check_search "./$BUILD_DIR/nctool search -sR snakecase $TESTS_DIR/snakecase_strict_matches" 100
check_search "./$BUILD_DIR/nctool search -sR constantcase $TESTS_DIR/constantcase_strict_matches" 100
check_search "./$BUILD_DIR/nctool search -sR kebabcase $TESTS_DIR/kebabcase_strict_matches" 100
check_search "./$BUILD_DIR/nctool search -sR cobolcase $TESTS_DIR/cobolcase_strict_matches" 100
printf "\n"

printf "${ORANGE}Recursive tests for lenient matches:${RESET}\n"
check_search "./$BUILD_DIR/nctool search -R flatcase $TESTS_DIR/flatcase_lenient_matches" 100
check_search "./$BUILD_DIR/nctool search -R camelcase $TESTS_DIR/camelcase_lenient_matches" 100
check_search "./$BUILD_DIR/nctool search -R pascalcase $TESTS_DIR/pascalcase_lenient_matches" 100
check_search "./$BUILD_DIR/nctool search -R snakecase $TESTS_DIR/snakecase_lenient_matches" 100
check_search "./$BUILD_DIR/nctool search -R constantcase $TESTS_DIR/constantcase_lenient_matches" 100
check_search "./$BUILD_DIR/nctool search -R kebabcase $TESTS_DIR/kebabcase_lenient_matches" 100
check_search "./$BUILD_DIR/nctool search -R cobolcase $TESTS_DIR/cobolcase_lenient_matches" 100
printf "\n"

printf "\n${ORANGE}Testing 'analyze' subcommand. Verifying every output line.${RESET}\n\n"

printf "${ORANGE}Simple tests, lenient matching (no flags):${RESET}\n"
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/flatcase_strict_matches" \
    flatcase=10 camelcase=5 pascalcase=0 snakecase=10 constantcase=0 \
    kebabcase=10 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/camelcase_strict_matches" \
    flatcase=0 camelcase=10 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/pascalcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=10 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/snakecase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=10 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/constantcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=10 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/kebabcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=10 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/cobolcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=10 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/flatcase_lenient_matches" \
    flatcase=10 camelcase=5 pascalcase=0 snakecase=10 constantcase=0 \
    kebabcase=10 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/camelcase_lenient_matches" \
    flatcase=10 camelcase=10 pascalcase=0 snakecase=10 constantcase=0 \
    kebabcase=10 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/pascalcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=10 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/snakecase_lenient_matches" \
    flatcase=10 camelcase=10 pascalcase=0 snakecase=10 constantcase=0 \
    kebabcase=10 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/constantcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=10 \
    kebabcase=0 cobolcase=10 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/kebabcase_lenient_matches" \
    flatcase=10 camelcase=10 pascalcase=0 snakecase=10 constantcase=0 \
    kebabcase=10 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze $TESTS_DIR/cobolcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=10 \
    kebabcase=0 cobolcase=10 other=0 total=10
printf "\n"

printf "${ORANGE}Simple tests, strict matching (-s):${RESET}\n"
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/flatcase_strict_matches" \
    flatcase=10 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/camelcase_strict_matches" \
    flatcase=0 camelcase=10 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/pascalcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=10 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/snakecase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=10 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/constantcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=10 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/kebabcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=10 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/cobolcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=10 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/flatcase_lenient_matches" \
    flatcase=10 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/camelcase_lenient_matches" \
    flatcase=10 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/pascalcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=10 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/snakecase_lenient_matches" \
    flatcase=10 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/constantcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=10 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/kebabcase_lenient_matches" \
    flatcase=10 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=10
check_analyze "./$BUILD_DIR/nctool analyze -s $TESTS_DIR/cobolcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=10 total=10
printf "\n"

printf "${ORANGE}Recursive tests, lenient matching (-R):${RESET}\n"
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/flatcase_strict_matches" \
    flatcase=100 camelcase=50 pascalcase=0 snakecase=100 constantcase=0 \
    kebabcase=100 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/camelcase_strict_matches" \
    flatcase=0 camelcase=100 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/pascalcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=100 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/snakecase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=100 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/constantcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=100 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/kebabcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=100 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/cobolcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=100 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/flatcase_lenient_matches" \
    flatcase=100 camelcase=50 pascalcase=0 snakecase=100 constantcase=0 \
    kebabcase=100 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/camelcase_lenient_matches" \
    flatcase=100 camelcase=100 pascalcase=0 snakecase=100 constantcase=0 \
    kebabcase=100 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/pascalcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=100 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/snakecase_lenient_matches" \
    flatcase=100 camelcase=100 pascalcase=0 snakecase=100 constantcase=0 \
    kebabcase=100 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/constantcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=100 \
    kebabcase=0 cobolcase=100 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/kebabcase_lenient_matches" \
    flatcase=100 camelcase=100 pascalcase=0 snakecase=100 constantcase=0 \
    kebabcase=100 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -R $TESTS_DIR/cobolcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=100 \
    kebabcase=0 cobolcase=100 other=0 total=100
printf "\n"

printf "${ORANGE}Recursive tests, strict matching (-sR):${RESET}\n"
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/flatcase_strict_matches" \
    flatcase=100 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/camelcase_strict_matches" \
    flatcase=0 camelcase=100 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/pascalcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=100 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/snakecase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=100 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/constantcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=100 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/kebabcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=100 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/cobolcase_strict_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=100 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/flatcase_lenient_matches" \
    flatcase=100 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/camelcase_lenient_matches" \
    flatcase=100 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/pascalcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=100 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/snakecase_lenient_matches" \
    flatcase=100 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/constantcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=100 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/kebabcase_lenient_matches" \
    flatcase=100 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=0 total=100
check_analyze "./$BUILD_DIR/nctool analyze -sR $TESTS_DIR/cobolcase_lenient_matches" \
    flatcase=0 camelcase=0 pascalcase=0 snakecase=0 constantcase=0 \
    kebabcase=0 cobolcase=0 other=100 total=100
printf "\n"

printf "\nTesting completed. %s/%s tests were successful.\n\n" "$tests_passed" "$tests_executed"
