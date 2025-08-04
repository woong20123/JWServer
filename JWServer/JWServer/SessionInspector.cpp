#include "SessionInspector.h"
#include "Logger.h"
#include "Network.h"
#include "Session.h"
#include "TypeDefinition.h"
#include "TimeUtil.h"
#include "ThreadManager.h"
#include "ThreadHelper.h"
#include <algorithm>

namespace jw
{
    SessionInspectorInfoTable::SessionInspectorInfoTable() : _sessionInfoTables{ nullptr }, _maxSessionCount{}, _portId{ INVALID_VALUE }, _recvCheckTimeSecond{ TimeUtil::HOUR_TO_SECOND }
    {
    }
    SessionInspectorInfoTable::~SessionInspectorInfoTable()
    {
        if (_sessionInfoTables)
        {
            delete[] _sessionInfoTables;
            _sessionInfoTables = nullptr;
        }
    }

    void  SessionInspectorInfoTable::Initialize(uint16_t portId, const int64_t maxSessionCount, const time_t recvCheckTimeSecond)
    {
        if (maxSessionCount <= 0 || 0 == portId)
        {
            LOG_ERROR(L"Invalid Parameter, portId:{}, maxSessionCount:{}", portId, maxSessionCount);
            return;
        }

        _maxSessionCount = maxSessionCount;
        _portId = portId;
        _recvCheckTimeSecond = recvCheckTimeSecond;
        _sessionInfoTables = new SessionInspectorInfo[_maxSessionCount];
        for (int i = 0; i < _maxSessionCount; ++i)
        {
            _sessionInfoTables[i]._sessionId = INVALID_SESSION_ID;
            _sessionInfoTables[i]._state = SESSION_INSPECTOR_STATUS_NONE;
        }
    }

    void SessionInspectorInfoTable::SetStateAllocated(const int32_t sessionIndex)
    {
        setState(sessionIndex, SESSION_INSPECTOR_STATUS_ALLOCATED);
        const auto session = GetNetwork().GetSession(_portId, sessionIndex);
        if (session)
            session->UpdateRecvTime();
    }
    void SessionInspectorInfoTable::SetStateReleased(const int32_t sessionIndex)
    {
        setState(sessionIndex, SESSION_INSPECTOR_STATUS_RLEASED);
    }

    void SessionInspectorInfoTable::Inspect()
    {
        time_t currentTime = TimeUtil::GetCurrentTimeT();
        for (int32_t i = 0; i < static_cast<int32_t>(_maxSessionCount); ++i)
        {
            int32_t sessionIndex = i;
            SessionInspectorInfo sessionInspectorInfo;
            {
                // Lock을 최소화 하기 위해서 임시 객체에 데이터를 복사합니다. 
                // 이런 구현이 가능한 이유는 데이터 검사의 약간의 오차가 발생해도 문제가 없다고 판단하기 때문입니다.
                READ_LOCK(_sessionInspectorTableMutex);
                sessionInspectorInfo = _sessionInfoTables[sessionIndex];
            }

            if (sessionInspectorInfo._state == SESSION_INSPECTOR_STATUS_NONE ||
                sessionInspectorInfo._state == SESSION_INSPECTOR_STATUS_RLEASED)
            {
                continue;
            }
            else if (sessionInspectorInfo._state == SESSION_INSPECTOR_STATUS_ALLOCATED)
            {
                const auto session = GetNetwork().GetSession(_portId, sessionIndex);
                if (session)
                {
                    if (session->GetLastRecvTime() + _recvCheckTimeSecond <= currentTime)
                    {
                        setState(sessionIndex, SESSION_INSPECTOR_STATUS_CANDIDATE_CLOSE);
                    }
                }
            }
            else if (sessionInspectorInfo._state == SESSION_INSPECTOR_STATUS_CANDIDATE_CLOSE)
            {
                const auto session = GetNetwork().GetSession(_portId, sessionIndex);
                if (session)
                {
                    if (session->GetLastRecvTime() + _recvCheckTimeSecond <= currentTime)
                    {
                        session->Close(CloseReason::CLOSE_REASON_INSPECTOR_TIMEOUT);
                        LOG_INFO(L"Session Inspector Timeout, sessionId:{}, portId:{}, sessionIndex:{}", sessionInspectorInfo._sessionId, _portId, sessionIndex);
                    }
                    else
                    {
                        setState(sessionIndex, SESSION_INSPECTOR_STATUS_ALLOCATED);
                    }
                }
                else
                {
                    LOG_ERROR(L"Session is nullptr, sessionId:{}, portId:{}, sessionIndex:{}", sessionInspectorInfo._sessionId, _portId, sessionIndex);
                }

            }
        }
    }

    void SessionInspectorInfoTable::setState(const uint64_t sessionIndex, const SessionInspectorStatus state)
    {
        if (sessionIndex < 0 || _maxSessionCount <= sessionIndex)
        {
            LOG_ERROR(L"Session Index is Invalid, sessionIndex:{}", sessionIndex);
            return;
        }

        uint8_t beforeState{ SESSION_INSPECTOR_STATUS_NONE };
        {
            WRITE_LOCK(_sessionInspectorTableMutex);
            beforeState = _sessionInfoTables[sessionIndex]._state;
            _sessionInfoTables[sessionIndex]._state = state;
        }
        LOG_DEBUG(L"Session Connection State Change, sessionIndex:{}, beforeState:{}, newState:{}", sessionIndex, static_cast<int>(beforeState), static_cast<int>(state));
    }

    SessionInspector::SessionInspector() : _checkIntervalMilliSecond{ DEFAULT_SESSION_INSPECTOR_CHECK_INTERVAL_SECOND }, _UpdateExecutionFunc{ nullptr }
    {
    }
    SessionInspector::~SessionInspector()
    {
    }

    void SessionInspector::Initialize(const bool isRun, const time_t checkIntervalMilliSecond)
    {
        _checkIntervalMilliSecond = checkIntervalMilliSecond;
    }

    void SessionInspector::RegisterTable(int16_t portId, std::shared_ptr<SessionInspectorInfoTable>& table)
    {
        WRITE_LOCK(_tablesMutex);
        _sessionInspectorInfoTables[portId] = table;
    }

    void SessionInspector::execute(std::stop_token stopToken)
    {
        // 세션을 검사하는 스레드입니다. 
        while (true)
        {
            if (stopToken.stop_requested())
            {
                LOG_INFO(L"SessionInspector Thread Stop");
                return;
            }

            _UpdateExecutionFunc();

            {
                READ_LOCK(_tablesMutex);
                std::for_each(_sessionInspectorInfoTables.begin(), _sessionInspectorInfoTables.end(),
                    [this](auto& sessionInspectorInfoTable) {
                        sessionInspectorInfoTable.second->Inspect();
                    });
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(_checkIntervalMilliSecond));
        }

        onCloseExecute();
    }


    void SessionInspector::RunThread()
    {
        using namespace std::placeholders;
        auto t = ThreadHelper::MakeThreadAndGetInfo(L"SessionInspector", _threadId, _UpdateExecutionFunc);
        t->SetExecution(std::bind(&SessionInspector::execute, this, _1));
        GetThreadManager().AddThread(std::move(t));
    }

    void SessionInspector::onCloseExecute()
    {
        if (_threadId != std::thread::id())
        {
            GetThreadManager().RemoveThread(_threadId);
        }

        if (_UpdateExecutionFunc)
            _UpdateExecutionFunc = nullptr;
    }

    void SessionInspector::Stop()
    {
        if (_threadId != std::thread::id())
        {
            GetThreadManager().StopThread(_threadId);
            LOG_INFO(L"SessionInspector Stop");
        }

    }
}
