#pragma once
#ifndef __JW_SESSION_INSPECTOR_H__
#define __JW_SESSION_INSPECTOR_H__
#include <thread>
#include <atomic>
#include <cstdint>
#include <shared_mutex>
#include <unordered_map>

namespace jw {
    class Port;
    struct SessionInspectorInfo
    {
        uint64_t _sessionId;
        uint8_t _state;
    };

    class SessionInspectorInfoTable
    {
    public:
        enum SessionInspectorStatus
        {
            SESSION_INSPECTOR_STATUS_NONE = 0,
            SESSION_INSPECTOR_STATUS_ALLOCATED,
            SESSION_INSPECTOR_STATUS_CANDIDATE_CLOSE,
            SESSION_INSPECTOR_STATUS_RLEASED,
            SESSION_INSPECTOR_STATUS_MAX
        };

        static constexpr int64_t INVALID_SESSION_ID = 0;

        SessionInspectorInfoTable();
        ~SessionInspectorInfoTable();
        void Initialize(uint16_t portId, const int64_t maxSessionCount, const time_t recvCheckTimeSecond);
        void SetStateAllocated(const int32_t sessionIndex);
        void SetStateReleased(const int32_t sessionIndex);
        int16_t GetPortId() const { return _portId; }

        void Inspect();
    private:
        void setState(const uint64_t sessionIndex, const SessionInspectorStatus state);

        int16_t                         _portId;
        std::shared_mutex               _sessionInspectorTableMutex;
        SessionInspectorInfo* _sessionInfoTables;
        size_t                          _maxSessionCount;
        time_t                          _recvCheckTimeSecond;
    };

    class SessionInspector
    {
    public:
        using SessionInspectorInfoTableContainer = std::unordered_map<int16_t, std::shared_ptr<SessionInspectorInfoTable>>;
        static constexpr int64_t DEFAULT_SESSION_INSPECTOR_CHECK_INTERVAL_SECOND = 60 * 60;

        SessionInspector();
        ~SessionInspector();

        void Initialize(const bool isRun, const time_t checkIntervalMilliSecond);
        void RegisterTable(int16_t portId, std::shared_ptr<SessionInspectorInfoTable>& table);
        void Run();
        void Stop();
    private:
        std::shared_mutex                   _tablesMutex;
        SessionInspectorInfoTableContainer _sessionInspectorInfoTables;
        std::thread _inspectorThread;
        bool        _isRun;
        time_t      _checkIntervalMilliSecond;

    };
}

#endif // !__JW_SESSION_INSPECTOR_H__

