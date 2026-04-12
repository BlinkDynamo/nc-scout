/**********************************************************************************************
*
*   analyze - Handles options, validation, and runtime of the nctool analyze subcommand.
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
#include <getopt.h>

#include "help.h"
#include "validate.h"
#include "common.h"
#include "naming.h"
#include "analyze.h"

#define N_REQUIRED_ARGS 2

// Flags.
static bool strict_flag    = false;
static bool recursive_flag = false;

static int matches = 0;
static int non_matches = 0;

static void analyze_callback (struct dirent *entry, const char *dir_path,
                              const regex_t *regex)
{
    (void)dir_path;
    if (naming_match_regex(regex, entry->d_name)) {
        matches++;
    } else {
        non_matches++;
    }
}

int subc_exec_analyze (int argc, char *argv[])
/**********************************************************************************************
*
*   The external function called from src/main.c. Processes options, compiles the regex, does
*   validation, and finally calls analyze_directory.
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
    matches = 0;
    non_matches = 0;
    strict_flag = false;
    recursive_flag = false;
    optind = 1;

    int current_opt;

    while (1)
    {
        static struct option long_options_analyze[] =
        {
            {"help", no_argument, 0, 'h'},
            {"strict", no_argument, 0, 's'},
            {"recursive", no_argument, 0, 'R'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        current_opt = getopt_long (argc, argv, "+hsR", long_options_analyze, &option_index);
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
                    printf("%s", HELP_ANALYZE);
                    return EXIT_SUCCESS;
                }
                else {
                    fprintf(stderr, "Incorrect usage.\nDo `nctool analyze --help` for more information about usage.\n");
                    return EXIT_FAILURE;
                }

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
        fprintf(stderr, "Insufficient arguments.\nDo `nctool analyze --help` for more information about usage.\n");
        return EXIT_FAILURE;
    }

    const char *arg_naming_convention = argv[optind];
    char *arg_target_dirname = canonicalize_file_name(argv[optind + 1]);

    // Set to Conventions[i].regex if arg_naming_convention is valid, otherwise it remains NULL.
    const char *analyze_expression = NULL;
    // Set by naming_compile_regex() after analyze_expression is known to be set.
    regex_t analyze_regex;

    bool target_is_dir = is_file_valid(arg_target_dirname) &&
                         is_file_dir(arg_target_dirname);

    if ((naming_set_expression(arg_naming_convention, &analyze_expression, strict_flag)) &&
        (target_is_dir) &&
        (naming_compile_regex(&analyze_regex, analyze_expression)))
    {
        traverse_directory(arg_target_dirname, &analyze_regex, recursive_flag,
                           analyze_callback);
        printf("Analyzed the presence of %s %s files and directories in '%s'.\n\n",
                (strict_flag) ? "strictly" : "leniently",
                arg_naming_convention,
                arg_target_dirname);

        printf("%s:         %d\n", arg_naming_convention, matches);
        printf("non-%s:     %d\n\n", arg_naming_convention, non_matches);
        printf("%s %s files and directories make up %0.3f%% of '%s'.\n",
                (strict_flag) ? "strictly" : "leniently",
                arg_naming_convention,
                percentage(matches, matches + non_matches),
                arg_target_dirname);

        free(arg_target_dirname);
        regfree(&analyze_regex);

        return EXIT_SUCCESS;
    }
    if (arg_target_dirname != NULL && !target_is_dir) {
        fprintf(stderr, "Error: '%s' is not a directory.\n", arg_target_dirname);
    }
    free(arg_target_dirname);
    return EXIT_FAILURE;
}
