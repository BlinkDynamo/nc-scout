/**********************************************************************************************
*
*   validate - nc-scout cli input validation and prompting. 
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

#include <stdio.h>
#include <sys/stat.h>

#include "validate.h"

bool is_file_valid (const char *filepath)
{
    // Check if canonicalize_file_name() returned NULL (file doesn't exist or path is invalid).
    if (filepath == NULL) {
        fprintf(stderr, "Error: file does not exist or path is invalid.\n");
        return false;
    }
    return true;
}

bool is_file_dir (const char *filepath)
{
    struct stat filepath_stat;
    if (stat(filepath, &filepath_stat) != 0) {
        return false;
    }
    return S_ISDIR(filepath_stat.st_mode);
}
