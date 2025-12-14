#include <cassert>
#include <cstring>
#include "legacy/comqueue.h"

int main() {
    CommQueueClass q(4,4,128);
    const char* msg = "hello";
    assert(q.Queue_Send((void*)msg, strlen(msg)+1) == 1);
    assert(q.Next_Send() != nullptr);
    char out2[128]; int len2;
    assert(q.UnQueue_Send(out2, &len2) == 1);
    assert(strcmp(out2, msg) == 0);
    return 0;
}
