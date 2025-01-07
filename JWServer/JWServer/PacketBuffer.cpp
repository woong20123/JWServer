#include "PacketBuffer.h"
#include <string>

namespace jw
{
    PacketBuffer::PacketBuffer()
    {
        ::memset(_buffer, 0x00, sizeof(BufferType) * BUFFER_SIZE);
    }

    PacketBuffer::~PacketBuffer()
    {}

    void* PacketBuffer::GetBuffer()
    {
        return reinterpret_cast<void*>(_buffer);
    }
}
