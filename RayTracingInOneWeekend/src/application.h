#pragma once

#include <memory>

#include "camera.h"
#include "tracer.h"

class Application
{
public:
	Application();
	~Application();

	void Run();
	bool IsRunning() const { return m_running; }

private:
	int m_windowWidth = 1000;
	double m_aspectRatio = 16.0 / 9.0;
	int m_windowHeight = ((int)(m_windowWidth / m_aspectRatio) < 1) ? 1 : (int)(m_windowWidth / m_aspectRatio);

	bool m_running = true;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	Camera m_camera;
	std::unique_ptr<Tracer> m_tracer = nullptr;

	void HandleEvents();
	void Update();
	void Render();

	std::unique_ptr <color[]> m_imageBuffer = nullptr;
	void SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
};

