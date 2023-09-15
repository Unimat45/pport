CC 		:= cc
C_FLAGS := -O3 -Wall -Wextra -std=c11 -s

INCLUDE := include
SRC 	:= src
BIN 	:= bin
EXEC	:= pport

LIB		:= -ljson-c

all: $(BIN)/$(EXEC)

$(BIN)/$(EXEC): $(SRC)/*.c
	[ -d bin ] || mkdir bin
	$(CC) $(C_FLAGS) -I$(INCLUDE) $^ -o $@ $(LIB)

clean:
	[ -f $(BIN)/$(EXEC) ] && rm $(BIN)/$(EXEC)