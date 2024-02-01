#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/files.h"
#include "../../include/search.h"
#include "../include/argParser.h"

#define CWD_SIZE 256

int main(int argc, char *argv[]) {
  Arguments **args = parseArgs(argc, argv);

  char *path;
  char *target;
  Types type;
  int i = 0;

  if (args == NULL) {
    return -1;
  }

  while (args[i] != NULL) {
    // use switch fall throught and disable warning for fall throught
    if (!strcmp(args[i]->flag, "--path")) {
      asprintf(&path, "%s", args[i]->value);
    } else if (!strcmp(args[i]->flag, "--target")) {
      asprintf(&target, "%s", args[i]->value);
    } else if (!strcmp(args[i]->flag, "--type")) {
      if (!strcmp(args[i]->value, "function")) {
        type = FUNCTION;
      } else if (!strcmp(args[i]->value, "struct")) {
        type = STRUCT;
      } else if (!strcmp(args[i]->value, "union")) {
        type = UNION;
      } else if (!strcmp(args[i]->value, "enum")) {
        type = ENUM;
      }
    }

    i++;
  }

  freeArguments(args);

  if (target == NULL) {
    return -2;
  }

  char *cwd;

  char **files;

  if (path == NULL) {
    cwd = (char *)malloc(CWD_SIZE);

    if (getcwd(cwd, CWD_SIZE) == NULL)
      return 1;

    files = fetchFiles(cwd);
  } else {
    files = fetchFiles(path);
  }

  if (files == NULL)
    return -3;

  int k = 0;
  while (files[k] != NULL) {

    Result **results = searchFile(files[k], target, type);

    if (results != NULL) {
      int i = 0;
      while (results[i] != NULL) {
        // could add separate header for printing colored text
        printf("\033[1;32m");
        printf("FILE PATH:[%s] >>> LINE NUMBER:%d\n", results[i]->filename,
               results[i]->lineNumber);
        printf("\033[0m");
        i++;
      }

      freeResults(results);
    }

    k++;
  }

  freeFiles(files);

  if (cwd != NULL) {
    free(cwd);
  }

  return 0;
}
