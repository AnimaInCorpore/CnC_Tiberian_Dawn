#include <cctype>
#include <cstring>

#include "legacy/function.h"

void __cdecl strtrim(char *buffer)
{
    if (!buffer) return;

    char *end = buffer + std::strlen(buffer);
    while (end > buffer && std::isspace(static_cast<unsigned char>(*(end - 1)))) {
        *(--end) = '\0';
    }
}
