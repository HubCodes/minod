CC:=clang
BIN:=minod
CLIENT_BIN:=client
CFLAGS:= -o $(BIN) -Wall -Werror -pedantic -O2
RM:=rm

.PHONY: all default clean client

all: main.c hashmap.c
	$(CC) main.c hashmap.c $(CFLAGS)

$(BIN): all

debug: main.c hashmap.c
	$(CC) main.c hashmap.c $(CFLAGS) -g

run: $(BIN)
	./$(BIN)

client: client.c
	$(CC) client.c $(CFLAGS)

clean:
	$(RM) ./$(BIN) ./$(CLIENT_BIN)

