#ifndef TEST_OUTB_H
#define TEST_OUTB_H

#define outb __outb
#define inb __inb

void __outb(unsigned char value, unsigned short port);
unsigned char __inb(unsigned short port) ;

#endif // TEST_OUTB_H