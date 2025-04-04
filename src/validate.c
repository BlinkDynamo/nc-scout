/**********************************************************************************************
*
*   validate - nc-scout cli input validation and prompting. 
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#include "validate.h"

bool validate_file_exists (const char *filepath)
{
    // Check if the file doesn't exist.
    if (access(filepath, F_OK) != 0) {
        printf("Error: '%s' does not exist.\n", filepath);
        return false;
    }    
    return true;
}

bool validate_file_is_dir (const char *filepath)
{
    // Check if the file is a directory.
    struct stat filepath_stat;
    stat(filepath, &filepath_stat);
    if (!S_ISDIR(filepath_stat.st_mode)) {
        printf("Error: '%s' is not a directory.\n", filepath);
        return false;
    }
    return true;
}
