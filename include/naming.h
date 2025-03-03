/**********************************************************************************************
*
*   naming - naming comparison for nc-scout. 
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

#ifndef NAMING_H
#define NAMING_H

#include <stdbool.h>
#include <regex.h>

struct Convention {
    const char *name;
    const char *expr_strict;
    const char *expr_lenient;
};

bool naming_set_expression (const char *arg_naming_convention, const char **ptr_search_expression, bool strict_flag);

bool naming_compile_regex (regex_t *regex, const char *search_expression);

bool naming_match_regex (regex_t regex, const char *filename);

#endif // NAMING_H
