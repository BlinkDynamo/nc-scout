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
*   ---------------------- STRUCTURE OF REGULAR EXPRESSION DEFINITIONS -----------------------
* 
*   Each regular expression is made up of three sections:
*  
*   1. A non-capturing group at the beginning to allow for dotfiles (ie .local/):
*      ^\\.?
*
*   2. A middle body that enforces the required naming convention:
*      [a-z0-9]+   'flatcase' in this example.
*
*   3. A non-capturing group at the end that allows for the file extention (ie foo.txt):
*      (\\.[A-Za-z0-9]+?)
*   
*   ------------------------------- STRICT REGULAR EXPRESSIONS -------------------------------
*
*   Strict regular expressions for naming conventions. The expression must follow the main
*   defining pattern throughout the name.
*
*   Correct: "camelCase" is a correct strict camelcase name as it follows the defining pattern
*            throughout it's name. 
* 
*   Incorrect: "camel" is an incorrect strict camelcase name because even though you could add
*              a "Case", it does not exhibit the main defining pattern (the camel hump).
*
*   ------------------------------ LENIENT REGULAR EXPRESSIONS -------------------------------
*
*   Lenient regular expressions for naming conventions. The expression must be able to
*   be made correct to the main defining pattern if more text is added, but not removed. 
*   Some defines may be the same as the strict version. 
*
*   Correct: "kebab" is a correct lenient kebabcase name as a "-case" could be added to
*            make it a correct strict kebabcase ("kebab-case").
*
*   Incorrect: "Kebab" is an incorrect lenient kebabcase name because to make it valid
*              you must change the "K" to a "k", as well as add a "-case".
* 
********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <regex.h>

#include "naming.h"

#define EXPR_FLATCASE_STRICT        "^\\.?[a-z0-9]+(\\.[A-Za-z0-9]+)?$"
#define EXPR_CAMELCASE_STRICT       "^\\.?[a-z]+([A-Z][a-z0-9]+)+(\\.[A-Za-z0-9]+)?$"
#define EXPR_PASCALCASE_STRICT      "^\\.?([A-Z][a-z0-9]+)+(\\.[A-Za-z0-9]+)?$"
#define EXPR_SNAKECASE_STRICT       "^\\.?[a-z0-9]+(_[a-z0-9]+)+(\\.[A-Za-z0-9]+)?$"
#define EXPR_CONSTANTCASE_STRICT    "^\\.?[A-Z0-9]+(_[A-Z0-9]+)+(\\.[A-Za-z0-9]+)?$"
#define EXPR_KEBABCASE_STRICT       "^\\.?[a-z0-9]+(-[a-z0-9]+)+(\\.[A-Za-z0-9]+)?$"
#define EXPR_COBOLCASE_STRICT       "^\\.?[A-Z0-9]+(-[A-Z0-9]+)+(\\.[A-Za-z0-9]+)?$"

#define EXPR_FLATCASE_LENIENT       EXPR_FLATCASE_STRICT
#define EXPR_CAMELCASE_LENIENT      "^\\.?[a-z]+([A-Z][a-z0-9]+)*(\\.[A-Za-z0-9]+)?$"
#define EXPR_PASCALCASE_LENIENT     EXPR_PASCALCASE_STRICT
#define EXPR_SNAKECASE_LENIENT      "^\\.?[a-z0-9]+(_[a-z0-9]+)*(\\.[A-Za-z0-9]+)?$"
#define EXPR_CONSTANTCASE_LENIENT   "^\\.?[A-Z0-9]+(_[A-Z0-9]+)*(\\.[A-Za-z0-9]+)?$"
#define EXPR_KEBABCASE_LENIENT      "^\\.?[a-z0-9]+(-[a-z0-9]+)*(\\.[A-Za-z0-9]+)?$"
#define EXPR_COBOLCASE_LENIENT      "^\\.?[A-Z0-9]+(-[A-Z0-9]+)*(\\.[A-Za-z0-9]+)?$"

struct Convention Conventions[] = {
    {"flatcase", EXPR_FLATCASE_STRICT, EXPR_FLATCASE_LENIENT}, 
    {"camelcase", EXPR_CAMELCASE_STRICT, EXPR_CAMELCASE_LENIENT},
    {"pascalcase", EXPR_PASCALCASE_STRICT, EXPR_PASCALCASE_LENIENT},
    {"snakecase", EXPR_SNAKECASE_STRICT, EXPR_SNAKECASE_LENIENT},
    {"constantcase", EXPR_CONSTANTCASE_STRICT, EXPR_CONSTANTCASE_LENIENT},
    {"kebabcase", EXPR_KEBABCASE_STRICT, EXPR_KEBABCASE_LENIENT},
    {"cobolcase", EXPR_COBOLCASE_STRICT, EXPR_COBOLCASE_LENIENT},
};

const int n_members_in_Conventions = (sizeof(Conventions) / sizeof(struct Convention));

// Attempts to match arg_naming_convention against every Conventions[i].name in Conventions[].
// If a match is found, dereference the pointer ptr_search_expression points to, set it to
// Conventions[i].expr_lenient (the default) unless strict_flag is true, in which case set it
// to Conventions[i].expr_strict. Return true. If no match is found, return false and keep 
// search_expression NULL.
bool naming_set_expression (const char *arg_naming_convention, const char **ptr_search_expression, 
                            bool strict_flag)
{
    for (int i = 0; i < n_members_in_Conventions; i++) {
        if (strcmp(arg_naming_convention, Conventions[i].name) == 0) {
            if (strict_flag == true) {
                *ptr_search_expression = Conventions[i].expr_strict;
            }
            else {
                *ptr_search_expression = Conventions[i].expr_lenient;
            }
            return true;
        }
    }
    printf("Error: '%s' is not a valid naming convention\n", arg_naming_convention);
    return false;
}

// Attempts to compile a pattern to a regex pointed to by *regex, returning true if successful.
bool naming_compile_regex (regex_t *regex, const char *expression)
{
    if (regcomp(regex, expression, REG_EXTENDED) != 0) {
        printf("Error: Failed to compile regex.\n");
        return false; 
    }
    return true;
}

// Attempts to match a regex to a filename, returning true if a match.
bool naming_match_regex (regex_t regex, const char *filename)
{
    int regex_return;
    regex_return = regexec(&regex, filename, 0, NULL, 0);
    
    if (regex_return == 0) {
        return true;
    }
    return false;
}
