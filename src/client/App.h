#pragma once

class App
{
public:
	App();
	~App();

	int Tick();

	// false if the processed event has closed the window
	bool HandleEvents();

	RenderWindow _window;
private:
	void InitializeWindow();
	void AdjustViewport(const GLsizei& x, const GLsizei& y);
};

