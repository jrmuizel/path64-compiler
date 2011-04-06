
#include "string_case.h"

#include <cctype>

char *string_tolower(char *str) {
    char *s = str;
    while(*s != 0) {
        *s = std::tolower(*s);
        ++s;
    }

    return str;
}


char *string_toupper(char *str) {
    char *s = str;
    while(*s != 0) {
        *s = std::toupper(*s);
        ++s;
    }

    return str;
}

