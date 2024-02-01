// files.h
#ifndef FILES_H
#define FILES_H

#define MAX_FILES 128
#define RECURSION_LIMIT 128

#include <dirent.h>
#include <unistd.h>

char **fetchFiles(const char *targetDirectory);
void freeFiles(char **files);

#endif // FILES_H
