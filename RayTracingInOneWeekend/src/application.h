#pragma once

#include <memory>

#include "tools/windowInfo.h"
#include "camera.h"
#include "sdlwindowrenderer.h"


class Application
{
public:
	Application();
	~Application();

	void Run();
	bool IsRunning() const { return m_running; }

private:
	bool m_running = true;
	//int m_samplesPerPixel = 4;
	int m_windowWidth = 1000;
	double m_aspectRatio = 16.0 / 9.0;
	int m_windowHeight = ((int)(m_windowWidth / m_aspectRatio) < 1) ? 1 : (int)(m_windowWidth / m_aspectRatio);
	WindowInfo m_windowInfo;
	std::unique_ptr<Camera> m_camera = nullptr;
	std::unique_ptr<color[]> m_imageBuffer = nullptr;
	std::unique_ptr<SDLWindowRenderer> m_windowRenderer = nullptr;
	HittableList m_world;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;

	void SetWorld();
	void HandleEvents();
	void Update();
	void Render();
};

