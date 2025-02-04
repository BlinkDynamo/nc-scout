/*
 * @file	validate.c
 * @author	Josh Hayden
 * @license	GPL-3.0
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "validate.h"

bool validate_target_dirname_exists(const char *target_dirname)
{
	if (access(target_dirname, F_OK) != 0) {
		fprintf(stderr, "Error: Directory '%s' does not exist.\n", target_dirname);
		return false;
	}
	return true;
}

bool validate_arg_naming_convention(const char *arg_naming_convention)
{
	if (
	(strcmp("camelcase", arg_naming_convention) == 0) ||
	(strcmp("snakecase", arg_naming_convention) == 0) ||
	(strcmp("kebabcase", arg_naming_convention) == 0)
	)
		return true;
	else {
		fprintf(stderr, "Error: '%s' is not a valid naming convention\n", arg_naming_convention);
		return false;
	}
}
