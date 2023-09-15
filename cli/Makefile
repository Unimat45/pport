CC		:= cc
C_FLAGS	:= -O3 -std=c11 -Wall -Wextra -s

INCLUDE := include
SRC		:= src
BIN		:= bin
LIBS	:= -ljson-c

EXEC	:= pport-cli

all: $(BIN)/$(EXEC)

$(BIN)/$(EXEC): $(SRC)/*.c
	[ -d bin ] || mkdir bin
	$(CC) $(C_FLAGS) -I$(INCLUDE) -o $@ $^ $(LIBS)

clean:
	[ -f $(BIN)/$(EXEC) ] && rm $(BIN)/$(EXEC)