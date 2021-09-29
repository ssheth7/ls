#include <sys/stat.h>

#include <dirent.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "ls.h"

int 
countentries(char *dir)
{
	int numentries;
	struct dirent *dirp;
	DIR *dp;
	if ((dp = opendir(dir)) == NULL) {
		return -1;
	}
	numentries = 0;
	while ((dirp = readdir(dp)) != NULL) {
		numentries++;
	}

	closedir(dp);
	free(dirp);
	
	return numentries;
}

int 
lexicosort(const void* str1, const void* str2) 
{
	return strcmp(*(const char **) str1, *(const char **) str2);
}

int sizesort(const void* file1, const void* file2)
{

	return strcmp(*(const char **) file1, *(const char **) file2);

	/*const fileentry *f1 = (fileentry *)file1;
	const fileentry *f2 = (fileentry *)file2;
	if (f1->sb.st_size > f2->sb.st_size) {
		return 1;
	}
	if (f1->sb.st_size < f2->sb.st_size) {
		return -1;
	}
	return 0;
	*/
}
