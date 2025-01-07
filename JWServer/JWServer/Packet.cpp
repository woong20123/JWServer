#include "Packet.h"
#include "PacketBuffer.h"
#include "TypeDefinition.h"

namespace jw
{
    Packet::Packet() : _header{ nullptr }
    { }

    bool Packet::Add(const void* data, Packet::packetSize size)
    {
        if (!_header)
            return false;

        if (size < 0 || getFreeSize() < size)
        {
            MAKE_CRASH;
            return false;
        }

        void* curPos = ((char*)_header + _header->_size);
        ::memcpy(curPos, data, size);
        _header->_size += size;
        return true;
    }

    bool Packet::IsSet()  const
    {
        return nullptr != _header;
    }

    void Packet::SetBuffer(void* buffer)
    {
        _header = reinterpret_cast<Header*>(buffer);
    }

    void Packet::SetPacketBuffer(std::shared_ptr<PacketBuffer>& buffer)
    {
        _packetBuffer = buffer;
        _header = reinterpret_cast<Header*>(_packetBuffer->GetBuffer());
        _header->_size = sizeof(Header);

    }

    Packet::Header* Packet::GetHeader()  const
    {
        return _header;
    }
    char* Packet::GetBody() const
    {
        return reinterpret_cast<char*>(_header + 1);
    }

    Packet::packetSize Packet::GetBodySize() const
    {
        if (getSize() == 0) return 0;
        return getSize() - HEADER_SIZE;
    }

    Packet::packetSize Packet::GetTotalSize() const
    {
        if (getSize() == 0) return 0;
        return getSize();
    }

    Packet::packetSize Packet::getSize() const
    {
        return _header ? _header->_size : 0;
    }

    Packet::packetSize Packet::getFreeSize() const
    {
        return _header ? MAX_SIZE - _header->_size : 0;
    }
}
