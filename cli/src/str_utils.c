#include "str_utils.h"

#include <ctype.h>

#ifdef _WINDOWS
#define UNICODE
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <wchar.h>

size_t read_line(char *const line) {
    HANDLE stdin_hnd = GetStdHandle(STD_INPUT_HANDLE);

    wchar_t wline[1024] = {0};
    DWORD bytesRead = 0;
    ReadConsole(stdin_hnd, wline, 1024, &bytesRead, NULL);

    // Removes cr and nl characters from buffer
    wline[bytesRead - 2] = 0;

    WideCharToMultiByte(CP_UTF8, 0, wline, bytesRead - 2, line, 1024, "-",
                        NULL);
    return bytesRead - 2;
}

#else

#include <stdio.h>

size_t read_line(char *const line) {
    fgets(line, 1024, stdin);
    size_t bytesRead = strnlen(line, 1024);
    line[bytesRead - 1] = 0;

    return bytesRead;
}

#endif

size_t utf8_strlen(const char *bytes) {
    size_t len = 0;

    while (*bytes != 0) {
        if ((*bytes & 0x80) == 0 || (*bytes & 0xE0) == 0xC0) {
            len++;
            bytes++;
        } else if ((*bytes & 0xF0) == 0xE0) {
            len++;
            bytes += 2;
        } else if ((*bytes & 0xF8) == 0xF0) {
            len++;
            bytes += 3;
        } else {
            bytes++;
        }
    }

    return len;
}

size_t findLongestLabel(Pin **parallel, size_t size) {
    size_t longest = 0;

    for (size_t i = 0; i < size; i++) {
        size_t label_len = utf8_strlen(parallel[i]->label);

        if (label_len > longest) {
            longest = label_len;
        }
    }

    return longest;
}

int istrcmp(const char *a, const char *b) {
    for (; *a != 0 && *b != 0; a++, b++) {
        if (toupper(*a) != toupper(*b)) {
            return 0;
        }
    }

    return *a == 0 && *b == 0;
}
