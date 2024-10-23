#include "LogWorker.h"
#include "LogBuffer.h"
//#include "LogStream.h"
#include <memory>
#include <array>

namespace jw
{
    // 작업 처리 내용을 등록합니다. 
    void LogWorker::handle(const queueObject& obj)
    {
        obj->MakePreFix();
        std::cout << obj->GetPrefix() << obj->GetMsg() << "\r\n";
    }
}
