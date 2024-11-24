#ifndef GLOBALS_H
#define GLOBALS_H

#ifndef PPORT
#define PPORT 0x378
#endif

#define PARA_LOOP(i) for (int i = 0; i < 8; i++)

#define FIRST_DAY(range) ((range >> 24) & 0xFF)
#define FIRST_MONTH(range) ((range >> 16) & 0xFF)
#define LAST_DAY(range) ((range >> 8) & 0xFF)
#define LAST_MONTH(range) (range & 0xFF)

#endif // GLOBALS_H