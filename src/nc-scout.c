/**********************************************************************************************
*
*   nc-scout - A simple naming convention tool.
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
#include <stdlib.h>
#include <string.h>

#include "search.h"
#include "version.h"
#include "help.h"

int builtin_exec_help (int argc)
{
    // Input must be either nc-scout -h or nc-scout --help exactly.
    if (argc == 2) {
        printf("%s", HELP_DEFAULT);
        return EXIT_SUCCESS;
    }
    else {
        printf("Incorrect usage.\nDo `nc-scout --help` for more information about usage.\n");
        return EXIT_FAILURE;
    }
}

int builtin_exec_version (int argc)
{   
    // Input must be either nc-scout -v or nc-scout --version exactly.
    if (argc == 2) {
        printf("%s %s\n", PROGRAM_NAME, PROGRAM_VERSION);
        return EXIT_SUCCESS;
    }
    else {
        printf("Incorrect usage.\nDo `nc-scout --help` for more information about usage.\n");
        return EXIT_FAILURE;
    }
}

int main (int argc, char *argv[]) 
{   
    // Check for no supplied arguments (ie: just `nc-scout`).
    if (argc < 2) {
        printf("No arguments supplied.\nDo `nc-scout --help` for usage information.\n");
        return EXIT_FAILURE;
    }

    struct Builtin 
    {   
        const char *name;
        int (*execute)(int argc); // Pointer to the function that executes a builtin.
    };

    struct Subcommand 
    {   
        const char *name;
        int (*execute)(int argc, char *argv[]); // Pointer to the function that executes a subcommand.
    };
   
    struct Builtin Builtins[] = 
    {
        {"--help", builtin_exec_help},
        {"-h", builtin_exec_help},
        {"--version", builtin_exec_version},
        {"-v", builtin_exec_version},
    };

    struct Subcommand Subcommands[] = 
    {
        {"search", subc_exec_search}
    };

    // Get the number of Builtins and Subcommands.
    int n_builtins = sizeof(Builtins) / sizeof(Builtins[0]);
    int n_subcommands = sizeof(Subcommands) / sizeof(Subcommands[0]);

    // Iterate through Builtins and Subcommands, looking for names that match argv[1].
    // Search through Builtins first, then Subcommands. Execute the first match that is found.
    for (int i = 0; i < n_builtins; i++)
    {
        if (strcmp(argv[1], Builtins[i].name) == 0) {
            return Builtins[i].execute(argc);
        }
    }
    for (int i = 0; i < n_subcommands; i++)
    {
        if (strcmp(argv[1], Subcommands[i].name) == 0) {
            // -1 and 1 to strip "nc-scout" from the input.
            return Subcommands[i].execute(argc - 1, &argv[1]);
        }
    }
    // If this point is reached, no valid subcommand was found.
    printf("Error: Unknown command `%s`.\n", argv[1]);
    printf("Do `nc-scout --help` for usage information.\n");
    return EXIT_FAILURE;
}
