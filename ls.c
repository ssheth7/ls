/*
 */
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ls.h"
#include "helpers.h"
#include "print.h"

/* flags  */
int A_allentries;        /* Includes all files present/previous directory  */
int a_allentries;        /* Includes hidden files  */
int c_lastChanged;       /* Shows last changed  */
int d_directories;       /* Shows directories as plain files */
int F_specialsymbols;    /* Adds symbols to special files */
int f_unsorted;          /* Files are unsorted */
int h_humanreadable;     /* Shows sizes in readable format */
int i_inodes;            /* Shows file inodes */
int k_kilobytes;         /* Shows size in kilobytes*/
int l_longformat;        /* Shows long format */                                   
int n_numericalids;      /* Shows IDs numerically */                               
int q_forcenonprintable; /* Forces nonprintable characters */                      
int R_recurse;           /* Recurses into directories */                           
int r_reverseorder;      /* Reverses output*/                                      
int S_sizesorted;        /* Sorts by size */                                       
int s_systemblocks;      /* Shows number of blocks used  */                        
int t_modifiedsorted;    /* Sort by time modified */                               
int u_lastaccess;        /* Shows time of last access  */                          
int w_forcenonprintable; /* Forces raw nonprintable characters  */ 

int NUMDIRS;
int NUMNONDIRS;
int EXIT_STATUS;
char **NONDIRS;
char **DIRS;


struct fileentry {
	struct stat sb;
	char* entryname;
};

int
parseargs(int argc, char **argv)
{	
	int opt, flagsset;
	
	flagsset = 0;
	while ((opt = getopt(argc, argv, "AacdFfhiklnqRrSstuw")) != -1) {
		flagsset = 1;
		switch (opt) {
			case 'A':
				A_allentries = 1;
				break;
			case 'a':
				a_allentries = 1;
				A_allentries = 0;
				break;
			case 'c':
				if (f_unsorted == 0) {
					break;
				}
				c_lastChanged = 1;
				u_lastaccess = 0;
				break;
			case 'd':
				d_directories = 1;
				break;
			case 'F':
				F_specialsymbols = 1;
				break;
			case 'f':
				f_unsorted = a_allentries = 1;
				r_reverseorder = S_sizesorted = u_lastaccess = c_lastChanged = 0;
				break;
			case 'h':
				h_humanreadable = 1;
				break;
			case 'i':
				i_inodes = 1;
				break;
			case 'k':
				k_kilobytes = 1;
				break;
			case 'l':
				l_longformat = 1;
				n_numericalids = 0;
				break;
			case 'n':
				n_numericalids = 1;
				l_longformat = 0;
				break;
			case 'q':
				q_forcenonprintable = 1;
				w_forcenonprintable = 0;
				break;
			case 'R':
				R_recurse = 1;
				break;
			case 'r':
				if (f_unsorted == 0) {
					break;
				}
				r_reverseorder = 1;
				break;
			case 'S':
				if (f_unsorted == 0) {
					break;
				}
				S_sizesorted = 1;
				break;
			case 's':
				s_systemblocks = 1;
				break;
			case 't':
				if (f_unsorted == 0) {
					break;
				}
				t_modifiedsorted = 1;
				break;
			case 'u':
				if (f_unsorted == 0) {
					break;
				}
				u_lastaccess = 1;
				c_lastChanged = 0;
				break;
			case 'w':
				w_forcenonprintable = 1;
				q_forcenonprintable = 0;
				break;
			default: /* Invalid Argument provided  */
				fprintf(stderr, "%s [-AacdFfhiklnqRrSstuw] [file ...]\n", getprogname());
				exit(EXIT_FAILURE);
		}
	}
	if (getuid() == 0) {
		A_allentries = 1;
	}
	return flagsset;
}

void
splitargs(int argc, char **argv, int offset)
{
	int i, dirindex, nondirindex, arglength;
	struct stat sb;
	dirindex = 0;
	nondirindex = 0;
	
	if (NUMDIRS == 0 && NUMNONDIRS == 0) {
		NUMDIRS++;
		if ((DIRS = malloc(NUMDIRS * sizeof(char*))) == NULL) {
			fprintf(stderr, "Memory could not be allocated");
			exit(EXIT_FAILURE);
		}
		if ((DIRS[0] = malloc(strlen(".") + 1)) == NULL) {
			fprintf(stderr, "Could not allocate memory.\n");
			exit(EXIT_FAILURE);
		}
		if (strncpy(DIRS[0], ".", strlen(".")) == NULL) {
			fprintf(stderr, "Could not copy to destination\n");
			exit(EXIT_FAILURE);
		}
	}
	for (i = 1 + offset; i < argc; i++){
		if (stat(argv[i], &sb) == 0) {
			arglength = strlen(argv[i]);
			if (S_ISDIR(sb.st_mode)) {
				if ((DIRS[dirindex] = malloc(arglength + 1)) == NULL) {
					fprintf(stderr, "Could not allocate memory.\n");
				}
				if (strncpy(DIRS[dirindex], argv[i], arglength) == NULL) {
					fprintf(stderr, "Could not copy %s to buffer.\n", argv[i]);
				}
				dirindex++;
			} else {
				if ((NONDIRS[nondirindex] = malloc(arglength + 1)) == NULL) {
					fprintf(stderr, "Could not allocate memory.\n");
				}
				
				if (strncpy(NONDIRS[nondirindex], argv[i], arglength) == NULL) {
					fprintf(stderr, "Could not copy %s to buffer.\n", argv[i]);
				}
				nondirindex++;
			}
		} else {
			fprintf(stderr, "%s: cannot access %s: No such file or director\n", getprogname(), argv[i]);
			EXIT_STATUS = EXIT_FAILURE;
		}
	}
}

void
formatnondir(char* file, struct stat sb)
{
	if (A_allentries) {
		;
	}
	else {
	printdefault(file);
	}
	if (0) {
		printf("%d", sb.st_mode);
	}
}

void
formatdir(char* dir) 
{
	int fileinfo;
	FTS *directory;
	FTSENT *entry;
	char *dirptr[2];
	
	int (*comparefunction)(const FTSENT **, const FTSENT **);
	
	comparefunction = &fts_lexicosort;
	
	if (r_reverseorder) {
		comparefunction = &fts_rlexicosort;
	}
	
	if (S_sizesorted) {
		comparefunction = &fts_sizesort;
		if (r_reverseorder) {
			comparefunction = &fts_rsizesort;
		}
	}

	if (t_modifiedsorted) {
		comparefunction = &fts_timemodifiedsort;
		if (r_reverseorder) {
			comparefunction = &fts_rtimemodifiedsort;
		}
	}
	
	if (u_lastaccess) {
		comparefunction = &fts_lastaccesssort;
		if (r_reverseorder) {
			comparefunction = &fts_rlastaccesssort;
		}
	}
	if (f_unsorted) {
		comparefunction = NULL;	
	}
	
	dirptr[0] = dir;
	dirptr[1] = NULL;
	
	if ((directory = fts_open(dirptr, FTS_COMFOLLOW | FTS_SKIP, comparefunction)) == NULL) {
		fprintf(stderr, "Cannot open %s.\n", dir);
		EXIT_STATUS = EXIT_FAILURE;
		return;
	}
	while((entry = fts_read(directory)) != NULL) {
		if (entry->fts_level == 0) {
			continue;
		}
		if (!R_recurse) {
			if (fts_set(directory, entry, FTS_SKIP) != 0) {
				fprintf(stderr, "Could not set fts options.\n");
				EXIT_STATUS = 1;
				continue;
			}	
		}
		
		fileinfo = entry->fts_info;
		if (fileinfo != FTS_DP) {
			formatnondir(entry->fts_name, *entry->fts_statp);
		}
	}	
	
	fts_close(directory);
}

void
cleanup() {
	int i;
	for (i = 0; i < NUMDIRS; i++) {
		free(DIRS[i]);
	}
	free(DIRS);
	for (i = 0; i < NUMNONDIRS; i++) {
		free(NONDIRS[i]);
	}
	free(NONDIRS);
}

int
main(int argc, char **argv) 
{

	int i, argoffset;
	struct stat sb;
	
	setprogname(argv[0]);
	
	// validate directory/permissions
	// formatting 
 	EXIT_STATUS = 0;	
	NUMDIRS = 0;
	NUMNONDIRS = 0;
	argoffset = 0;
	if (parseargs(argc, argv)) {
		argoffset = 1;
	}
	 
	for (i = 1 + argoffset; i < argc; i++){
		if (stat(argv[i], &sb) == 0) {
			if (S_ISDIR(sb.st_mode)) {
				NUMDIRS++;
			} else {
				NUMNONDIRS++;
			}
		} else {
			fprintf(stderr, "%s: cannot access %s: No such file or director\n", getprogname(), argv[i]);
			EXIT_STATUS = EXIT_FAILURE;
		}
	}

	if (NUMDIRS > 0 &&  (DIRS = malloc(NUMDIRS * sizeof(char*))) == NULL) {
		fprintf(stderr, "Memory could not be allocated");
		exit(EXIT_FAILURE);
	}


	if (NUMNONDIRS > 0 && (NONDIRS = malloc(NUMNONDIRS * sizeof(char*))) == NULL) {
		fprintf(stderr, "Memory could not be allocated");
		exit(EXIT_FAILURE);
	}

	splitargs(argc, argv, argoffset);
	
	qsort(NONDIRS, NUMNONDIRS, sizeof(char*), lexicosort);	
	qsort(DIRS, NUMDIRS, sizeof(char*), lexicosort);
	/*	
	for (i = 0; i < NUMNONDIRS; i++) {
		printf("\t%d %s\n", i, NONDIRS[i]);
	}
	for (i = 0; i < NUMDIRS; i++) {
		printf("\t%d %s\n", i, DIRS[i]);
	}*/

	for (i = 0; i < NUMNONDIRS; i++) {
		if (stat(NONDIRS[i], &sb) != 0) {
			fprintf(stderr, "%s: cannot access %s: No such file or directory.\n", getprogname(), NONDIRS[i]);
			continue;
		}
		formatnondir(NONDIRS[i], sb);
	}
	for (i = 0; i < NUMDIRS; i++) {
		
		if (stat(DIRS[i], &sb) != 0) {
			fprintf(stderr, "%s: cannot access %s: No such file or directory.\n", getprogname(), DIRS[i]);
			continue;
		}
		if (NUMDIRS + NUMNONDIRS > 1) {
			if (i > 0) {
				printf("\n");
			}
			printf("%s:\n", DIRS[i]);
		
		}
		formatdir(DIRS[i]);
	}

	cleanup(DIRS, NONDIRS);
	exit(EXIT_STATUS);
}
