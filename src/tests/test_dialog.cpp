#include <cassert>
#include <cstring>
#include "legacy/function.h"

int main() {
    char buf1[128];
    std::strncpy(buf1, "Hello world", sizeof(buf1));
    int w1,h1;
    int lines1 = Format_Window_String(buf1, 1000, w1, h1);
    assert(lines1 == 1);
    // no CR inserted when fits
    assert(std::strchr(buf1, '\r') == nullptr);

    char buf2[128];
    std::strncpy(buf2, "A B C D E F G H I J K L", sizeof(buf2));
    int w2,h2;
    int lines2 = Format_Window_String(buf2, 10, w2, h2);
    assert(lines2 >= 1);
    assert(w2 >= 0 && h2 >= 0);

    return 0;
}
