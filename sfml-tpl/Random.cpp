#include "stdafx.h"
#include "Random.h"

Random::Random()
{
	_mt = mk<std::mt19937>(_rd());
}

Random::~Random()
{
}

bool Random::Probability(const float & num)
{
	if(num > 1.f)
		return true;
	else if(num < 0.f)
		return false;

	std::bernoulli_distribution d(num);
	return d(get());
}
