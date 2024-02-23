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
	bool m_running = true;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	Camera m_camera;
	std::unique_ptr<Tracer> m_tracer = nullptr;


	void HandleEvents();
	void Update();
	void Render();
};

