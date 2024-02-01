// argParser.h
#ifndef ARGPARSER_H
#define ARGPARSER_H

#define MAX_ARGS 4

#include <dirent.h>
#include <unistd.h>

typedef struct Arguments {
  char *flag;
  char *value;
} Arguments;

Arguments **parseArgs(int argc, char *argv[]);
void freeArguments(Arguments **args);

#endif // ARGPARSER_H
