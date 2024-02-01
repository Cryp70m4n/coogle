// search.h
#ifndef SEARCH_H
#define SEARCH_H

#define MAX_LINE_LENGTH 256
#define MAX_RESULTS 128

#include <stdbool.h>
#include <unistd.h>

typedef enum Types { FUNCTION, STRUCT, UNION, ENUM } Types;

typedef struct Result {
  const char *filename;
  int lineNumber;
} Result;

bool regexSearch(char *pattern, char *text);
Result **searchFile(const char *filename, char *target, Types type);
void freeResult(Result *result);
void freeResults(Result **results);

#endif // SEARCH_H
