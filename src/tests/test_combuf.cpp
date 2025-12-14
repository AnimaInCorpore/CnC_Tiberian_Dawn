#include <cassert>
#include <cstring>
#include "legacy/combuf.h"

int main() {
    CommBufferClass buf(4,4,128);
    const char* msg = "hello";
    assert(buf.Queue_Send((void*)msg, strlen(msg)+1) == 1);
    assert(buf.Num_Send() == 1);
    SendQueueType* entry = buf.Get_Send(0);
    assert(entry != nullptr);
    assert(strcmp(entry->Buffer, msg) == 0);
    char out[128]; int outlen;
    assert(buf.UnQueue_Send(out, &outlen, 0) == 1);
    assert(strcmp(out, msg) == 0);
    return 0;
}
