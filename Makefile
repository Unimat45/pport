CC 		:= clang
C_FLAGS := -Wall -std=c17 -ggdb

INCLUDE := include
SRC 	:= src
BIN 	:= bin
EXEC	:= pport

TEST 	:= test

LIB		:= 

all: $(BIN)/$(EXEC)

test: $(BIN)/$(EXEC).test

$(BIN)/$(EXEC): $(SRC)/*.c
	$(CC) $(C_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIB)

$(BIN)/$(EXEC).test: $(TEST)/*.c
	$(CC) $(C_FLAGS) -I$(INCLUDE) $^ $(SRC)/parallel.c $(SRC)/tokstr.c $(SRC)/command.c -o $@ $(LIB)

clean:
	rm $(BIN)/*