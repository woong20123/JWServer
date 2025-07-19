#include "MSSQLDB.h"
#include "Logger.h"
#include <string>
#include <format>
#include <windows.h>
#include <sqlext.h>

namespace jw
{
    void MSSQLDBHelper::errorLog(int16_t handleType, void* handle, const wchar_t* addMsg)
    {
        SQLRETURN ret{};
        SQLSMALLINT index{ 0 };
        SQLWCHAR sqlState[SQL_SQLSTATE_SIZEW]{ 0 };
        SQLINTEGER native;
        SQLWCHAR errorMsg[SQL_MAX_MESSAGE_LENGTH]{ 0 };
        SQLSMALLINT	errMsgLen;

        do
        {
            ret = SQLGetDiagRec(handleType, handle, index, sqlState, &native, errorMsg, SQL_MAX_MESSAGE_LENGTH, &errMsgLen);
            if (SQL_SUCCEEDED(ret))
            {
                LOG_ERROR(L"errMsg : {}, nativeCode: {}, sqlState : {}, {}", errorMsg, native, sqlState, addMsg);
            }
        } while (ret == SQL_SUCCESS);
    }

    void MSSQLDBHelper::StatementErrorLog(void* handle, const  wchar_t* addMsg = L"")
    {
        errorLog(SQL_HANDLE_STMT, handle, addMsg);
    }
    void MSSQLDBHelper::ConnectionErrorLog(void* handle, const wchar_t* addMsg = L"")
    {
        errorLog(SQL_HANDLE_DBC, handle, addMsg);
    }

    void MSSQLDBHelper::EnviromentErrorLog(void* handle, const wchar_t* addMsg = L"")
    {
        errorLog(SQL_HANDLE_ENV, handle, addMsg);
    }

    MSSQLDBSession::MSSQLDBSession() = default;
    MSSQLDBSession::~MSSQLDBSession()
    {
        close();
    }

    bool MSSQLDBSession::Initialize(SQLHENV hEnv)
    {
        SQLRETURN ret{};
        // 연결 핸들 할당
        ret = SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &_hDbc);
        if (!SQL_SUCCEEDED(ret))
        {
            MSSQLDBHelper::ConnectionErrorLog(_hDbc, L"SQLAllocHandle SQL_HANDLE_DBC Fail");
            return false;
        }

        setState(DBSessionState::DB_SESSION_STATE_INIT);

        return true;
    }

    bool MSSQLDBSession::Connect(std::wstring_view connectStr)
    {
        _connectStr = connectStr;


        SQLRETURN ret{};
        ret = SQLDriverConnect(_hDbc, NULL, (SQLWCHAR*)_connectStr.c_str(), SQL_NTS, NULL, 0, NULL,
            SQL_DRIVER_COMPLETE);

        if (!SQL_SUCCEEDED(ret))
        {
            const auto& addMsg = std::format(L"Connect to MSSQLDB Fail, connectStr:{}", _connectStr.c_str());
            MSSQLDBHelper::ConnectionErrorLog(_hDbc, addMsg.c_str());
            return false;
        }

        ret = SQLAllocHandle(SQL_HANDLE_STMT, _hDbc, &_hStmt);

        if (!SQL_SUCCEEDED(ret))
        {
            const auto& addMsg = std::format(L"initialize _hStmt Fail, connectStr:{}", _connectStr.c_str());
            MSSQLDBHelper::StatementErrorLog(_hStmt, addMsg.c_str());
            return false;
        }

        SQLSetStmtAttr(_hStmt, SQL_ATTR_CONCURRENCY, (SQLPOINTER)SQL_CONCUR_READ_ONLY, 0);
        SQLSetStmtAttr(_hStmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER)(intptr_t)(DEFAULT_DB_STATMENT_TIMEOUT), 0);

        setState(DBSessionState::DB_SESSION_STATE_CONNECTED);
        LOG_INFO(L"Connect to MSSQLDB Success, connectStr:{}", _connectStr.c_str());

        return true;
    }

    void MSSQLDBSession::Execute()
    {
        if (_hStmt == SQL_NULL_HANDLE)
        {
            LOG_ERROR(L"Statement handle is null");
            return;
        }

        SQLRETURN ret = SQLExecute(_hStmt);
        if (!SQL_SUCCEEDED(ret))
        {
            MSSQLDBHelper::StatementErrorLog(_hStmt, L"fail Execute()");
            return;
        }

        LOG_DEBUG(L"Execute query Success");
    }

    void MSSQLDBSession::DirectExecute(std::wstring_view query)
    {
        if (_hStmt == SQL_NULL_HANDLE)
        {
            LOG_ERROR(L"Statement handle is null, cannot execute query:{}", query.data());
            return;
        }

        // 쿼리 실행
        SQLRETURN ret = SQLExecDirect(_hStmt, (SQLWCHAR*)query.data(), SQL_NTS);
        if (!SQL_SUCCEEDED(ret))
        {
            const auto& addMsg = std::format(L"Execute query Fail, query:{}", query.data());
            MSSQLDBHelper::StatementErrorLog(_hStmt, addMsg.c_str());
            return;
        }

        LOG_DEBUG(L"Execute query Success, query:{}", query.data());
    }

    void MSSQLDBSession::close()
    {
        if (_hDbc)
        {
            SQLDisconnect(_hDbc);
            SQLFreeHandle(SQL_HANDLE_DBC, _hDbc);
            _hDbc = SQL_NULL_HANDLE;
        }
        setState(DBSessionState::DB_SESSION_STATE_CLOSE);
        LOG_INFO(L"freeHandle to MSSQLDB Success, connectStr:{}", _connectStr.c_str());
    }

    void MSSQLDBSession::setState(DBSessionState state)
    {
        if (_state == state)
        {
            LOG_ERROR(L"equal state, state:{}", static_cast<int>(state));
            return;
        }

        LOG_INFO(L"DBSession state changed, before state:{}, after state:{}", GetDBSessionStateStr(_state), GetDBSessionStateStr(state));
        _state = state;
    }


    MSSQLDB::MSSQLDB() = default;
    MSSQLDB::~MSSQLDB()
    {
        freeHandle();
    }

    bool MSSQLDB::Initialize(ConnectDBInfo& info)
    {
        SQLRETURN ret{};
        // 환경 핸들 할당
        ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &_hEnv);
        if (!SQL_SUCCEEDED(ret))
        {
            MSSQLDBHelper::EnviromentErrorLog(_hEnv, L"SQLAllocHandle SQL_HANDLE_ENV Fail");
            return false;
        }

        ret = SQLSetEnvAttr(_hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
        if (!SQL_SUCCEEDED(ret))
        {
            MSSQLDBHelper::EnviromentErrorLog(_hEnv, L"SQLSetEnvAttr SQL_ATTR_ODBC_VERSION Fail");
            return false;
        }

        _connectStr = std::format(L"DRIVER={{ODBC Driver 17 for SQL Server}};SERVER={};DATABASE={};UID={};PWD={};",
            info.ip.c_str(), info.dataBase.c_str(), info.userName.c_str(), info.password.c_str()).c_str();

        LOG_DEBUG(L"Initialize MSSQLDB, connectStr:{}", _connectStr.c_str());
        return true;
    }

    void MSSQLDB::freeHandle()
    {
        if (_hEnv)
        {
            SQLFreeHandle(SQL_HANDLE_ENV, _hEnv);
            _hEnv = NULL;
        }
    }
}
