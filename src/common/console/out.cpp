#include "stdafx.h"
#include "out.h"
#include "rang.hpp"

template<ConsoleType T>
ConsoleOut<T>::~ConsoleOut()
{
	// #todo
#ifndef CLIENT
	std::unique_lock<std::mutex> lock(_mutex);
#endif

	switch (T)
	{
	case Type::Out:		std::cout << rang::fg::green;	break;
	case Type::Err:		std::cout << rang::fg::red;		break;
	case Type::Warn:	std::cout << rang::fg::magenta;	break;
	case Type::Log:		std::cout << rang::fg::yellow;	break;
	}

	std::cout << _str.c_str() << "\n";
}

template ConsoleOut<ConsoleType::Out>::~ConsoleOut<ConsoleType::Out>();
//template ConsoleOut<ConsoleType::Log>::~ConsoleOut<ConsoleType::Log>();
//template ConsoleOut<ConsoleType::Warn>::~ConsoleOut<ConsoleType::Warn>();
template ConsoleOut<ConsoleType::Err>::~ConsoleOut<ConsoleType::Err>();