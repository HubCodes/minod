CC:=clang
BIN:=minod
CLIENT_BIN:=client
CFLAGS:=-std=c99 -Wall -Werror -Wextra -pedantic -O2
RM:=rm

.PHONY: all default clean client

all: main.c hashmap.c
	$(CC) main.c hashmap.c $(CFLAGS) -o $(BIN)

$(BIN): all

debug: main.c hashmap.c
	$(CC) main.c hashmap.c $(CFLAGS) -g -o $(BIN)

run: $(BIN)
	./$(BIN)

client: client.c
	$(CC) client.c $(CFLAGS) -o $(CLIENT_BIN)

clean:
	$(RM) ./$(BIN) ./$(CLIENT_BIN)

