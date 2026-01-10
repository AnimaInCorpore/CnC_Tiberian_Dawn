#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>

/*
 * Legacy DPMI/DOS segmented memory helper. The original Watcom build used this
 * to access low DOS memory and hardware-mapped regions (e.g. text mode video).
 *
 * In the portable SDL build there is no concept of real-mode segments, so this
 * class is implemented as a simple heap-backed byte buffer that preserves the
 * public interface for legacy call sites.
 */

extern void output(short port, short data);

class DOSSegmentClass {
    unsigned int Selector;
    unsigned char *Buffer;
    std::size_t BufferSize;

    void Ensure_Size(std::size_t size);

public:
    DOSSegmentClass(void);
    ~DOSSegmentClass(void);
    DOSSegmentClass(unsigned short segment, long size = (1024L * 64L));

    unsigned int Get_Selector(void);
    void Assign(unsigned short segment, long size = (1024L * 64L));

    void Copy_To(void *source, int dest, int size);
    void Copy_From(void *dest, int source, int size);

    void Copy_Word_To(short data, int dest);
    void Copy_Byte_To(char data, int dest);
    void Copy_DWord_To(long data, int dest);

    short Copy_Word_From(int source);
    char Copy_Byte_From(int source);
    long Copy_DWord_From(int source);

    static void Copy(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size);
    static void Swap(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size);
};

