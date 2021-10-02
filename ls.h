#ifndef _LS_H_
#define _LS_H_

#include <sys/stat.h>

int parseargs(int, char*[]);
void splitargs(int, char*[], int);
void formatnondir(char*, struct stat);
void formatdir(char*);
void cleanup();
int main(int, char*[]);

#endif /* !_LS_H_  */
