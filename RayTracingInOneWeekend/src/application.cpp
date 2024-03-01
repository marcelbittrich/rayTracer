#include "application.h"

#include <chrono>

#include "SDL.h"

#include "objects/sphere.h"
#include "objects/material.h"
#include "tools/color.h"


Application::Application()
{
	m_windowInfo =
	{
		m_aspectRatio,
		m_windowWidth,
		m_windowHeight
	};

	SDL_Init(SDL_INIT_EVERYTHING);
	m_window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_RESIZABLE);
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	m_renderer = SDL_CreateRenderer(m_window, -1, flags);

	m_camera = std::make_unique<Camera>(m_windowInfo);
	m_imageBuffer = std::make_unique<color[]>(m_windowWidth * m_windowHeight);
	m_windowRenderer = std::make_unique<SDLWindowRenderer>(m_renderer);

	SetWorld();
}

void Application::SetWorld()
{
	auto materialGround = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
	auto materialCenter = make_shared<Lambertian>(color(0.7, 0.3, 0.3));
	auto materialLeft   = make_shared<Metal>(color(0.8, 0.8, 0.8), 0.0 );
	auto materialRight  = make_shared<Metal>(color(0.82, 0.68, 0.22), 0.7);

	// Refraction Index: glass 1.3 - 1.7, diamond 2.4.  
	auto materialGlass = make_shared<Dielectric>(2.4);

	m_world.add(make_shared<Sphere>(point3(2, 0, -5), 1.5, materialRight));
	m_world.add(make_shared<Sphere>(point3(0, 0, -1), 0.5, materialCenter));
	m_world.add(make_shared<Sphere>(point3(-3, 0.5, -3), 1.0, materialLeft));
	m_world.add(make_shared<Sphere>(point3(0, -100.5, -1), 100, materialGround));

	m_world.add(make_shared<Sphere>(point3(-1.5, -0.1, -1.5), 0.4, materialGlass));
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
	m_deltaTime = timeSpan.count();
	std::clog << "Took " << m_deltaTime << "s to calculate" << std::endl;
}



void Application::HandleEvents()
{
	m_input.HandleInput(m_running);
	m_camera->HandleInput(m_input, m_deltaTime);
}

void Application::Update()
{
	m_camera->Update(m_world, m_imageBuffer.get(), m_windowInfo);
}

void Application::Render()
{
	m_windowRenderer->Render(m_imageBuffer.get(), m_windowInfo);
}


