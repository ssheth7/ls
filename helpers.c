/*
 * Contains sorting helper methods for ls.c
*/
#include <sys/stat.h>

#include <dirent.h>
#include <fts.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "ls.h"


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

int 
countblocks(FTSENT *children)
{
	int blocksum, printallflags;
	
	printallflags = A_allentries || a_allentries; 
	blocksum = 0;
	while (children != NULL) {
		while(children != NULL) {
			if (printallflags || strncmp(children->fts_name, ".", 1) != 0) {
				if (!h_humanreadable) {
					blocksum += children->fts_statp->st_blocks;
				} else {
					blocksum += children->fts_statp->st_size;	
				}
			}
			children = children->fts_link;
		}
	}
	return blocksum;
}

int
lexicosort(const void* str1, const void* str2) 
{
	return strcmp(*(const char **) str1, *(const char **) str2);
}

int 
fts_lexicosort(const FTSENT** file1, const FTSENT** file2)
{
	
	return (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int 
fts_rlexicosort(const FTSENT** file1, const FTSENT** file2)
{
	
	return -1 * (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int 
fts_sizesort(const FTSENT** file1, const FTSENT** file2)
{
	int file1size = (*file1)->fts_statp->st_size;
	int file2size = (*file2)->fts_statp->st_size;
	if (file1size < file2size) {
		return 1;
	} 
	if (file1size > file2size) {
		return -1;
	}
	return (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int 
fts_rsizesort(const FTSENT** file1, const FTSENT** file2)
{
	int file1size = (*file1)->fts_statp->st_size;
	int file2size = (*file2)->fts_statp->st_size;
	if (file1size < file2size) {
		return -1;
	} 
	if (file1size > file2size) {
		return 1;
	}
	return -1 * (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int
fts_timemodifiedsort(const FTSENT** file1, const FTSENT** file2)
{
	int file1time = (*file1)->fts_statp->st_mtime;
	int file2time = (*file2)->fts_statp->st_mtime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return -(strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int
fts_rtimemodifiedsort(const FTSENT** file1, const FTSENT** file2)
{
	int file1time = (*file1)->fts_statp->st_mtime;
	int file2time = (*file2)->fts_statp->st_mtime;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int
fts_lastaccesssort(const FTSENT** file1, const FTSENT** file2)
{
	int file1time = (*file1)->fts_statp->st_atime;
	int file2time = (*file2)->fts_statp->st_atime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int
fts_rlastaccesssort(const FTSENT** file1, const FTSENT** file2)
{
	int file1time = (*file1)->fts_statp->st_atime;
	int file2time = (*file2)->fts_statp->st_atime;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return (strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int
fts_statuschangesort(const FTSENT** file1, const FTSENT** file2)
{
	int file1time = (*file1)->fts_statp->st_ctime;
	int file2time = (*file2)->fts_statp->st_ctime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return -(strcmp((*file1)->fts_name, (*file2)->fts_name));
}

int
fts_rstatuschangesort(const FTSENT** file1, const FTSENT** file2)
{
	int file1time = (*file1)->fts_statp->st_ctime;
	int file2time = (*file2)->fts_statp->st_ctime;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return (strcmp((*file1)->fts_name, (*file2)->fts_name));
}
