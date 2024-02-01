#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/argParser.h"

// ignore switch fall through error

Arguments **parseArgs(int argc, char *argv[]) {
  if (argc < 3) {
    return NULL;
  }

  size_t knownArgumentsLength = 4;

  Arguments **args = malloc(MAX_ARGS * sizeof(Arguments *));
  if (args == NULL) {
    return NULL;
  }

  for (int i = 0; i < MAX_ARGS; i++) {
    args[i] = NULL;
  }

  char **knownArguments = malloc(knownArgumentsLength * sizeof(char *));
  knownArguments[0] = "--path";
  knownArguments[1] = "--target";
  knownArguments[2] = "--type";

  if (knownArguments == NULL) {
    free(args);
    return NULL;
  }

  int argumentCounter = 0;

  size_t knownArgumentsSize = 0;
  while (knownArguments[knownArgumentsSize] != NULL) {
    knownArgumentsSize++;
  }

  for (int i = 0; i < argc; i++) {
    for (int k = 0; k < (int)knownArgumentsSize; k++) {
      if (!strcmp(argv[i], knownArguments[k])) {
        if (i + 1 < argc) {
          args[argumentCounter] = malloc(sizeof(Arguments));

          if (args[argumentCounter] == NULL) {
            continue;
          }

          args[argumentCounter]->flag = strdup(argv[i]);

          if (args[argumentCounter]->flag == NULL) {
            free(args[argumentCounter]);
            continue;
          }

          args[argumentCounter]->value = strdup(argv[i + 1]);

          if (args[argumentCounter]->flag == NULL) {
            free(args[argumentCounter]->flag);
            free(args[argumentCounter]);
            continue;
          }

          argumentCounter++;
        }
      }
    }
  }

  return args;
}

void freeArguments(Arguments **args) {
  int i = 0;
  while (args[i] != NULL) {
    free(args[i]->flag);
    free(args[i]->value);
    free(args[i]);
    i++;
  }

  free(args);
}
