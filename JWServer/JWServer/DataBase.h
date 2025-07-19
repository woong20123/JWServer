#pragma once
#ifndef __JW_DATA_BASE_H__
#define __JW_DATA_BASE_H__
#include <string>
#include <string_view>

namespace jw
{
    struct ConnectDBInfo
    {
        std::wstring ip;
        std::wstring dataBase;
        std::wstring userName;
        std::wstring password;
    };

    enum class DBSessionState
    {
        DB_SESSION_STATE_NONE = 0,
        DB_SESSION_STATE_INIT,
        DB_SESSION_STATE_CONNECTED,
        DB_SESSION_STATE_CLOSE,
        DB_SESSION_STATE_MAX,
    };

    constexpr const wchar_t* DBSessionStateStr[static_cast<size_t>(DBSessionState::DB_SESSION_STATE_MAX)] =
    {
        L"NONE",
        L"INIT",
        L"CONNECTED",
        L"CLOSE",
    };

    constexpr const wchar_t* GetDBSessionStateStr(const DBSessionState state)
    {
        return DBSessionStateStr[static_cast<size_t>(state)];
    }

    class DBSession
    {
    public:
        DBSession();
        virtual ~DBSession();
        virtual bool Initialize(void* _hEnv) = 0;
        virtual bool Connect(std::wstring_view _connectStr) = 0;
        virtual void Execute() = 0;
        virtual void DirectExecute(std::wstring_view query) = 0;
    };

    class DataBase
    {
    public:
        DataBase();
        virtual ~DataBase();

        virtual bool Initialize(ConnectDBInfo& info) = 0;
    };

    class TESTDB : public DataBase
    {
    public:
        TESTDB() = default;
        ~TESTDB() = default;
        bool Initialize(ConnectDBInfo& info) override { return true; }
    };

}

#endif // __JW_DATA_BASE_H__

