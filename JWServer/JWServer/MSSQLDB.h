#pragma once
#ifndef __JW_DB_SESSION_H__
#define __JW_DB_SESSION_H__
#include "DataBase.h"
#include <string_view>

namespace jw
{
    class MSSQLDBHelper
    {
    public:
        static void StatementErrorLog(void* handle, const wchar_t* addMsg);
        static void ConnectionErrorLog(void* handle, const wchar_t* addMsg);
        static void EnviromentErrorLog(void* handle, const wchar_t* addMsg);
    private:
        static void errorLog(int16_t handleType, void* handle, const wchar_t* addMsg);
    };

    class MSSQLDBSession : public DBSession
    {
    public:
        constexpr static const int DEFAULT_DB_STATMENT_TIMEOUT = 15; // 15초

        MSSQLDBSession();
        virtual ~MSSQLDBSession();
        bool Initialize(void* _hEnv) override;
        bool Connect(std::wstring_view _connectStr) override;
        void Execute() override;
        void DirectExecute(std::wstring_view query) override;
    private:
        void close();
        void setState(DBSessionState state);

        DBSessionState      _state{ DBSessionState::DB_SESSION_STATE_NONE };
        void* _hStmt;
        void* _hDbc;
        std::wstring        _connectStr;
    };

    class MSSQLDB : public DataBase
    {
    public:
        MSSQLDB();
        virtual ~MSSQLDB();

        bool Initialize(ConnectDBInfo& info) override;
    private:
        void freeHandle();

        void* _hEnv;
        std::wstring    _connectStr;
    };

}
#endif // !__JW_DB_SESSION_H__

