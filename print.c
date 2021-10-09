/*
 * Contains print helper functions for ls.c
*/

#include <sys/stat.h>

#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "print.h"


/* flags  */
extern int A_allentries;        /* Includes all files but current/previous directory  */
extern int a_allentries;        /* Includes hidden files  */
extern int c_lastchanged;       /* Shows last changed  */
extern int d_directories;       /* Shows directories as plain files */
extern int F_specialsymbols;    /* Adds symbols to special files */
extern int f_unsorted;          /* Files are unsorted */
extern int h_humanreadable;     /* Shows sizes in readable format */
extern int i_inodes;            /* Shows file inodes */
extern int k_kilobytes;         /* Shows size in kilobytes*/
extern int l_longformat;        /* Shows long format */                                   
extern int n_numericalids;      /* Shows IDs numerically */                               
extern int q_forcenonprintable; /* Forces nonprintable characters */                      
extern int R_recurse;           /* Recurses into directories */                           
extern int r_reverseorder;      /* Reverses output*/                                      
extern int S_sizesorted;        /* Sorts by size */                                       
extern int s_systemblocks;      /* Shows number of blocks used  */                        
extern int t_modifiedsorted;    /* Sort by time modified */                               
extern int u_lastaccess;        /* Shows time of last access  */                          
extern int w_forcenonprintable; /* Forces raw nonprintable characters  */ 
extern int BLOCKSIZE;

#define CEIL(w, x, y) (1 + (w - 1)/(x / y))

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
	float blocks;
	blocks = sb.st_blocks;
	
	if (!h_humanreadable) {
		blocks = CEIL(blocks, BLOCKSIZE, 512);
		printf("total %f\n", blocks);
		return;	
	}
	blocks = sb.st_size;
	if ((humanize_number()	
}


void
formatblocks(int blocksum) {
	if (!h_humanreadable) {
		if (blocksum < BLOCKSIZE / 512) {
			blocksum = 1;
		} else {
			blocksum = CEIL(blocksum, BLOCKSIZE, 512);
		}
		printf("total %d\n", blocksum);
		return;
	} 
	if (blocksum < KILOBYTE) {
		printf("total %dB\n", blocksum);
	} else if (blocksum < MEGABYTE) {
		blocksum /= KILOBYTE;
		printf("total %dK\n", blocksum);
	} else if (blocksum < GIGABYTE) {
		blocksum /= MEGABYTE;
		printf("total %dM\n", blocksum);
	} else {
		blocksum /= GIGABYTE;
		printf("total %dG\n", blocksum);
	}
}

