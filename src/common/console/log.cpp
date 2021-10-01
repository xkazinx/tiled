#include "stdafx.h"
#include "log.h"
#include "rang.hpp"

template<LogType T>
ConsoleOut<T>::~ConsoleOut()
{
	// #todo #thread
#ifndef CLIENT
	unique_mutex lock(_mutex);
#endif

	// #todo #log Log into files what's printed for release mode if enabled for debugging
	
	using enum rang::fg;

	switch (T)
	{
	case Type::Debug:	cout << cyan << "Debug: ";	break;
	case Type::Error:	cout << red << "Error: ";	break;
	case Type::Info:	cout << green;				break;
	}

	cout << (const char*)(_ptr ? _ptr : _str.c_str()) << "\n";
}

template ConsoleOut<LogType::Debug>::~ConsoleOut<LogType::Debug>();
template ConsoleOut<LogType::Error>::~ConsoleOut<LogType::Error>();
template ConsoleOut<LogType::Info>::~ConsoleOut<LogType::Info>();
