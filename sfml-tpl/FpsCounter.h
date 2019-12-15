#pragma once
class FpsCounter
{
public:
	FpsCounter(const string & name);
	~FpsCounter();

	// Return fps count after restarting clock and counter
	auto Reset();

	// Increase counter and check for time
	void Tick();

private:
	u16 _count = 0;
	string _name;
	sf::Clock _clock;

	// If time has elapsed for reset
	auto Elapsed() const {
		return _clock.getElapsedTime().asSeconds() >= 1;
	}
};

