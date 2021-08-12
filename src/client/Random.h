#pragma once
#include <random>

class Random
{
private:
	std::mt19937 _engine;
	std::random_device _device;

public:
	Random();
	~Random();

	template<typename T>
	T Generate(const T & from, const T & to) {
		if constexpr (std::is_floating_point_v<T>) 
		{
			std::uniform_real_distribution<T> dist(from, to);
			return dist(_engine);
		}
		else 
		{
			std::uniform_int_distribution<T> dist(from, to);
			return dist(_engine);
		}
		return T(0);
	}

	bool Probability(float num);
};