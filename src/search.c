/**********************************************************************************************
*
*   search - Handles options, validation, and runtime of the nctool search subcommand.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2024-2025 Josh Hayden (@jhayden02)
*
*   This software is provided ‘as-is’, without any express or implied
*   warranty. In no event will the authors be held liable for any damages
*   arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose,
*   including commercial applications, and to alter it and redistribute it
*   freely, subject to the following restrictions:
*
*   1. The origin of this software must not be misrepresented; you must not
*   claim that you wrote the original software. If you use this software
*   in a product, an acknowledgment in the product documentation would be
*   appreciated but is not required.
*
*   2. Altered source versions must be plainly marked as such, and must not be
*   misrepresented as being the original software.
*
*   3. This notice may not be removed or altered from any source
*   distribution.
*
*********************************************************************************************/

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif // _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "help.h"
#include "validate.h"
#include "common.h"
#include "naming.h"
#include "search.h"

#define N_REQUIRED_ARGS 2

// Flags.
static bool full_path_flag      = false;
static bool non_matches_flag    = false;
static bool strict_flag         = false;
static bool recursive_flag      = false;

static const char *initial_search_path = NULL;

static void process_current_file (struct dirent *current_file, const char *search_path,
                                  const regex_t *regex)
/**********************************************************************************************
*
*   Compares a d_name to a regular expression. Will print matches or non-matches depending on
*   matches_flag.
*
*   ---------------------------------------- ARGUMENTS ----------------------------------------
*
*   current_file                The pointer to the current file being processed.
*
*   search_path                 The absolute path of the dir current_file is searching in.
*
*   regex                       The compiled regex of the convention being searched for.
*
**********************************************************************************************/
{
    bool is_match = naming_match_regex(regex, current_file->d_name);
    bool should_print = (is_match != non_matches_flag);

    if (should_print)
    {
        if (full_path_flag)
            {
                printf("%s/%s\n", search_path, current_file->d_name);
            }
        else
        {
        // If relative_path is empty (current directory), insert a forward-slash between
        // relative_path and current_file->d_name.
        const char *relative_path = get_relative_path(initial_search_path, search_path);
        printf("%s%s%s\n",
               relative_path,
               (strlen(relative_path) > 0) ? "/" : "",
               current_file->d_name);
        }
    }
}


int subc_exec_search (int argc, char *argv[])
/**********************************************************************************************
*
*   The external function called from src/main.c. Processes options, compiles the regex, does
*   validation, and finally calls search_directory.
*
*   ---------------------------------------- ARGUMENTS ----------------------------------------
*
*   argc    The argc of main() in src/main.c with 1 removed.
*
*   argv    The argv of main() in src/main.c with "nctool" removed from the beginning.
*
*   ----------------------------------------- RETURNS -----------------------------------------
*
*   It inherrits the return of main() in src/main.c, returning EXIT_FAILURE or EXIT_SUCCESS.
*
**********************************************************************************************/
{
    full_path_flag = false;
    non_matches_flag = false;
    strict_flag = false;
    recursive_flag = false;
    initial_search_path = NULL;
    optind = 1;

    int current_opt;

    while (1)
    {
        static struct option long_options_search[] =
        {
            {"help", no_argument, 0, 'h'},
            {"full-path", no_argument, 0, 'f'}, 
            {"non-matches", no_argument, 0, 'n'},
            {"strict", no_argument, 0, 's'},
            {"recursive", no_argument, 0, 'R'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        current_opt = getopt_long (argc, argv, "+hfnsR", long_options_search, &option_index);
        // Break if at the end of the options.
        if (current_opt == -1) {
            break;
        }

        switch (current_opt)
        {
            case '?':
                return EXIT_FAILURE;

            case 'h':
                // Make sure that there are no arguments supplied.
                if (argc == 2) {
                    printf("%s", HELP_SEARCH);
                    return EXIT_SUCCESS;
                }
                else {
                    fprintf(stderr, "Incorrect usage.\nDo `nctool search --help` for more information about usage.\n");
                    return EXIT_FAILURE;
                }

            case 'f':
                full_path_flag = true;
                break;

            case 'n':
                non_matches_flag = true;
                break;

            case 's':
                strict_flag = true;
                break;

            case 'R':
                recursive_flag = true;
                break;

            default:
                abort();
        }
    }

    int non_option_argc = argc - optind;
    if (non_option_argc < N_REQUIRED_ARGS) {
        fprintf(stderr, "Insufficient arguments.\nDo `nctool search --help` for more information about usage.\n");
        return EXIT_FAILURE;
    }

    const char *arg_naming_convention = argv[optind];
    char *arg_target_dirname = canonicalize_file_name(argv[optind + 1]);

    // Set to conventions[i].regex if arg_naming_convention is valid, otherwise it remains NULL.
    const char *search_expression = NULL;
    // Set by naming_compile_regex() after search_expression is known to be set.
    regex_t search_regex;

    bool target_is_dir = is_file_valid(arg_target_dirname) &&
                         is_file_dir(arg_target_dirname);

    if ((naming_set_expression(arg_naming_convention, &search_expression, strict_flag)) &&
        (target_is_dir) &&
        (naming_compile_regex(&search_regex, search_expression)))
    {
        initial_search_path = arg_target_dirname;
        traverse_directory(arg_target_dirname, &search_regex, recursive_flag,
                           process_current_file);
        free(arg_target_dirname);
        regfree(&search_regex);
        return EXIT_SUCCESS;
    }
    if (arg_target_dirname != NULL && !target_is_dir) {
        fprintf(stderr, "Error: '%s' is not a directory.\n", arg_target_dirname);
    }
    free(arg_target_dirname);
    return EXIT_FAILURE;
}
