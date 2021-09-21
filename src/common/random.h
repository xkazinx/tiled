#pragma once
#include <random>

namespace common
{
class Random
{
	std::mt19937 _engine;
	std::random_device _device;

public:
	Random();
	~Random();

	template<typename T>
	T Generate(const T & from, const T & to)
	{
		if constexpr (std::is_floating_point_v<T>) 
		{
			return std::uniform_real_distribution<T>(from, to)(_engine);
		}
		else 
		{
			return std::uniform_int_distribution<T>(from, to)(_engine);
		}
	}

	bool Probability(const float num);
};
}