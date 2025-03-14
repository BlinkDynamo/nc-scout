/**********************************************************************************************
*
*   search - Handles options, validation, and runtime of the nc-scout search subcommand.
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2024-2025 Josh Hayden (@BlinkDynamo)
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
#include <linux/limits.h>

#include "help.h"
#include "validate.h"
#include "naming.h"
#include "search.h"

#define N_REQUIRED_ARGS 2

// Flags.
static bool full_path_flag = false;
static bool matches_flag   = false;
static bool strict_flag    = false;
static bool recursive_flag = false;

static const char *abs_initial_search_path = NULL;

// Returns the difference of an absolute initial path and an absolute current path.
static const char *get_relative_path (const char *abs_initial_path, const char *abs_current_path)
{
    while (*abs_initial_path && *abs_current_path && *abs_initial_path == *abs_current_path)
    {
        abs_initial_path++;
        abs_current_path++;
    }
    // If abs_current_path starts with a '/', start the string one after that, otherwise return it unchanged.
    return (*abs_current_path == '/') ? abs_current_path + 1 : abs_current_path;
}

static void process_current_file (struct dirent *current_file, const char *abs_search_path,
                           const char *abs_initial_search_path, regex_t regex)
/**********************************************************************************************
*
*   Compares a d_name to a regular expression. Will print matches or non-matches depending on 
*   matches_flag.
*
*   ---------------------------------------- ARGUMENTS ----------------------------------------
*
*   current_file                The pointer to the current file being processed.
*
*   abs_search_path             The absolute path of the dir current_file is searching in.
*
*   abs_initial_search_path     The absolute path to the dir where search was first called.
*
*   regex                       The compiled regex of the convention being searched for.
*
**********************************************************************************************/
{
    bool should_print = (matches_flag && naming_match_regex(regex, current_file->d_name)) ||
                        (!matches_flag && !naming_match_regex(regex, current_file->d_name));

    if (should_print)
    {
        if (full_path_flag)
            {
                printf("%s/%s\n", abs_search_path, current_file->d_name);
            }
        else
        {
        // If relative_path is empty (current directory), insert a forward-slash between
        // relative_path and current_file->d_name.
        const char *relative_path = get_relative_path(abs_initial_search_path, abs_search_path);
        printf("%s%s%s\n",
               relative_path,
               (strlen(relative_path) > 0) ? "/" : "",
               current_file->d_name);
        }
    }
}

static void search_directory (const char *search_path, regex_t regex)
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
    const char *abs_search_path = canonicalize_file_name(search_path);
    
    // If this is the first time search_directory has been called, save the absolute initial search path.
    if (abs_initial_search_path == NULL) {
        abs_initial_search_path = abs_search_path;
    }

    // dir_path is known to exist at this point, but opendir() can still fail from permissions.
    DIR *current_dir = opendir(abs_search_path);
    if (current_dir == NULL) {
        printf("Error: cannot access %s due to Error %d (%s).\n", abs_search_path, errno, 
                strerror(errno));
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

        // If the current file is a directory...
        if (current_file->d_type == DT_DIR) {

            // Process it.
            process_current_file(current_file, abs_search_path, abs_initial_search_path, regex);

            // Then if recursive_flag is true, concatenate abs_search_path with current_file->d_name
            // and call search_directory at that location.
            if (recursive_flag == true) {
                char abs_new_search_path[PATH_MAX];
                snprintf(abs_new_search_path, sizeof(abs_new_search_path), "%s/%s", abs_search_path, 
                         current_file->d_name);
                search_directory(abs_new_search_path, regex);
            }
        }
        // Else if the current file is a regular file...
        else if (current_file->d_type == DT_REG) { 

            // Process it.
            process_current_file(current_file, abs_search_path, abs_initial_search_path, regex);
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
    int current_opt;
    while (1)
    {
        static struct option long_options_search[] =
        {
            {"help", no_argument, 0, 'h'},
            {"full-path", no_argument, 0, 'f'}, 
            {"matches", no_argument, 0, 'm'},
            {"strict", no_argument, 0, 's'},
            {"recursive", no_argument, 0, 'R'},
            {0, 0, 0, 0}
        };
        
        int option_index = 0;
        current_opt = getopt_long (argc, argv, "+hfmsR", long_options_search, &option_index);
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
                    printf("Incorrect usage.\nDo `nc-scout search --help` for more information about usage.\n");
                    return EXIT_FAILURE;
                }

            case 'f':
                full_path_flag = true;
                break;

            case 'm':
                matches_flag = true;
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
        printf("Insufficient arguments.\nDo `nc-scout search --help` for more information about usage.\n");
        return EXIT_FAILURE;
    }

    const char *arg_naming_convention = argv[optind];
    const char *arg_target_dirname = argv[optind + 1]; 

    // Set to Conventions[i].regex if arg_naming_convention is valid, otherwise it remains NULL.
    const char *search_expression;
    // Set by naming_compile_regex() after search_expression is known to be set.
    regex_t search_regex;

    if ((naming_set_expression(arg_naming_convention, &search_expression, strict_flag)) &&
        (validate_file_exists(arg_target_dirname)) && 
        (validate_file_is_dir(arg_target_dirname)) && 
        (naming_compile_regex(&search_regex, search_expression)))
    {     
        search_directory(arg_target_dirname, search_regex); 
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
