#pragma once
#ifndef __JW_BAD_IP_BLOCK_H__
#define __JW_BAD_IP_BLOCK_H__
#include <cstdint>
#include <unordered_map>
#include <shared_mutex>

namespace jw
{
    class BadIpBlock
    {
    public:
        using IPAddress = uint32_t;
        static constexpr IPAddress INVALID_IP_ADDRESS = 0xFFFFFFFF;

        struct BadIpEntry
        {
            static constexpr uint32_t MAX_BAD_IP_COUNT = 1000;
            uint32_t    _checkedCount;
            int64_t     _expiredTimeSecond;
        };

        struct BadIpOption
        {
            static constexpr int64_t DEFAULT_SACTION_TIME_SECOND = 60 * 60 * 1; // 1시간
            static constexpr int32_t DEFAULT_TRIGGERING_SESSION_COUNT = 5000;
            static constexpr int32_t DEFAULT_THRESHOLD_CHECKED_COUNT = 3;

            int64_t _sanctionsTimeSecond;
            int32_t _triggeringSessionCount;
            int32_t _thresholdCheckedCount;
        };

        BadIpBlock() = default;
        virtual ~BadIpBlock() = default;

        virtual void Initalize(const BadIpOption& option) = 0;
        virtual void RegisterBadIp(IPAddress) = 0;
        virtual const bool IsBadIp(IPAddress, int32_t sessionCount) = 0;
    };

    class EmptyBadIpBlock : public BadIpBlock
    {
    public:
        void Initalize(const BadIpOption& option) override {};
        void RegisterBadIp(IPAddress address) override {}
        const bool IsBadIp(IPAddress address, int32_t sessionCount) override { return false; }
    };

    class DefaultBadIpBlock : public BadIpBlock
    {
    public:
        static constexpr uint64_t DETECTED_BAD_IP_LOG_COUNT = 1000;
        using BadIpEntryList = std::unordered_map<IPAddress, BadIpEntry>;
        DefaultBadIpBlock();
        ~DefaultBadIpBlock() override;

        void Initalize(const BadIpOption& option) override;
        void RegisterBadIp(IPAddress address) override;
        const bool IsBadIp(IPAddress address, int32_t sessionCount) override;
    private:
        BadIpOption _option;
        BadIpEntryList _badIpList;
        std::shared_mutex _badIpListMutex;
        std::atomic<uint64_t> _detectedbadIPCount;
    };
}
#endif // __JW_BAD_IP_BLOCK_H__

