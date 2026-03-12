/**********************************************************************************************
*
*   search - Handles options, validation, and runtime of the nc-scout search subcommand.
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
#include <dirent.h>
#include <getopt.h>
#include <errno.h>
#include <limits.h>

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
                           const char *initial_search_path, const regex_t *regex)
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
*   initial_search_path         The absolute path to the dir where search was first called.
*
*   regex                       The compiled regex of the convention being searched for.
*
**********************************************************************************************/
{
    bool should_print = (!non_matches_flag && naming_match_regex(regex, current_file->d_name)) ||
                        (non_matches_flag && !naming_match_regex(regex, current_file->d_name));

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

static void search_directory (const char *search_path, const regex_t *regex)
/**********************************************************************************************
*
*   Searches a directory for filenames that match a regular expression.
*
*   ---------------------------------------- ARGUMENTS ----------------------------------------
*
*   search_path     The directory where the search will take place.
*
*   regex           The compiled regex of the convention being searched for.
*
**********************************************************************************************/
{
    // If this is the first time search_directory has been called, save the absolute initial search path.
    if (initial_search_path == NULL) {
        initial_search_path = search_path;
    }

    // dir_path is known to exist at this point, but opendir() can still fail from permissions.
    DIR *current_dir = opendir(search_path);
    if (current_dir == NULL) {
        fprintf(stderr, "Error: cannot access %s due to Error %d (%s).\n", search_path,
                errno, strerror(errno));
        return;
    }
   
    // Begin reading directories/files inside current_dir.
    struct dirent *current_file;
    while ((current_file = readdir(current_dir)) != NULL)
    {
        // Skip current and parent entries.
        if (strcmp(current_file->d_name, ".") == 0 || strcmp(current_file->d_name, "..") == 0) {
            continue;
        }

        // Build full path for stat() fallback when d_type is DT_UNKNOWN.
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", search_path, current_file->d_name);

        // Determine if current entry is a directory (with fallback for DT_UNKNOWN).
        bool is_dir = (current_file->d_type == DT_DIR) ||
                      (current_file->d_type == DT_UNKNOWN && is_file_dir(full_path));

        // If the current file is a directory...
        if (is_dir) {

            // Process it.
            process_current_file(current_file, search_path, initial_search_path, regex);

            // Then if recursive_flag is true, call search_directory at that location.
            if (recursive_flag == true) {
                search_directory(full_path, regex);
            }
        }
        // Else if the current file is a regular file or a symlink...
        else if ((current_file->d_type == DT_REG) || (current_file->d_type == DT_LNK) ||
                 (current_file->d_type == DT_UNKNOWN)) {

            // Process it.
            process_current_file(current_file, search_path, initial_search_path, regex);
        }
    }
    closedir(current_dir);
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
*   argv    The argv of main() in src/main.c with "nc-scout" removed from the beginning.
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
                    fprintf(stderr, "Incorrect usage.\nDo `nc-scout search --help` for more information about usage.\n");
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
        fprintf(stderr, "Insufficient arguments.\nDo `nc-scout search --help` for more information about usage.\n");
        return EXIT_FAILURE;
    }

    const char *arg_naming_convention = argv[optind];
    char *arg_target_dirname = canonicalize_file_name(argv[optind + 1]);

    // Set to Conventions[i].regex if arg_naming_convention is valid, otherwise it remains NULL.
    const char *search_expression = NULL;
    // Set by naming_compile_regex() after search_expression is known to be set.
    regex_t search_regex;

    if ((naming_set_expression(arg_naming_convention, &search_expression, strict_flag)) &&
        (is_file_valid(arg_target_dirname)) &&
        (is_file_dir(arg_target_dirname)) &&
        (naming_compile_regex(&search_regex, search_expression)))
    {
        search_directory(arg_target_dirname, &search_regex);
        free(arg_target_dirname);
        regfree(&search_regex);
        return EXIT_SUCCESS;
    }
    if (arg_target_dirname != NULL && !is_file_dir(arg_target_dirname)) {
        fprintf(stderr, "Error: '%s' is not a directory.\n", arg_target_dirname);
    }
    free(arg_target_dirname);
    return EXIT_FAILURE;
}
