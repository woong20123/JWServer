#include "Packet.h"
#include "PacketBuffer.h"
#include "PacketBufferPool.h"
#include "TypeDefinition.h"

namespace jw
{
    struct PacketBufferDelter
    {
        void operator()(PacketBuffer* obj) const
        {
            PACKET_BUFFER_POOL().Release(obj);
        }
    };

    struct Impl
    {
        Impl() : _header{ nullptr }, _packetBuffer{ nullptr }
        { }
        using PacketBufferPtr = std::unique_ptr<PacketBuffer, PacketBufferDelter>;
        Packet::Header* _header;
        PacketBufferPtr _packetBuffer;

        void setPacketBuffer(PacketBufferPtr& buffer)
        {
            _packetBuffer = std::move(buffer);
            _header = reinterpret_cast<Packet::Header*>(_packetBuffer->GetBuffer());
            _header->_size = sizeof(Packet::Header);
        }
    };

    Packet::Packet() : _impl{ nullptr }
    {
        _impl = std::make_unique<Impl>();
    }
    Packet::~Packet() = default;

    void Packet::Allocate()
    {
        std::unique_ptr<PacketBuffer, PacketBufferDelter> packetBuffer{ PACKET_BUFFER_POOL().Acquire() };
        _impl->setPacketBuffer(packetBuffer);
    }

    bool Packet::Add(const void* data, Packet::packetSize size)
    {
        if (!_impl->_header)
            return false;

        if (size < 0 || getFreeSize() < size)
        {
            MAKE_CRASH("packet size error");
            return false;
        }

        void* curPos = ((char*)_impl->_header + _impl->_header->_size);
        ::memcpy(curPos, data, size);
        _impl->_header->_size += size;
        return true;
    }

    bool Packet::IsSet()  const
    {
        return nullptr != _impl->_header;
    }

    void Packet::SetBuffer(void* buffer)
    {
        _impl->_header = reinterpret_cast<Header*>(buffer);
    }

    Packet::Header* Packet::GetHeader()  const
    {
        return _impl->_header;
    }
    char* Packet::GetBody() const
    {
        return reinterpret_cast<char*>(_impl->_header + 1);
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
        return _impl->_header ? _impl->_header->_size : 0;
    }

    Packet::packetSize Packet::getFreeSize() const
    {
        return _impl->_header ? PacketBuffer::BUFFER_SIZE - _impl->_header->_size : 0;
    }
}
