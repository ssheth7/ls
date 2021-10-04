/*
 * Contains print helper functions for ls.c
*/

#include <sys/stat.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"

void
printdefault(char* entry)
{
	if (entry[0] != '.') {
		printf("%s\n", entry);
	}
}

void
printall_A(char* entry) 
{
	printf("%s\n", entry);
}

void
printall_a(char* entry)
{
	printf("%s\n", entry);
}

void
addsymbols_F(char** entry, struct stat sb)
{
	int entrymode;
	char appended;
	char* modifiedentry;
	
	if ((modifiedentry = malloc(strlen(*entry) + 2)) == NULL) {
		fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
	}
	
	entrymode = sb.st_mode;
			
	if (S_ISDIR(entrymode)) {
		appended = '/';
	} else if (S_ISLNK(entrymode)) {
		appended = '@';
	} else if (S_ISSOCK(entrymode)) {
		appended = '=';
	} else if(S_ISFIFO(entrymode)) {
		appended = '|';
	} else if (S_IEXEC & entrymode) {
		appended = '*';
	} else if (S_ISWHT(entrymode)) {
		printf("%d ", S_IFWHT * entrymode);
		appended = '%';
	} else {
		free(modifiedentry);
		return;
	}
	snprintf(modifiedentry, strlen(*entry) + 2, "%s%c", *entry, appended);
	*entry = modifiedentry;
}
