#include "GamePacketHandler.h"
#include "Logger.h"
#include "Packet.h"

namespace jw
{
    bool GamePacketHandler::RegistHandler(const cmdType cmd, HandlerType handler)
    {
        const auto [iter, isSuccess] = _packetHandlersMap.insert({ cmd, handler });

        if (!isSuccess)
        {
            LOG_ERROR(L"fail registe handler, cmd:{}", cmd);
        }

        return isSuccess;
    }

    bool GamePacketHandler::HandlePacket(const Session* session, const Packet& packet)
    {
        const cmdType cmd = *reinterpret_cast<cmdType*>(packet.GetBody());

        if (!_packetHandlersMap.contains(cmd))
        {
            LOG_DEBUG(L"not find cmd, cmd:{}", cmd);
            return false;
        }

        const auto isSuccess = _packetHandlersMap[cmd](session, packet);
        LOG_DEBUG(L"handle packet, cmd:{}, isSuccess:{}", cmd, isSuccess);
        return true;;
    }
}

