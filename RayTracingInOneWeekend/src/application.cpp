#include "application.h"

#include <chrono>

#include "SDL.h"


Application::Application()
{
	WindowInfo windowInfo =
	{
		16.0 / 9.0,
		1000
	};

	int height = (int)(windowInfo.width / windowInfo.aspectRatio);
	height = (height < 1) ? 1 : height;

	SDL_Init(SDL_INIT_EVERYTHING);
	m_window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowInfo.width, height, SDL_WINDOW_RESIZABLE);
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	m_renderer = SDL_CreateRenderer(m_window, -1, flags);

	m_tracer = std::make_unique<Tracer>(m_camera, windowInfo, m_renderer);
}

Application::~Application()
{
	SDL_DestroyWindow(m_window);
	SDL_DestroyRenderer(m_renderer);
	SDL_Quit();
}

void Application::Run()
{
	// Performance time start
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	HandleEvents();
	Update();
	Render();

	// Performance time end
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	std::clog << "Took " << timeSpan.count() << "s to calculate" << std::endl;
}

void Application::HandleEvents()
{

}

void Application::Update()
{
	m_tracer->Update();
}

void Application::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	m_tracer->Render();

	SDL_RenderPresent(m_renderer);
}
