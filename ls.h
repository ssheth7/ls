#ifndef _LS_H_
#define _LS_H_

#include <sys/stat.h>

#include "helpers.h"

int parseargs(int, char*[]);
void splitargs(int, char*[], int);
void formatentry(char*, char*, struct stat, struct paddings);
void formatdir(char*);
void cleanup();
int main(int, char*[]);

#endif /* !_LS_H_  */
