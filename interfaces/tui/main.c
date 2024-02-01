#include <curses.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ncurses.h>

#include "../../include/files.h"
#include "../../include/search.h"
#include "../include/argParser.h"

#define CWD_SIZE 256

/* TODO:
 * 1. Bug kada input-am samo / dobijem seg fault
 * 1. Fix where line is on some files it is exatcly on targetline but on some it appears under it when doing mvprintw but it always highlights correct line. (You are able to see targetLine but cursor is not on it)
 * 2. Fix that on big files last line is not shown when you do (l < maxLines) instead of (l < maxLines - 1) it will show null as last line on files that aren't as big as those.
 * 3. Add so that u can scroll to the top of the file and show cursor to it. (Not mandatory)
 * 4. Same as number three but for the bottom. (Not mandatory)
 *
 */

int getMaxLines(const char *filename) {
  FILE *fp = fopen(filename, "r");
  int newLineCounter = 0;
  int c = '\0';
  while ((c = fgetc(fp)) != EOF) {
    if (c == '\n') {
      newLineCounter++;
    }
  }
  fclose(fp);
  return newLineCounter;
}

char **readFile(const char *filename) {
  FILE *filePtr;
  char *content = NULL;

  filePtr = fopen(filename, "r");

  if (filePtr == NULL)
    return NULL;

  fseek(filePtr, 0, SEEK_END);
  long fileSize = ftell(filePtr);
  fseek(filePtr, 0, SEEK_SET);

  content = (char *)malloc(fileSize + 1);

  if (content == NULL) {
    fclose(filePtr);
    return NULL;
  }

  int maxLines = getMaxLines(filename);
  char **lines = malloc((maxLines + 1) * sizeof(char *));

  if (lines == NULL)
    return NULL;

  for (int f = 0; f <= maxLines; f++) {
    lines[f] = NULL;
  }

  char *currentLine = malloc(MAX_LINE_LENGTH + 1);
  if (currentLine == NULL)
    return NULL;

  int lineCounter = 0;

  int currentCharacter = '\0';

  int currentCharacterCounter = 0;
  while ((currentCharacter = fgetc(filePtr)) != EOF) {
    if (currentCharacter == '\n') {
      currentLine[currentCharacterCounter] = '\0';
      lines[lineCounter] = strdup(currentLine);
      lineCounter++;
      currentCharacterCounter = 0;
    } else {
      currentLine[currentCharacterCounter] = currentCharacter;
      currentCharacterCounter++;
    }
  }
  free(currentLine);

  fclose(filePtr);

  return lines;
}

void printFile(char **lines, int f, int l, int bold) {
  curs_set(0);
  for (int i = f; i <= l; i++) {
    if (i == bold) {
      attron(COLOR_PAIR(1));
      attron(A_BOLD);
    }
    mvprintw(i - f, 0, "%s\n", lines[i]);
    if (i == bold) {
      attroff(COLOR_PAIR(1));
      attroff(A_BOLD);
    }
  }
  mvprintw(bold, 0, "");
  refresh();
  curs_set(1);
}

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

  int resultsCounter = 0;
  Result **resultsPtr = malloc(MAX_RESULTS * sizeof(Result *));
  if (resultsPtr == NULL)
    return 2;

  for (int i = 0; i < MAX_RESULTS; i++) {
    resultsPtr[i] = NULL;
  }

  int k = 0;

  while (files[k] != NULL) {
    Result **results = searchFile(files[k], target, type);

    if (results != NULL) {
      int i = 0;
      while (results[i] != NULL) {
        resultsPtr[resultsCounter] = results[i];
        resultsCounter++;
        i++;
      }
    }
    k++;
  }

  freeFiles(files);

  if (cwd != NULL) {
    free(cwd);
  }

  initscr();
  cbreak();
  keypad(stdscr, TRUE);
  noecho();
  curs_set(0);
  start_color();
  use_default_colors();
  init_pair(1, COLOR_RED, COLOR_BLACK);

  int t = 0;
  while (resultsPtr[t] != NULL) {
    printw("[ ]FILE PATH:[%s] >>> LINE NUMBER:%d\n", resultsPtr[t]->filename,
           resultsPtr[t]->lineNumber);
    t++;
  }

  int currentOption = 0;
  int choice = 0;

  if (resultsPtr[0] == NULL) {
    printw("No files found for given search query\n");
    printw("Press any key to exit...\n");
    refresh();
    getch();
    endwin();

    return 0;
  }

  mvprintw(currentOption, 1, ">");
  refresh();

  while (1) {
    choice = getch();

    switch (choice) {
    case KEY_UP:
      mvprintw(currentOption, 1, " ");
      if (currentOption < 1)
        currentOption = t;
      currentOption--;
      mvprintw(currentOption, 1, ">");
      break;

    case KEY_DOWN:
      mvprintw(currentOption, 1, " ");
      if (currentOption > t - 2)
        currentOption = -1;
      currentOption++;
      mvprintw(currentOption, 1, ">");
      break;

    case 10: // Enter key
      curs_set(1);
      clear();
      char **lines = readFile(resultsPtr[currentOption]->filename);
      if (lines == NULL) {
        printw("Error while trying to read file... Press any key to exit\n");
        refresh();
        getch();
        endwin();
        return -1;
      }

      int targetLine = resultsPtr[currentOption]->lineNumber - 1;
      int maxLines = 0;

      while (lines[maxLines] != NULL) {
        maxLines++;
      }

      int windowHeight, windowWidth;
      getmaxyx(stdscr, windowHeight, windowWidth);

      int fileChoice = 0;
      int done = 0;

      int f = 0;
      int l = 0;
      int mid = 0;

      if (maxLines < windowHeight) {
        l = maxLines - 1;
      } else {
        mid = windowHeight / 2;
        f = targetLine - mid;
        if (f < 0) {
          f = 0;
        }
        l = targetLine + mid;
        if (l > maxLines) {
          l = maxLines - 1;
        }
      }

      int changed = 0;
      printFile(lines, f, l, targetLine);

      while (!done) {
        fileChoice = getch();
        switch (fileChoice) {
        case KEY_UP:
          if (f > 0) {
            f--;
            l--;
            changed = 1;
          }
          break;
        case KEY_DOWN:
          if (l < maxLines - 1) {
            f++;
            l++;
            changed = 1;
          } /*else {
            f = 0;
            l = (maxLines < windowHeight - 1 ? maxLines - 1 : windowHeight - 1) + f;
            if (l > maxLines)
              l = maxLines - 1;
            changed = 1;
          }*/ // if you want user to go back to first line after last line
          break;
        case 'q':
          done = 1;
          break;

        default:
          break;
        }
        if (changed) {
          printFile(lines, f, l, targetLine);
          changed = 0;
        }
      }

      clear();
      t = 0;
      while (resultsPtr[t] != NULL) {
        printw("[ ]FILE PATH:[%s] >>> LINE NUMBER:%d\n",
               resultsPtr[t]->filename, resultsPtr[t]->lineNumber);
        t++;
      }

      mvprintw(currentOption, 1, ">");
      refresh();
      free(lines);
      curs_set(0);
      break;

    case 'q':
      endwin();
      return 0;

    default:
      break;
    }
  }

  freeResults(resultsPtr);

  return 0;
}
