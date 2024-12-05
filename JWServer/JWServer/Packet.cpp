#include "Packet.h"
#include "TypeDefinition.h"

namespace jw
{
    Packet::Packet() : _header{ nullptr }
    { }

    void Packet::Add(const void* data, Packet::packetSize size)
    {
        if (size < 0 || getFreeSize() < size)
        {
            MAKE_CRASH;
            return;
        }
    }

    bool Packet::IsSet()  const
    {
        return nullptr != _header;
    }

    void Packet::SetBuffer(void* buffer)
    {
        _header = reinterpret_cast<Header*>(buffer);
    }

    Packet::Header* Packet::GetHeader()  const
    {
        return _header;
    }
    void* Packet::GetBody() const
    {
        return (_header + 1);
    }

    Packet::cmdType Packet::GetCmd() const
    {
        return *reinterpret_cast<cmdType*>(GetBody());
    }

    Packet::packetSize Packet::getSize()
    {
        return _header ? _header->_size : 0;
    }

    Packet::packetSize Packet::getFreeSize()
    {
        return _header ? MAX_SIZE - _header->_size : 0;
    }
}
