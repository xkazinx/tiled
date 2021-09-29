#include "stdafx.h"
#include "log.h"
#include "rang.hpp"

template<LogType T>
ConsoleOut<T>::~ConsoleOut()
{
	// #todo
#ifndef CLIENT
	unique_mutex lock(_mutex);
#endif

	switch (T)
	{
	case Type::Debug:	std::cout << rang::fg::cyan;	break;
	case Type::Error:	std::cout << rang::fg::red;		break;
	case Type::Info:	std::cout << rang::fg::green;	break;
	}

	std::cout << (const char*)(_ptr ? _ptr : _str.c_str()) << "\n";
}

template ConsoleOut<LogType::Debug>::~ConsoleOut<LogType::Debug>();
template ConsoleOut<LogType::Error>::~ConsoleOut<LogType::Error>();
template ConsoleOut<LogType::Info>::~ConsoleOut<LogType::Info>();
