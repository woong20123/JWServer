#include "BadIpBlock.h"
#include "TimeUtil.h"
#include "TypeDefinition.h"
#include "NetworkHelper.h"
#include "Logger.h"

namespace jw
{
    DefaultBadIpBlock::DefaultBadIpBlock() : _option{ BadIpOption::DEFAULT_SACTION_TIME_SECOND, BadIpOption::DEFAULT_TRIGGERING_SESSION_COUNT, BadIpOption::DEFAULT_THRESHOLD_CHECKED_COUNT }
    {

    }

    DefaultBadIpBlock::~DefaultBadIpBlock()
    {

    }

    void DefaultBadIpBlock::Initalize(const BadIpOption& option)
    {
        _option._sanctionsTimeSecond = option._sanctionsTimeSecond;
        _option._triggeringSessionCount = option._triggeringSessionCount;
        _option._thresholdCheckedCount = option._thresholdCheckedCount;
    }

    void DefaultBadIpBlock::RegisterBadIp(IPAddress address)
    {
        if (address == INVALID_IP_ADDRESS) return;

        const auto sanctionsEndTime = TimeUtil::GetCurrentUTCTimeSecond() + _option._sanctionsTimeSecond;
        static constexpr uint32_t FIRST_COUNT{ 1 };
        size_t totalRegisteredCount{ 0 };
        uint32_t checkedCount{ FIRST_COUNT };
        {
            WRITE_LOCK(_badIpListMutex);
            totalRegisteredCount = _badIpList.size();
            if (const auto it = _badIpList.find(address);
                _badIpList.end() == it)
            {
                _badIpList[address] = { checkedCount, sanctionsEndTime };
            }
            else
            {
                _badIpList[address]._checkedCount;
                _badIpList[address]._expiredTimeSecond = sanctionsEndTime;
            }
        }

        LOG_INFO(L"[DefaultBadIpBlock] {} is Regist BadIp, sanctionsEndTime:{}, totalRegisteredCount:{}", NetworkHelper::GetAddressStringW(address).c_str(), sanctionsEndTime, totalRegisteredCount);
    }

    const bool DefaultBadIpBlock::IsBadIp(IPAddress address, int32_t sessionCount)
    {
        if (address == INVALID_IP_ADDRESS) return false;

        if (sessionCount < _option._triggeringSessionCount) return false;

        const auto now = TimeUtil::GetCurrentUTCTimeSecond();
        int64_t expirationTime{};
        int32_t checkedCount{};

        {
            READ_LOCK(_badIpListMutex);
            if (_badIpList.end() == _badIpList.find(address))
            {
                return false;
            }

            expirationTime = _badIpList[address]._expiredTimeSecond;
            checkedCount = _badIpList[address]._checkedCount;
        }

        if (expirationTime < now)
        {
            WRITE_LOCK(_badIpListMutex);
            _badIpList.erase(address);
            return false;
        }

        if (checkedCount < _option._thresholdCheckedCount)
        {
            return false;
        }

        const auto count = _detectedbadIPCount.fetch_add(1, std::memory_order_relaxed);
        if (count % DETECTED_BAD_IP_LOG_COUNT == 0)
        {
            LOG_INFO(L"[DefaultBadIpBlock] {:s} is detected. expirationTime:{:d}, checkedCount:{:d}, count:{:d}", NetworkHelper::GetAddressStringW(address), expirationTime, checkedCount, count);
        }
        return true;
    }
}
