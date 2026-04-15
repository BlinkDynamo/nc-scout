/**********************************************************************************************
*
*   common - Contains general functions that are commonly used throughout the project.
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
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <regex.h>

#include "common.h"
#include "validate.h"

void traverse_directory (const char *dir_path, const regex_t *regex, bool recursive,
                         traverse_callback callback)
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

        if (is_dir) {
            callback(current_file, dir_path, regex);
            if (recursive) {
                traverse_directory(full_path, regex, recursive, callback);
            }
        }
        else if (current_file->d_type == DT_REG ||
                 current_file->d_type == DT_LNK ||
                 current_file->d_type == DT_UNKNOWN) {
            callback(current_file, dir_path, regex);
        }
    }
    closedir(current_dir);
}

// Returns the difference of an absolute initial path and an absolute current path.
const char *get_relative_path (const char *abs_initial_path, const char *abs_current_path)
{
    const char *last_slash = abs_current_path;

    while (*abs_initial_path && *abs_current_path && *abs_initial_path == *abs_current_path)
    {
        if (*abs_current_path == '/') {
            last_slash = abs_current_path;
        }
        abs_initial_path++;
        abs_current_path++;
    }

    // If paths diverged mid-component, backtrack to the last '/' boundary.
    if (*abs_initial_path && *abs_initial_path != '/') {
        abs_current_path = last_slash;
    }

    return (*abs_current_path == '/') ? abs_current_path + 1 : abs_current_path;
}

// Returns a float of num_a / num_b, unless num_b is 0, in which case it returns 0.0.
double percentage(int num_a, int num_b) {
    return (num_b) == 0 ? 0.0 : (num_a * 100.0) / (num_b); 
}
