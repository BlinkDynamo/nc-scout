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

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "help.h"
#include "validate.h"
#include "common.h"
#include "naming.h"
#include "analyze.h"

#define N_REQUIRED_ARGS 1

// Flags.
static bool strict_flag = false;
static bool recursive_flag = false;

void analyze (const char *dir_path, int *matches, const regex_t *regexes, bool recursive)
{	
    DIR *current_dir = opendir(dir_path);
    if (current_dir == NULL) {
        fprintf(
			stderr,
			"Error: cannot access %s due to Error %d (%s).\n",
            dir_path, errno, strerror(errno)
		);
        return;
    }

    struct dirent *current_file;
    while ((current_file = readdir(current_dir)) != NULL)
    {
        if (strcmp(current_file->d_name, ".") == 0 || strcmp(current_file->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, current_file->d_name);

        bool is_dir = (current_file->d_type == DT_DIR) ||
                      (current_file->d_type == DT_UNKNOWN && is_file_dir(full_path));

		int file_conventions = 0;
		for (int i = 0; i < n_members_in_Conventions; i++) {
			if (regexec(&regexes[i], current_file->d_name, 0, NULL, 0) == 0) {
				matches[i]++;
				file_conventions++;
			}
		}
		// Increment total.
		matches[n_members_in_Conventions]++;

		// Increment other.
		if (file_conventions == 0) {
			matches[n_members_in_Conventions + 1]++;
		}

        if (is_dir && recursive) {
            analyze(full_path, matches, regexes, recursive);
        } 
    }
    closedir(current_dir);
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

    char *arg_target_dirname = canonicalize_file_name(argv[optind]);
    bool target_is_dir = is_file_valid(arg_target_dirname) && is_file_dir(arg_target_dirname);
	if (arg_target_dirname != NULL && !target_is_dir) {
        fprintf(stderr, "Error: '%s' is not a directory.\n", arg_target_dirname);
		return EXIT_FAILURE;
    }

    regex_t regexes[n_members_in_Conventions];
	for (int i = 0; i < n_members_in_Conventions; i++) {
		const char *expression;
		if (strict_flag) {
			expression = Conventions[i].expr_strict;
		} else {
			expression = Conventions[i].expr_lenient;
		}
		regcomp(&regexes[i], expression, REG_EXTENDED);
	}

	// The number of matching files for each convention in 'Conventions'. The last index is the
	// total number of files.
	int matches[n_members_in_Conventions + 2];
	memset(matches, 0, sizeof(matches));

	analyze(arg_target_dirname, matches, regexes, recursive_flag);

	// Free all allocated sections.
	free(arg_target_dirname);
	for (int i = 0; i < n_members_in_Conventions; i++) {	
		regfree(&regexes[i]);
	}
	
	// Print the formatted output.
	const int total_padding = 20;
	const int number_padding = 7;

	const int total_files = matches[n_members_in_Conventions];
	int other_files = matches[n_members_in_Conventions + 1];
	for (int i = 0; i < n_members_in_Conventions; i++) {	
		const char *convention_name = Conventions[i].name;
		const int convention_matches = matches[i];

		printf("%-*s %*d\n", total_padding, convention_name, number_padding, convention_matches);
	}
	printf("%-*s %*d\n\n", total_padding, "other", number_padding, other_files);
	printf("%-*s %*d\n", total_padding, "total", number_padding, total_files);

	return EXIT_SUCCESS; 
}
