/*
 * Contains sorting methods for traversal
*/
#include <sys/stat.h>

#include <fts.h>
#include <string.h>

#include "cmp.h"

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
	off_t file1size = (*file1)->fts_statp->st_size;
	off_t file2size = (*file2)->fts_statp->st_size;
	if (file1size < file2size) {
		return 1;
	} 
	if (file1size > file2size) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int 
fts_rsizesort(const FTSENT** file1, const FTSENT** file2)
{
	off_t file1size = (*file1)->fts_statp->st_size;
	off_t file2size = (*file2)->fts_statp->st_size;
	if (file1size < file2size) {
		return -1;
	} 
	if (file1size > file2size) {
		return 1;
	}
	return fts_rlexicosort(file1, file2);
}

int
fts_timemodifiedsort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_mtime;
	time_t file2time = (*file2)->fts_statp->st_mtime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int
fts_rtimemodifiedsort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_mtime;
	time_t file2time = (*file2)->fts_statp->st_mtime;
	struct timespec file1tspec, file2tspec;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	file1tspec = (*file1)->fts_statp->st_atim;
	file2tspec = (*file2)->fts_statp->st_atim;
	file1time = file1tspec.tv_nsec;
	file2time = file2tspec.tv_nsec;
		
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return fts_rlexicosort(file1, file2);
}

int
fts_lastaccesssort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_atime;
	time_t file2time = (*file2)->fts_statp->st_atime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int
fts_rlastaccesssort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_atime;
	time_t file2time = (*file2)->fts_statp->st_atime;
	struct timespec file1tspec, file2tspec;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	file1tspec = (*file1)->fts_statp->st_atim;
	file2tspec = (*file2)->fts_statp->st_atim;
	file1time = file1tspec.tv_nsec;
	file2time = file2tspec.tv_nsec;
		
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return fts_rlexicosort(file1, file2);
}

int
fts_statuschangesort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_ctime;
	time_t file2time = (*file2)->fts_statp->st_ctime;
	if (file1time < file2time) {
		return 1;
	}	
	if (file1time > file2time) {
		return -1;
	}
	return fts_lexicosort(file1, file2);
}

int
fts_rstatuschangesort(const FTSENT** file1, const FTSENT** file2)
{
	time_t file1time = (*file1)->fts_statp->st_ctime;
	time_t file2time = (*file2)->fts_statp->st_ctime;
	if (file1time < file2time) {
		return -1;
	}	
	if (file1time > file2time) {
		return 1;
	}
	return fts_rlexicosort(file1, file2);
}
