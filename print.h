#ifndef _PRINT_H_
#define _PRINT_H_

#include <sys/stat.h>

void printdefault(char*);
void printall_A(char*);
void printall_a(char*);
void addsymbols_F(char**, struct stat sb);
void printraw_q(char*);
void printraw_w(char*);
#endif /* !_PRINT_H_ */

