/*
 * "Advanced Programming in the UNIX Environment" midterm assignment
 * Based on NETBSD's ls(1)
*/

/* Shivam Sheth (ssheth7)
 * ls: list the files in a directory
 * 10/02/2021
*/

#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <dirent.h>
#include <errno.h>
#include <fts.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ls.h"
#include "helpers.h"
#include "print.h"
/*
 TODO:
 combine traversal logic
 use getbsize
 change padding struct parameters
 run exit status tests
 store time in unsigned form
 left align users/group
*/

/* flags  */
int A_allentries;        /* Includes all files but current/previous directory  */
int a_allentries;        /* Includes hidden files  */
int c_lastchanged;       /* Shows last changed  */
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

long BLOCKSIZE;		 /* BLOCKSIZE env variable  */
int EXIT_STATUS; 
int NUMDIRS; 		 /* Number of directory arguments  */
int NUMNONDIRS; 	 /* Number of non-dirextory arguments */
char **DIRS;   		 /* String array of directory arguments */
char **NONDIRS; 	 /* String array of non-directory arguments */



/*
 * Parses command line arguments and sets option flags
*/
int
parseargs(int argc, char **argv)
{	
	int opt, flagsset, envlength;
	char* blocksize;
		
	if (isatty(1)) {
		q_forcenonprintable = 1;	
	} 
	BLOCKSIZE = 512;
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
				if (f_unsorted == 1) {
					break;
				}
				c_lastchanged = 1;
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
				r_reverseorder = S_sizesorted = u_lastaccess = c_lastchanged = 0;
				break;
			case 'h':
				h_humanreadable = 1;
				k_kilobytes = 0;
				break;
			case 'i':
				i_inodes = 1;
				break;
			case 'k':
				k_kilobytes = 1;
				h_humanreadable = 0;
				BLOCKSIZE = 1024;
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
				break;
			case 'R':
				R_recurse = 1;
				break;
			case 'r':
				if (f_unsorted == 1) {
					break;
				}
				r_reverseorder = 1;
				break;
			case 'S':
				if (f_unsorted == 1) {
					break;
				}
				S_sizesorted = 1;
				break;
			case 's':
				s_systemblocks = 1;
				break;
			case 't':
				if (f_unsorted == 1) {
					break;
				}
				t_modifiedsorted = 1;
				break;
			case 'u':
				if (f_unsorted == 1) {
					break;
				}
				u_lastaccess = 1;
				c_lastchanged = 0;
				break;
			case 'w':
				q_forcenonprintable = 0;
				break;
			default: /* Invalid Argument provided  */
				(void)fprintf(stderr, "%s [-AacdFfhiklnqRrSstuw] [file ...]\n", getprogname());
				exit(EXIT_FAILURE);
		}
	}
	if (getuid() == 0 && a_allentries == 0) {
		A_allentries = 1;
	}
	if (s_systemblocks == 1 && h_humanreadable == 0 && k_kilobytes == 0) {
		if (getenv("BLOCKSIZE") != NULL) {
			envlength = strlen(getenv("BLOCKSIZE"));
			if ((blocksize = malloc(envlength * sizeof(char*))) == NULL) {
				(void)fprintf(stderr, "Could not allocate memory: %s\n", strerror(errno));
				exit(EXIT_FAILURE);
			}
			blocksize = getenv("BLOCKSIZE");
			BLOCKSIZE = strtol(blocksize, (char **)NULL, 10);
			if (BLOCKSIZE < 512) {
				(void)printf("%s: %ld: minimum blocksize is 512\n", getprogname(), BLOCKSIZE);
				BLOCKSIZE = 512;
			}
			if (BLOCKSIZE > 1074741824) {
				(void)printf("%s: %ld: maximum blocksize is 1G\n", getprogname(), BLOCKSIZE);
				BLOCKSIZE = 1074741824;
			}
		} 
		
	}
	return flagsset;
}

/*
 * Splits arguments into DIR and NONDIR arrays
*/
void
splitargs(int argc, char **argv, int offset)
{
	int i, dirindex, nondirindex, arglength;
	struct stat sb;
	dirindex = 0;
	nondirindex = 0;
	
	if (NUMDIRS == 0 && NUMNONDIRS == 0) { /* If no command line arguments are provided */
		if (d_directories == 0){
			NUMDIRS++;
			if ((DIRS = malloc(NUMDIRS * sizeof(char*))) == NULL) {
				(void)fprintf(stderr, "Memory could not be allocated");
				exit(EXIT_FAILURE);
			}
			if ((DIRS[0] = malloc(strlen(".") + 1)) == NULL) {
				(void)fprintf(stderr, "Could not allocate memory.\n");
				exit(EXIT_FAILURE);
			}
			if (strncpy(DIRS[0], ".", strlen(".")) == NULL) {
				(void)fprintf(stderr, "Could not copy to destination\n");
				exit(EXIT_FAILURE);
			}
		} else {
			NUMNONDIRS++;
			if ((NONDIRS = malloc(NUMNONDIRS * sizeof(char*))) == NULL) {
				(void)fprintf(stderr, "Memory could not be allocated");
				exit(EXIT_FAILURE);
			}
			if ((NONDIRS[0] = malloc(strlen(".") + 1)) == NULL) {
				(void)fprintf(stderr, "Could not allocate memory.\n");
				exit(EXIT_FAILURE);
			}
			if (strncpy(NONDIRS[0], ".", strlen(".")) == NULL) {
				(void)fprintf(stderr, "Could not copy to destination\n");
				exit(EXIT_FAILURE);
			}
		}
	}
	
	for (i = 1 + offset; i < argc; i++){

		if (stat(argv[i], &sb) == 0) {
			arglength = strlen(argv[i]);
			if (S_ISDIR(sb.st_mode) && d_directories == 0) {
				if ((DIRS[dirindex] = malloc(arglength + 1)) == NULL) {
					(void)fprintf(stderr, "Could not allocate memory.\n");
				}
				if (strncpy(DIRS[dirindex], argv[i], arglength) == NULL) {
					(void)fprintf(stderr, "Could not copy %s to buffer.\n", argv[i]);
				}
				dirindex++;
			} else {
				if ((NONDIRS[nondirindex] = malloc(arglength + 1)) == NULL) {
					(void)fprintf(stderr, "Could not allocate memory.\n");
				}
				
				if (strncpy(NONDIRS[nondirindex], argv[i], arglength) == NULL) {
					(void)fprintf(stderr, "Could not copy %s to buffer.\n", argv[i]);
				}
				nondirindex++;
			}
		} else {
			EXIT_STATUS = EXIT_FAILURE;
		}
	}
}

/*
 * Reads flags and called corresponding print functions 
*/
void
formatentry(int directentry, char* entry, char* path, struct stat sb, struct paddings paddings)
{
	int printallflags = A_allentries || a_allentries;
	
	if (directentry == 0 && d_directories == 0 && printallflags == 0 && entry[0] == '.') {
		return;
	}
	
	if (F_specialsymbols == 1) {
		addsymbols_F(&entry, sb);
	}
	if (i_inodes == 1) {
		printinode_i(sb, paddings.inode);
	} 
	if (s_systemblocks == 1) {
		printblocks_s(sb.st_blocks, sb.st_size,  1, paddings.block);
	}
	if (n_numericalids == 1 || l_longformat == 1) {
		printlong_l(entry, path, sb, paddings);
	}  else if (q_forcenonprintable == 1) {
		printraw_q(entry);
	} else {
		printdefault(entry);
	} 
}

/*
 * Reads contents of directory using set sort flags and sends directory 
 * entries to formatentry
*/
void
formatdir(char* dir) 
{
	int blocksum, currentlevel, fileinfo;
	int ftsopenflags, printallflags, ignoredflag, outputflag;
	int blockpadding, userpadding, grouppadding, sizepadding, inodepadding;
	char *dirptr[2];
	paddings paddings;
	FTSENT *blockchildren, *entry, *recursivechildren;
	FTS *directory;

	
	int (*comparefunction)(const FTSENT **, const FTSENT **);
	paddings.block = 0;
	paddings.user = 0;
	paddings.group = 0;
	paddings.size = 0;
	paddings.inode = 0;
	paddings.link = 0;
	comparefunction = &fts_lexicosort;
	
	if (r_reverseorder == 1) {
		comparefunction = &fts_rlexicosort;
	}
	
	if (S_sizesorted == 1) {
		comparefunction = &fts_sizesort;
		if (r_reverseorder == 1) {
			comparefunction = &fts_rsizesort;
		}
	}

	if (t_modifiedsorted == 1) {
		comparefunction = &fts_timemodifiedsort;
		if (r_reverseorder == 1) {
			comparefunction = &fts_rtimemodifiedsort;
		}
	}
	
	if (t_modifiedsorted == 1 && u_lastaccess == 1) {
		comparefunction = &fts_lastaccesssort;
		if (r_reverseorder == 1) {
			comparefunction = &fts_rlastaccesssort;
		}
	}
	if (t_modifiedsorted == 1 && c_lastchanged == 1) {
		comparefunction = &fts_statuschangesort;
		if (r_reverseorder == 1) { 
			comparefunction = &fts_rstatuschangesort;
		}
	}
	if (f_unsorted == 1) {
		comparefunction = NULL;	
	}
	
	ftsopenflags = FTS_COMFOLLOW;
	if (a_allentries == 1) {
		ftsopenflags |= FTS_SEEDOT;
	}
	if (R_recurse == 1) {
		ftsopenflags |= FTS_NOCHDIR;
	}
	if ((dirptr[0] = malloc(strlen(dir) + 1)) == NULL) {
		(void)fprintf(stderr, "Could not allocate memory.\n");
		exit(EXIT_FAILURE);
	}	
	dirptr[0] = dir;
	dirptr[1] = NULL;
	
	if ((directory = fts_open(dirptr, ftsopenflags, comparefunction)) == NULL) {
		(void)fprintf(stderr, "Cannot open %s.\n", dir);
		EXIT_STATUS = EXIT_FAILURE;
		return;
	}
	
	if (R_recurse == 0) {
		while((entry = fts_read(directory)) != NULL) {
			fileinfo = entry->fts_info;
			if (fileinfo == FTS_DP) {
				continue;
			}
			if (entry->fts_level > 0  && fileinfo == FTS_D) {
				if (fts_set(directory, entry, FTS_SKIP) != 0) {
					(void)fprintf(stderr, "Could not set fts options.\n");
					EXIT_STATUS = 1;
					continue;
				}
			}
			blocksum = 0;
			if (l_longformat ==1 || n_numericalids == 1|| s_systemblocks == 1 || isatty(1) == 0) {
				if (entry->fts_level == 0 && (blockchildren = fts_children(directory, 0)) != NULL) {
					blocksum = countblocks(blockchildren, &paddings);
					printblocks_s(blocksum, blocksum, 0, paddings.block);
					printf("Padding: blocks: %d user: %d group: %d size:  %d inode : %d link %d\n", 
					paddings.block, paddings.user, paddings.group, paddings.size, 
					paddings.inode, paddings.link);
				}
			}
			if (entry->fts_level > 0) {
				formatentry(0, entry->fts_name, entry->fts_path, *entry->fts_statp, paddings); 
			}
		}	
	} else {
		outputflag = 0;
		printallflags = A_allentries || a_allentries;

		while((entry = fts_read(directory)) != NULL) {
			fileinfo = entry->fts_info;
			if (fileinfo == FTS_DP) {
				continue;
			}
			currentlevel = entry->fts_level;
			blocksum = 0;
			ignoredflag = 0;
			if (printallflags == 0 && strncmp(entry->fts_name, ".", 1) == 0 && currentlevel != 0) { 
				ignoredflag = 1;
			}
			if (fileinfo == FTS_D) {
				if (ignoredflag == 0 && outputflag == 1) {
					printf("\n%s:\n", entry->fts_path);
				}
				paddings.block = 0;
				paddings.user = 0;
				paddings.group = 0;
				paddings.size = 0;
				paddings.inode = 0;
				paddings.link = 0;
			}
		
			outputflag = 1;
			if (ignoredflag == 0 && (l_longformat == 1|| n_numericalids == 1 || s_systemblocks == 1 || isatty(1) == 0)) {
				if ((blockchildren = fts_children(directory, 0)) != NULL) {
					blocksum = countblocks(blockchildren, &paddings); 
					printblocks_s(blocksum, blocksum,  0, paddings.block);
				//	printf("Padding: blocks: %d user: %d group: %d size: %d inode : %d\n", 
				//	blockpadding, userpadding, grouppadding, sizepadding, inodepadding);
				}
			}
			if (ignoredflag == 0 && (recursivechildren = fts_children(directory, 0)) != NULL) {	
				getimmediatechildren(recursivechildren, currentlevel, &paddings); 
			}
		}	
	}
	fts_close(directory);
	free(dirptr[0]);
	free(dirptr[1]);
}

/*
 * Frees memory from global data structures
*/
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

/*
 * Program takes in a set of arguments and optionally a directory. If no directory is
 * provided, the program defaults to the current shell's directory. The program prints the
 * contents of the directory based on the arguments given. 
*/
int
main(int argc, char **argv) 
{
	int i, argoffset, outputflag;
	struct stat sb;
	paddings paddings;
	
	setprogname(argv[0]);
	
 	EXIT_STATUS = 0;	
	NUMDIRS = 0;
	NUMNONDIRS = 0;
	argoffset = 0;
	
	if (parseargs(argc, argv)) {
		argoffset = 1;
	}
	 
 	/* Counts the number of nondirs and dirs arguments given */		
	for (i = 1 + argoffset; i < argc; i++){
		if (stat(argv[i], &sb) == 0) {
			if (S_ISDIR(sb.st_mode) && d_directories == 0) {
				NUMDIRS++;
			} else {
				NUMNONDIRS++;
			}
		} else {
			(void)fprintf(stderr, "%s: %s: No such file or directory\n", getprogname(), argv[i]);
			EXIT_STATUS = EXIT_FAILURE;
			if (i == argc - 1 && i == 1 + argoffset) {
				exit(EXIT_STATUS);
			}
		}
	}
	if (NUMDIRS > 0 &&  (DIRS = malloc(NUMDIRS * sizeof(char*))) == NULL) {
		(void)fprintf(stderr, "Memory could not be allocated");
		exit(EXIT_FAILURE);
	}


	if (NUMNONDIRS > 0 && (NONDIRS = malloc(NUMNONDIRS * sizeof(char*))) == NULL) {
		(void)fprintf(stderr, "Memory could not be allocated");
		exit(EXIT_FAILURE);
	}

	splitargs(argc, argv, argoffset);
	
	qsort(NONDIRS, NUMNONDIRS, sizeof(char*), lexicosort);	
	qsort(DIRS, NUMDIRS, sizeof(char*), lexicosort);

	outputflag = 0;
	
	if (l_longformat  == 1|| n_numericalids == 1) {
		paddings.block = 0;
		paddings.user = 0;
		paddings.group = 0;
		paddings.size = 0;
		paddings.inode = 0;
		paddings.link = 0;
		for (i = 0; i < NUMNONDIRS; i++) {
			if (stat(NONDIRS[i], &sb) == 0) {
				getpaddingsizes(sb, &paddings);	
			}
		}					
	}
	/* Visits non directory entries and then directory entries */	
	for (i = 0; i < NUMNONDIRS; i++) {
		if (stat(NONDIRS[i], &sb) != 0) {
			(void)fprintf(stderr, "%s: cannot access %s: No such file or directory.\n"
			, getprogname(), NONDIRS[i]);
			continue;
		}
		formatentry(1, NONDIRS[i], dirname(NONDIRS[i]), sb, paddings);
		outputflag = 1;
	}

	for (i = 0; i < NUMDIRS; i++) {
		
		if (stat(DIRS[i], &sb) != 0) {
			(void)fprintf(stderr, "%s: cannot access %s: No such file or directory.\n", getprogname(), DIRS[i]);
			continue;
		}
		if (NUMDIRS + NUMNONDIRS > 1) {
			if (outputflag == 1) {
				(void)printf("\n");
			}
			(void)printf("%s:\n", DIRS[i]);
		}
			formatdir(DIRS[i]); 
			outputflag = 1;
	}

	cleanup(DIRS, NONDIRS);
	printf("Program exited with status: %d\n", EXIT_STATUS);
	exit(EXIT_STATUS);
}
