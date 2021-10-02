/*
 * Contains print helper functions for ls.c
*/

#include <stdio.h>

#include "print.h"

void
printdefault(char* entry)
{
	if (entry[0] != '.') {
		printf("%s\n", entry);
	}
}

