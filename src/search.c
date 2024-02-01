#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/search.h"

bool regexSearch(char *pattern, char *text) {
  regex_t reegex;

  bool found;

  found = regcomp(&reegex, pattern, 0);

  found = regexec(&reegex, text, 0, NULL, 0);

  return !found;
}

/* WE NEED TO HANDLE IF DEFINITION IS IN THE NEW LINE LINE FOR EXAMPLE
 * int
 * someFunction(void)
 * NOT IMPLEMENETED YET ^
 */
Result **searchFile(const char *filename, char *target, Types type) {
  // length of target + 8 because next part is the type and function has 8
  // charchters longest of all types and then +1 for " "
  size_t searchPatternLength = strlen(target) + 8 + 1;
  char *searchPattern = malloc(searchPatternLength);

  if (searchPattern == NULL) {
    return NULL;
  }

  switch (type) {
  case FUNCTION:
    strcpy(searchPattern, target);
    strcat(searchPattern, "(");
    break;

  case STRUCT:
    strcpy(searchPattern, "struct");
    strcat(searchPattern, " ");
    strcat(searchPattern, target);
    break;

  case UNION:
    strcpy(searchPattern, "union");
    strcat(searchPattern, " ");
    strcat(searchPattern, target);
    break;

  case ENUM:
    strcpy(searchPattern, "enum");
    strcat(searchPattern, " ");
    strcat(searchPattern, target);
    break;

  default:
    strcpy(searchPattern, target);
    break;
  }

  Result **results = malloc(MAX_RESULTS * sizeof(Result *));
  if (results == NULL) {
    free(searchPattern);
    return NULL;
  }

  for (int i = 0; i < MAX_RESULTS; i++) {
    results[i] = NULL;
  }

  FILE *file;
  char line[MAX_LINE_LENGTH];

  file = fopen(filename, "r");

  if (file == NULL) {
    free(results);
    free(searchPattern);
    return NULL;
  }

  int currentLine = 0;
  int currentResultCount = 0;

  while (fgets(line, sizeof(line), file) != NULL) {
    size_t lineLength = strlen(line);
    line[lineLength - 1] = '\0';

    currentLine++;
    if (regexSearch(searchPattern, line)) {
      // Result struct -> include/search.h
      Result *result = malloc(sizeof(Result));

      if (result != NULL) {

        result->filename = strdup(filename);
        if (result->filename == NULL) {
          free(result);
        } else {
          result->lineNumber = currentLine;

          results[currentResultCount] = result;
          currentResultCount++;
        }
      }
    }
  }

  free(searchPattern);

  if (file != NULL) {
    fclose(file);
  }

  if (results == NULL) {
    free(results);
  }

  return results;
}

void freeResult(Result *result) {
  free((char *)result->filename);
  free(result);
}

void freeResults(Result **results) {
  int i = 0;
  while (results[i] != NULL) {
    freeResult(results[i]);
    i++;
  }

  free(results);
}
