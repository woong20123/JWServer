#pragma once
#ifndef __JW_SERIALIZER_TIMER_H__
#define __JW_SERIALIZER_TIMER_H__
#include "Timer.h"
#include <shared_mutex>
#include <list>
#include <array>

namespace jw
{
    struct SerializerKey;
    class SerializeObject;
    class SerializerTimer : public Timer
    {
    public:
        static constexpr int64_t SHORT_TERM_MAX_TICK = 300;
        static constexpr int64_t INVALID_TICK = -1;
        using SerializeList = std::list<std::shared_ptr<SerializeObject>>;
        using SerializeListArray = std::array<SerializeList, SHORT_TERM_MAX_TICK>;

        SerializerTimer(SerializerKey& serializerKey);
        void OnTimer() override;
        bool Post(const std::shared_ptr<SerializeObject>& so, int32_t delayMilliSeconds);
    private:
        // _postObjectsMutex lock을 사용한 곳에서 호출 해야 합니다. 
        bool post(const std::shared_ptr<SerializeObject>& so);
        void updateExecuteTick();
        int64_t getNextTick(int64_t nextTick) const;
        const bool getIsLongTermCheckTerm();


        SerializerKey& _serializerKey;
        SerializeListArray                              _shortTermObjects;
        SerializeList                                   _longTermObjects;
        std::shared_mutex		                        _shortTermObjectsMutex;
        std::shared_mutex		                        _longTermObjectsMutex;
        int64_t                                         _executeTick;
    };
}
#endif // !__JW_SERIALIZER_TIMER_H__