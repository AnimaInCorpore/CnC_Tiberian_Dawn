#include "function.h"

#include "dpmi.h"

static std::size_t Clamp_Copy_Size(std::size_t offset, std::size_t size, std::size_t total)
{
    if (offset >= total) {
        return 0;
    }
    if (size > total - offset) {
        return total - offset;
    }
    return size;
}

void output(short, short)
{
    // No-op in the portable build (legacy hardware port I/O).
}

DOSSegmentClass::DOSSegmentClass(void)
    : Selector(0), Buffer(0), BufferSize(0)
{
    Ensure_Size(1024U * 64U);
}

DOSSegmentClass::~DOSSegmentClass(void)
{
    std::free(Buffer);
    Buffer = 0;
    BufferSize = 0;
}

DOSSegmentClass::DOSSegmentClass(unsigned short segment, long size)
    : Selector(0), Buffer(0), BufferSize(0)
{
    Assign(segment, size);
}

void DOSSegmentClass::Ensure_Size(std::size_t size)
{
    if (size <= BufferSize) {
        return;
    }
    unsigned char *new_buffer = static_cast<unsigned char *>(std::realloc(Buffer, size));
    if (!new_buffer) {
        return;
    }
    if (size > BufferSize) {
        std::memset(new_buffer + BufferSize, 0, size - BufferSize);
    }
    Buffer = new_buffer;
    BufferSize = size;
}

unsigned int DOSSegmentClass::Get_Selector(void)
{
    return Selector;
}

void DOSSegmentClass::Assign(unsigned short segment, long size)
{
    Selector = static_cast<unsigned int>(static_cast<unsigned long>(segment) << 4U);
    if (size < 0) {
        size = 0;
    }
    Ensure_Size(static_cast<std::size_t>(size));
}

void DOSSegmentClass::Copy_To(void *source, int dest, int size)
{
    if (!Buffer || !source || size <= 0 || dest < 0) {
        return;
    }
    std::size_t safe_size = Clamp_Copy_Size(static_cast<std::size_t>(dest),
                                            static_cast<std::size_t>(size),
                                            BufferSize);
    if (!safe_size) {
        return;
    }
    std::memmove(Buffer + dest, source, safe_size);
}

void DOSSegmentClass::Copy_From(void *dest, int source, int size)
{
    if (!Buffer || !dest || size <= 0 || source < 0) {
        return;
    }
    std::size_t safe_size = Clamp_Copy_Size(static_cast<std::size_t>(source),
                                            static_cast<std::size_t>(size),
                                            BufferSize);
    if (!safe_size) {
        return;
    }
    std::memmove(dest, Buffer + source, safe_size);
}

void DOSSegmentClass::Copy_Word_To(short data, int dest)
{
    Copy_To(&data, dest, static_cast<int>(sizeof(data)));
}

void DOSSegmentClass::Copy_Byte_To(char data, int dest)
{
    Copy_To(&data, dest, static_cast<int>(sizeof(data)));
}

void DOSSegmentClass::Copy_DWord_To(long data, int dest)
{
    Copy_To(&data, dest, static_cast<int>(sizeof(data)));
}

short DOSSegmentClass::Copy_Word_From(int source)
{
    short data = 0;
    Copy_From(&data, source, static_cast<int>(sizeof(data)));
    return data;
}

char DOSSegmentClass::Copy_Byte_From(int source)
{
    char data = 0;
    Copy_From(&data, source, static_cast<int>(sizeof(data)));
    return data;
}

long DOSSegmentClass::Copy_DWord_From(int source)
{
    long data = 0;
    Copy_From(&data, source, static_cast<int>(sizeof(data)));
    return data;
}

void DOSSegmentClass::Copy(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size)
{
    if (!src.Buffer || !dest.Buffer || size <= 0 || soffset < 0 || doffset < 0) {
        return;
    }

    std::size_t src_size = Clamp_Copy_Size(static_cast<std::size_t>(soffset),
                                           static_cast<std::size_t>(size),
                                           src.BufferSize);
    std::size_t dest_size = Clamp_Copy_Size(static_cast<std::size_t>(doffset),
                                            static_cast<std::size_t>(size),
                                            dest.BufferSize);
    std::size_t safe_size = src_size < dest_size ? src_size : dest_size;
    if (!safe_size) {
        return;
    }
    std::memmove(dest.Buffer + doffset, src.Buffer + soffset, safe_size);
}

void DOSSegmentClass::Swap(DOSSegmentClass &src, int soffset, DOSSegmentClass &dest, int doffset, int size)
{
    if (!src.Buffer || !dest.Buffer || size <= 0 || soffset < 0 || doffset < 0) {
        return;
    }

    std::size_t src_size = Clamp_Copy_Size(static_cast<std::size_t>(soffset),
                                           static_cast<std::size_t>(size),
                                           src.BufferSize);
    std::size_t dest_size = Clamp_Copy_Size(static_cast<std::size_t>(doffset),
                                            static_cast<std::size_t>(size),
                                            dest.BufferSize);
    std::size_t safe_size = src_size < dest_size ? src_size : dest_size;
    if (!safe_size) {
        return;
    }

    unsigned char *src_ptr = src.Buffer + soffset;
    unsigned char *dest_ptr = dest.Buffer + doffset;

    for (std::size_t idx = 0; idx < safe_size; ++idx) {
        unsigned char tmp = src_ptr[idx];
        src_ptr[idx] = dest_ptr[idx];
        dest_ptr[idx] = tmp;
    }
}

