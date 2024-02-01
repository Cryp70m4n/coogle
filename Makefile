CC := gcc 
CFLAGS := -Wall -Wextra -g
EXEC := coogle
SRC := interfaces/tui/main.c interfaces/argParser.c src/files.c src/search.c
LDFLAGS := -lncurses

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(EXEC)
