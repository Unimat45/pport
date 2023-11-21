CC      ?= cc
C_FLAGS := -O3 -DNDEBUG -Wall -Wextra -std=c11 -s

INCLUDE := include
SRC     := src
BIN     := bin
EXEC    := pport

LIB     := -lconfig

all: $(BIN)/$(EXEC)

$(BIN)/$(EXEC): $(SRC)/*.c
	[ -d bin ] || mkdir bin
	$(CC) $(C_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIB)

clean:
	[ -f $(BIN)/$(EXEC) ] && rm $(BIN)/$(EXEC)
