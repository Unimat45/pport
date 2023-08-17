#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"

int main(void) {
	Command *inst1 = token_command("SET PIN 2 HIGH");
	Command *inst2 = token_command("SHOW PIN 5 LOW");
	Command *inst3 = token_command("REBOOT PIN 2 LOW");
	Command *inst4 = token_command("TOGGLE PIN ALL HIGH");
	Command *inst5 = token_command("LABEL PIN 8 HIGH");
	Command *inst6 = token_command("DFHDF PIN 3");
	Command *inst7 = token_command("set PIN 9 AEHN");

	assert(inst1 != NULL);
	assert(inst2 != NULL);
	assert(inst3 != NULL);
	assert(inst4 != NULL);
	assert(inst5 != NULL);
	assert(inst6 == NULL);
	assert(inst7 != NULL);

	free_command(inst1);
	free_command(inst2);
	free_command(inst3);
	free_command(inst4);
	free_command(inst5);
	free_command(inst6);
	free_command(inst7);

	Command *pin1 = token_command("SHOW PIN 2");
	Command *pin2 = token_command("SHOW");

	assert(pin1 != NULL);
	assert(pin2 != NULL);

	free_command(pin1);
	free_command(pin2);

	Command *nbr1 = token_command("SHOW PIN 2");
	Command *nbr2 = token_command("SET PIN 0");
	Command *nbr3 = token_command("SET PIN 16");
	Command *nbr4 = token_command("SET PIN ALL");
	Command *nbr5 = token_command("SET PIN HDFH");

	assert(nbr1 != NULL);
	assert(nbr2 == NULL);
	assert(nbr3 == NULL);
	assert(nbr4 != NULL);
	assert(nbr5 == NULL);

	free_command(nbr1);
	free_command(nbr2);
	free_command(nbr3);
	free_command(nbr4);
	free_command(nbr5);

	Command *state1 = token_command("SHOW PIN 2 HIGH");
	Command *state2 = token_command("SET PIN 0 LOW");
	Command *state3 = token_command("SET PIN 16 ON");
	Command *state4 = token_command("SET PIN ALL OFF");
	Command *state5 = token_command("SET PIN HDFH Bob");

	assert(state1 != NULL);
	assert(state2 == NULL);
	assert(state3 == NULL);
	assert(state4 != NULL);
	assert(state5 == NULL);

	free_command(state1);
	free_command(state2);
	free_command(state3);
	free_command(state4);
	free_command(state5);

	Command *label1 = token_command("SHOW PIN 2 HIGH Bob");
	Command *label2 = token_command("SET PIN 5 Bob Pol");
	Command *label3 = token_command("SET PIN 6 Bob Marley Makes Good Music");
	Command *label4 = token_command("SET PIN ALL OFF A");
	Command *label5 = token_command("SET PIN HDFH ON");

	assert(label1 != NULL);
	assert(label2 != NULL);
	assert(label3 != NULL);
	assert(label4 != NULL);
	assert(label5 == NULL);

	free_command(label1);
	free_command(label2);
	free_command(label3);
	free_command(label4);
	free_command(label5);

	printf("All tests passed\n");

	return 0;
}