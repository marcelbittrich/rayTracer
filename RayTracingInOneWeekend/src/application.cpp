#include "application.h"

#include <chrono>

#include "SDL.h"


Application::Application()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	m_window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, SDL_WINDOW_RESIZABLE);
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	m_renderer = SDL_CreateRenderer(m_window, -1, flags);

	WindowInfo windowInfo =
	{
		m_aspectRatio,
		m_windowWidth,
		m_windowHeight
	};
	m_tracer = std::make_unique<Tracer>(m_camera, windowInfo);

	m_imageBuffer = std::make_unique<color[]>(m_windowWidth * m_windowHeight);
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
	m_tracer->Update(m_imageBuffer.get());
}

void Application::Render()
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	SDL_Surface* surface = SDL_CreateRGBSurface(0, m_windowWidth, m_windowHeight, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_memset(surface->pixels, 0, surface->h * surface->pitch);

	//std::cout << "P3\n" << m_imageWidth << ' ' << m_imageHeight << "\n255\n" << std::endl;

	for (int j = 0; j < m_windowHeight; j++)
	{
		for (int i = 0; i < m_windowWidth; i++)
		{
			//write_color(std::cout, m_imageBuffer.get()[j * m_imageWidth + i]);

			uint8_t r = (uint8_t)(255.999 * m_imageBuffer.get()[j * m_windowWidth + i].x());
			uint8_t g = (uint8_t)(255.999 * m_imageBuffer.get()[j * m_windowWidth + i].y());
			uint8_t b = (uint8_t)(255.999 * m_imageBuffer.get()[j * m_windowWidth + i].z());
			uint8_t a = (uint8_t)255;

			Uint32 color = SDL_MapRGBA(surface->format, r, g, b, a);
			SetPixel(surface, i, j, color);
		}
	}

	// Set sampling to nearest pixel
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);
	SDL_Rect imageRect = { 0, 0, m_windowWidth, m_windowHeight };
	SDL_RenderCopy(m_renderer, texture, &imageRect, &imageRect);

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(m_renderer);
}

void Application::SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* pixels = (Uint32*)surface->pixels;
	int position = (y * surface->w) + x;
	pixels[position] = pixel;
}