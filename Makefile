CC 		:= clang
C_FLAGS := -O2 -Wall -Wextra -std=c17 -s

INCLUDE := include
SRC 	:= src
BIN 	:= bin
EXEC	:= pport

TEST 	:= test

LIB		:= -ljson-c

all: $(BIN)/$(EXEC)

test: $(BIN)/$(EXEC).test

$(BIN)/$(EXEC): $(SRC)/*.c
	$(CC) $(C_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIB)

$(BIN)/$(EXEC).test: $(TEST)/*.c
	$(CC) $(C_FLAGS) -I$(INCLUDE) $^ $(SRC)/parallel.c $(SRC)/tokstr.c $(SRC)/command.c -o $@ $(LIB)

clean:
	rm $(BIN)/*