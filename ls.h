#include <sys/stat.h>

int parseargs(int, char*[]);
void splitargs(int, char*[], int);
void formatnondir(char*, struct stat);
void formatdir(char*);
void cleanup();
int main(int, char*[]);
