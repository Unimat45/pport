#include "test_outb.h"
#include <stdio.h>

unsigned char c_v = 0;

void __outb(unsigned char value, unsigned short port) {
	c_v = value;
	printf("New value is: %d\n", c_v);
	(void)port;
}

unsigned char __inb(unsigned short port) {
	(void)port;
	return c_v;
}
