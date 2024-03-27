#pragma once

#include <memory>
#include <array>

#include "tools/windowInfo.h"
#include "camera.h"
#include "sdlwindowrenderer.h"
#include "input/input.h"

#include "imgui.h"
#include "imgui_impl_sdlrenderer2.h"
#include "imgui_impl_sdl2.h"
#include <stdio.h>

class Application
{
public:
	Application();
	~Application();

	void Run();
	bool IsRunning() const { return m_running; }

private:
	bool   m_running = true;
	int    m_windowWidth = 1000;
	double m_aspectRatio = 16.0 / 9.0;
	int    m_windowHeight = ((int)(m_windowWidth / m_aspectRatio) < 1) ? 1 : (int)(m_windowWidth / m_aspectRatio);
	double m_deltaTime = 0.0;
	double m_averagedeltaTime = 0.0;
	const bool m_hasRandomSpheres = false;

	WindowInfo m_windowInfo;
	color* m_imageBuffer = nullptr;
	std::unique_ptr<Camera> m_camera = nullptr;
	std::unique_ptr<SDLWindowRenderer> m_windowRenderer = nullptr;
	HittableList m_world;
	SDL_Window* m_window;
	SDL_Renderer* m_renderer;
	Input m_input;

	bool m_showGuiDemo = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	bool m_applyBloom = false;
	bool m_lockInput = false;

	void SetWorld();
	void AddRandomSpheres(HittableList& world);
	void HandleEvents();
	void Update();
	void Render();
};