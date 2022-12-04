#pragma once

enum class LogType
{
	Debug,
	Error,
	Info,
};

inline thread_local string kDummy;

template<LogType T>
class ConsoleOut
{
public:
	using Type = LogType;

	ConsoleOut(const char * str)
		: _ptr(str)
	{

	}

	~ConsoleOut();

#ifndef CLIENT
	mutex _mutex;
#endif
	const char * _ptr;
};

namespace Log
{
	using Type = LogType;	

	template <typename ... Args>
	void Debug(std::_Fmt_string<Args...> fmt, Args &&... args)
	{
		auto str = std::vformat(fmt._Str, std::make_format_args(args...));
		ConsoleOut<Type::Debug>(str.c_str());
	}

	template <typename ...Args>
	void Error(std::_Fmt_string<Args...> fmt, Args &&... args)
	{
		auto str = std::vformat(fmt._Str, std::make_format_args(args...));
		ConsoleOut<Type::Error>(str.c_str());
	}
}

template <typename ...Args>
void Print(std::_Fmt_string<Args...> fmt, Args &&... args)
{
	auto str = std::vformat(fmt._Str, std::make_format_args(args...));
	ConsoleOut<LogType::Info>(str.c_str());
}