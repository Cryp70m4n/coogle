#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/files.h"

// FAIL SAFE ADD RECURSION LIMIT ZA FILES
// ADD SUPPORT FOR SUB-DIRS

char **fetchFiles(const char *targetDirectory) {

  int recursionCounter = 0;

  char **files = malloc(MAX_FILES * sizeof(char *));
  if (files == NULL) {
    return NULL;
  }

  for (int i = 0; i < MAX_FILES; i++) {
    files[i] = NULL;
  }

  int currentFileCount = 0;

  DIR *directory;
  struct dirent *directoryEntry;

  directory = opendir(targetDirectory);

  if (directory == NULL) {
    return NULL;
  }

  while ((directoryEntry = readdir(directory)) != NULL) {
    if (directoryEntry->d_type == DT_REG) {
      // could make for loop here if we need to add more filters
      if (directoryEntry->d_name[strlen(directoryEntry->d_name) - 1] != 'h' &&
          directoryEntry->d_name[strlen(directoryEntry->d_name) - 1] != 'c') {
        continue;
      }

      char *filePath;
      if (targetDirectory[strlen(targetDirectory) - 1] != '/') {
        asprintf(&filePath, "%s/%s", targetDirectory, directoryEntry->d_name);
      } else {
        asprintf(&filePath, "%s%s", targetDirectory, directoryEntry->d_name);
      }
      files[currentFileCount] = filePath;

      /*if (files[currentFileCount] == NULL) {
                          return NULL;
      }*/

      if (files[currentFileCount] != NULL) {

        currentFileCount++;

        if (currentFileCount == MAX_FILES) {
          return files;
        }
      }
    }

    if (directoryEntry->d_type == DT_DIR &&
        strcmp(directoryEntry->d_name, ".") &&
        strcmp(directoryEntry->d_name, "..")) {
      // +1 for / makes sense but this code requires me to do +2 otherwise I get
      // corrupted size vs. prev_size Idk probably because of null terminator or
      // something
      char *newTarget =
          malloc(strlen(targetDirectory) + strlen(directoryEntry->d_name) + 2);
      strcpy(newTarget, targetDirectory);
      if (targetDirectory[strlen(targetDirectory) - 1] != '/') {
        strcat(newTarget, "/");
      }
      strcat(newTarget, strdup(directoryEntry->d_name));

      if (newTarget != NULL) {
        int i = 0;
        char **childFiles = fetchFiles(newTarget);
        recursionCounter++;
        free(newTarget);
        while (childFiles[i] != NULL) {
          files[currentFileCount] = childFiles[i];
          currentFileCount++;
          i++;
          if (currentFileCount == MAX_FILES) {
            return files;
          }
        }

        if (recursionCounter > RECURSION_LIMIT) {
          return files;
        }
      }
    }
  }

  closedir(directory);

  files[currentFileCount] = NULL;

  return files;
}

void freeFiles(char **files) {
  int i = 0;
  while (files[i] != NULL) {
    free(files[i]);
    i++;
  }
  free(files);
}
