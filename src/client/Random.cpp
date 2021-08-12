#include "stdafx.h"
#include "Random.h"

Random::Random()
	: _engine(_device())
{

}

Random::~Random()
{
}

bool Random::Probability(float num)
{
	if(num > 1.f)
		return true;
	else if(num < 0.f)
		return false;

	std::bernoulli_distribution dist(num);
	return dist(_engine);
}
