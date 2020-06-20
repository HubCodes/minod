CC:=clang
BIN:=minod
CFLAGS:= -o $(BIN) -Wall -Werror -pedantic -O2
RM:=rm

.PHONY: all default clean

all: main.c hashmap.c
	$(CC) main.c hashmap.c $(CFLAGS)

$(BIN): all

debug: main.c hashmap.c
	$(CC) main.c hashmap.c $(CFLAGS) -g

run: $(BIN)
	./$(BIN)

clean:
	$(RM) ./$(BIN)

