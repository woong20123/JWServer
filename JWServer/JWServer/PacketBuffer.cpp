#include "PacketBuffer.h"
#include <string>

namespace jw
{
    PacketBuffer::PacketBuffer() : _setBufferSize{ 0 }
    {
        clearBuffer();
    }

    PacketBuffer::~PacketBuffer()
    {}

    void* PacketBuffer::GetBuffer()
    {
        return reinterpret_cast<void*>(_buffer);
    }

    void PacketBuffer::Initialize()
    {
        _setBufferSize = 0;
        clearBuffer();
    }

    void PacketBuffer::clearBuffer()
    {
        ::memset(_buffer, 0x00, sizeof(BufferType) * BUFFER_SIZE);
    }
}
