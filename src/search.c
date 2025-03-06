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
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <linux/limits.h>

#include "help.h"
#include "validate.h"
#include "naming.h"
#include "search.h"

#define N_REQUIRED_ARGS 2

// Flags.
bool full_path_flag = false;
bool matches_flag   = false;
bool strict_flag    = false;
bool recursive_flag = false;

const char *abs_initial_search_path = NULL;

// Returns the difference of an absolute initial path and an absolute current path.
const char *get_relative_path (const char *abs_initial_path, const char *abs_current_path)
{
    while (*abs_initial_path && *abs_current_path && *abs_initial_path == *abs_current_path)
    {
        abs_initial_path++;
        abs_current_path++;
    }
    return abs_current_path;
}

// Compares a d_name to a regular expression. Will print matches or non-matches depending on matches_flag.
void process_current_file (struct dirent *dp, char abs_current_file_path[PATH_MAX],
                           const char *abs_initial_search_path, regex_t regex)
{
    // Matches:
    if (matches_flag == true) {
        if (naming_match_regex(regex, dp->d_name)) { 
            // Matches with full path. 
            if (full_path_flag == true) {
                printf("%s\n", abs_current_file_path);
            }
            // Matches with relative path.
            else if (full_path_flag == false) {
                printf("%s\n", get_relative_path(abs_initial_search_path, abs_current_file_path));
            }
        }
    }
    // Non-matches:
    else {
        if (!naming_match_regex(regex, dp->d_name)) {
            // Non-matches with full path. 
            if (full_path_flag == true) {
                printf("%s\n", abs_current_file_path);
            }
            // Non-matches with relative path.
            else if (full_path_flag == false) {
                printf("%s\n", get_relative_path(abs_initial_search_path, abs_current_file_path));
            }
        }
    }
}

// Search a const char *search_path for matches to a regex_t regex.
void search_directory (const char *search_path, regex_t regex)
{
    const char *abs_search_path = canonicalize_file_name(search_path);
    
    // If this is the first time search_directory has been called, save the absolute initial search path.
    if (abs_initial_search_path == NULL) {
        abs_initial_search_path = abs_search_path;
    }

    // dir_path is known to exist at this point, but opendir() can still fail from permissions.
    DIR *current_dir = opendir(abs_search_path);
    if (current_dir == NULL) {
        printf("Error: cannot access %s due to Error %d (%s).\n", abs_search_path, errno, strerror(errno));
        return;
    }
   
    // Begin reading directories/files inside current_dir...
    struct dirent *dp;
    while ((dp = readdir(current_dir)) != NULL)
    {
        // Skip current and parent entries.
        if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0) {
            continue;
        }

        // 'abs_current_file_path' is a concatenation of abs_search_path and dp->d_name.
        char abs_current_file_path[PATH_MAX];
        snprintf(abs_current_file_path, sizeof(abs_current_file_path), "%s/%s", abs_search_path, dp->d_name);
        // Determine file type and perform actions accordingly.
        struct stat statbuf;
        if (lstat(abs_current_file_path, &statbuf) != 0) {
            printf("Error: lstat failed due to Error %d (%s).\n", errno, strerror(errno));
            return;
        }

        // If the current file is a directory...
        if (S_ISDIR(statbuf.st_mode)) {
            process_current_file(dp, abs_current_file_path, abs_initial_search_path, regex);
            // Recurse each subdirectory if --recursive is set.
            if (recursive_flag == true) {
                search_directory(abs_current_file_path, regex);
            }
        }
        // Else if the current file is a regular file...
        else if (S_ISREG(statbuf.st_mode)) { 
            process_current_file(dp, abs_current_file_path, abs_initial_search_path, regex);
        }
    }
    closedir(current_dir);
}

// The subcommand entry function called from src/nc-scout.c. argc and argv have "nc-scout" stripped.
int subc_exec_search (int argc, char *argv[])
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
        (validate_target_dirname_exists(arg_target_dirname)) && 
        (naming_compile_regex(&search_regex, search_expression)))
    {     
        search_directory(arg_target_dirname, search_regex); 
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
