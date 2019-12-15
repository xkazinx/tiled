#include "stdafx.h"
#include "FpsCounter.h"

FpsCounter::FpsCounter(const string & name) : _name(name)
{
}

FpsCounter::~FpsCounter()
{
}

auto FpsCounter::Reset()
{
	auto count = _count;
	_clock.restart();
	_count = 0;

	return count;
}

void FpsCounter::Tick()
{
	_count++;

	if (Elapsed()) {
		cout << _name << " fps: " << Reset() << endl;
	}
}
