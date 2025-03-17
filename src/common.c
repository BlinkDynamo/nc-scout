/**********************************************************************************************
*
*   common - Contains general functions that are commonly used throughout the project.
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

// Returns the difference of an absolute initial path and an absolute current path.
const char *get_relative_path (const char *abs_initial_path, const char *abs_current_path)
{
    while (*abs_initial_path && *abs_current_path && *abs_initial_path == *abs_current_path)
    {
        abs_initial_path++;
        abs_current_path++;
    }
    // If abs_current_path starts with a '/', start the string one after that, otherwise return it unchanged.
    return (*abs_current_path == '/') ? abs_current_path + 1 : abs_current_path;
}

// Returns a float of num_a / num_b, unless num_b is 0, in which case it returns 0.0.
double percentage(int num_a, int num_b) {
    return (num_b) == 0 ? 0.0 : (num_a * 100.0) / (num_b); 
}
