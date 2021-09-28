#pragma once
#include <mutex>

enum class ConsoleType
{
	Out,
	Log,
	Warn,
	Err,
};

inline thread_local string kBuffer;

template<ConsoleType T>
class ConsoleOut
{
public:
	using Type = ConsoleType;

	ConsoleOut(string& str) 
		: _str(str)
	{

	}

	~ConsoleOut();

	ConsoleOut(const char* str)
		: _str(kBuffer)
	{
		kBuffer = str;
	}

#ifndef CLIENT
	std::mutex _mutex;
#endif
	string& _str;
};

namespace Log
{
	using Type = ConsoleType; 
	template<Type T> using Cout = ConsoleOut<T>;	

	template <typename ... Args>
	static inline void Out(Args && ... args) 
	{
		Cout<Type::Out>(format(forward<Args>(args)...).c_str());
	}

// #todo
	/*

	template <typename ... Args>
	void Info(Args && ... args)
	{
		Cout<Type::Log>(format(std::forward<Args>(args)...));
	}*/
	/*

	template <typename ... Args>
	void Warn(Args && ... args)
	{
		Cout<Type::Warn>(format(std::forward<Args>(args)...));
	}
*/

	template <typename ... Args>
	static inline void Err(Args && ... args)
	{
		Cout<Type::Err>(format(forward<Args>(args)...).c_str());
	}
}