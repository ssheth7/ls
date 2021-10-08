/*
 * Contains print helper functions for ls.c
*/

#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"

extern int s_systemblocks;
extern int h_humanreadable;
extern int k_kilobytes;

extern int BLOCKSIZE;

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
	} else if (S_ISWHT(entrymode)) {
		appended = '%';
	} else if (S_IEXEC & entrymode) {
		appended = '*';
	} else {
		free(modifiedentry);
		return;
	}
	snprintf(modifiedentry, strlen(*entry) + 2, "%s%c", *entry, appended);
	*entry = modifiedentry;
	free(modifiedentry);
}

void
printdefault(char* entry)
{
	printf("%s\n", entry);
}

void
printraw_q(char* entry)
{
	int index;

	index = 0;

	while(entry[index]) {
		if (isprint(entry[index])) {
			printf("%c", entry[index]);
		} else {
			printf("?");
		}
		index++;
	}
	printf("\n");
}

void
printraw_w(char* entry)
{
	int index;

	index = 0;
	while(entry[index]) {
		if (isprint(entry[index])) {
			printf("%c", entry[index]);
		} else {
			printf("%c", entry[index]);
		}
		index++;
	}
	printf("\n");
}

void
printinode_i(struct stat sb) 
{
	printf("%ld ", sb.st_ino);
}

void
printblocks_s(struct stat sb)
{
	int blocks, adjustedblocks;
	blocks = sb.st_blocks;
	adjustedblocks = blocks / (BLOCKSIZE / 512);
	if (adjustedblocks == 0 && blocks > 0) {
		adjustedblocks = 1;
	}
	if (!h_humanreadable) {
		printf("%d ", adjustedblocks);	
	} else {
		printf("%d ", blocks / 2);		
	}
}
