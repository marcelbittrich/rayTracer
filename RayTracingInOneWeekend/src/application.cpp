#include "application.h"

#include <chrono>

#include "SDL.h"

#include "objects/sphere.h"
#include "objects/material.h"
#include "tools/color.h"
#include "imgui.h"

#include <String>

Application::Application()
{
	m_windowInfo =
	{
		m_aspectRatio,
		m_windowWidth,
		m_windowHeight
	};

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, windowFlags);
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	m_renderer = SDL_CreateRenderer(m_window, -1, flags);

	m_ui = std::make_unique<UI>(m_window, m_renderer);
	m_camera = std::make_unique<Camera>(m_windowInfo);
	m_windowRenderer = std::make_unique<SDLWindowRenderer>(m_renderer, m_windowInfo);

	m_imageBuffer = new color[m_windowInfo.width * m_windowInfo.height];

	SetWorld();
}

void Application::SetWorld()
{
	auto materialGround = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
	auto materialCenter = make_shared<Lambertian>(color(0.7, 0.3, 0.3));
	auto materialLeft   = make_shared<Metal>(color(0.8, 0.8, 0.8), 0.0 );
	auto materialRight  = make_shared<Metal>(color(0.82, 0.68, 0.22), 0.7);

	// Refraction Index: glass 1.3 - 1.7, diamond 2.4.  
	auto materialGlass = make_shared<Dielectric>(color(1.0, 1.0, 1.0), 1.5);

	auto lightMaterial = make_shared<DiffuseLight>(color(1, 1, 1), 3.0);

	m_world.addSphere(Sphere(point3(2, 0, -5), 1.5, materialRight));
	m_world.addSphere(Sphere(point3(0, 0, -1), 0.5, materialCenter));
	m_world.addSphere(Sphere(point3(-3, 0.5, -3), 1.0, materialLeft));
	m_world.addSphere(Sphere(point3(0, -1000.5, -1), 1000, materialGround));

	m_world.addSphere(Sphere(point3(-1.5, -0.1, -1.5), 0.4, materialGlass));

	m_world.addSphere(Sphere(point3(0, 3, -2), 1.0, lightMaterial));

	if(m_hasRandomSpheres)
		AddRandomSpheres(m_world);
}

void Application::AddRandomSpheres(HittableList& world)
{
	int count = 5;
	double size = 0.1;
	double distance = 4;

	for (int i = -count; i <= count; i++)
	{
		for (int j = -count; j <= count; j++)
		{
			double x = fastRandomDouble(-1, 1) * size + (double)i * size * 4.0;
			double y = -0.4;
			double z = fastRandomDouble(-1, 1) * size + (double)j * size * 4.0;
			color sphereColor = vec3::fastRandom(0.0, 1.0);

			int materialSelector = (int)fastRandomDouble(0.0, 2.99);
			
			if (materialSelector == 0)
			{
				auto labertian = make_shared<Lambertian>(sphereColor);
				m_world.addSphere(Sphere(point3(x, y, z), 0.1, labertian));
			}
			else if (materialSelector == 1)
			{
				auto metal = make_shared<Metal>(sphereColor, fastRandomDouble(0.0, 1.0));
				m_world.addSphere(Sphere(point3(x, y, z), 0.1, metal));
			}
			else if (materialSelector == 2)
			{
				auto glass = make_shared<Dielectric>(sphereColor, fastRandomDouble(1.3, 2.4));
				m_world.addSphere(Sphere(point3(x, y, z), 0.1, glass));
			}
		}
	}
}

Application::~Application()
{
	delete[] m_imageBuffer;

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

	const int sample = m_camera->GetSampleNumber();
	const double sc = (double)sample;
	m_averagedeltaTime = m_averagedeltaTime * ((sc - 1) / sc) + m_deltaTime * (1.0 / sc);
	//std::clog << "Sample: " << sample << " / Avg time per frame: " << m_averagedeltaTime << "s    \r" << std::flush;
}


void Application::HandleEvents()
{
	m_input.HandleInput(m_running);

	if (m_input.WindowResized())
	{
		m_windowInfo.hasChanged = true;		
		SDL_GetWindowSize(m_window, &m_windowInfo.width, &m_windowInfo.height);
		delete[] m_imageBuffer;
		m_imageBuffer = new color[m_windowInfo.width * m_windowInfo.height];
	}
	else
	{
		m_windowInfo.hasChanged = false;
	}

	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureMouse && !m_uiData.nonCritical.lockInput)
	{
		m_camera->HandleInput(m_input, m_deltaTime);
	}
}

void Application::Update()
{
	m_camera->GetDataForUI(m_uiData);

	std::string dataString = std::string((char*)&m_uiData.critical, sizeof(m_uiData.critical));
	size_t dataHashBefore = std::hash<std::string>{}(dataString);

	m_ui->Update(m_uiData, m_world);

	dataString = std::string((char*)&m_uiData.critical, sizeof(m_uiData.critical));
	size_t dataHashAfter = std::hash<std::string>{}(dataString);

	//if (dataString.compare(dataString2))
	if (dataHashBefore != dataHashAfter)
	{
		std::cout << "has changed" << std::endl;
		m_camera->SetHasChanged(true);
	}
	
	m_camera->SetDataFromUI(m_uiData);
	m_camera->Update(m_world, m_imageBuffer, m_windowInfo);
	m_windowRenderer->SetBloom(m_uiData.nonCritical.bloom);
}

void Application::Render()
{
	m_windowRenderer->Render(m_imageBuffer, m_windowInfo);
	m_ui->Render();
	SDL_RenderPresent(m_renderer);
}
