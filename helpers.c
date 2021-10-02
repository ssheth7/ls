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
