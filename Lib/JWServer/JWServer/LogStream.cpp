#include "LogStream.h"

namespace jw
{
	LogStream::LogStream() : _logTypeFlag{0}
	{
	}

	void LogStream::OnLogTypeFlag(const LogType flag)
	{
		_logTypeFlag |= static_cast<int16_t>(flag);
	}

	void LogStream::OnLogTypeFlags(const std::span<LogType> flags)
	{
		for (const auto flag : flags)
			OnLogTypeFlag(flag);
	}

	void LogStream::OffLogTypeFlag(const LogType flag)
	{
		_logTypeFlag &= ~(static_cast<int16_t>(flag));
	}
	void LogStream::OffLogTypeFlags(const std::span<LogType> flags)
	{
		for (const auto flag : flags)
			OffLogTypeFlag(flag);
	}

	bool LogStream::EnableLogType(const LogType flag)
	{
		return _logTypeFlag & static_cast<int16_t>(flag);
	}
}