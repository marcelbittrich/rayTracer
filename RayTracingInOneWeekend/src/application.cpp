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
	SDL_WindowFlags windowFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_window = SDL_CreateWindow("RayTracer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_windowWidth, m_windowHeight, windowFlags);
	Uint32 flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	m_renderer = SDL_CreateRenderer(m_window, -1, flags);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
	ImGui_ImplSDLRenderer2_Init(m_renderer);

	m_camera = std::make_unique<Camera>(m_windowInfo);
	m_imageBuffer = new color[m_windowInfo.width * m_windowInfo.height];
	m_windowRenderer = std::make_unique<SDLWindowRenderer>(m_renderer, m_windowInfo);

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

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

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
	if (!io.WantCaptureMouse && !m_lockInput)
	{
		m_camera->HandleInput(m_input, m_deltaTime);
	}
}

void Application::Update()
{
	m_camera->Update(m_world, m_imageBuffer, m_windowInfo);

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport((const ImGuiViewport *) 0, flags, (const ImGuiWindowClass *) 0);
	ImGui::ShowDemoWindow(&m_showGuiDemo);

	ImGuiIO& io = ImGui::GetIO();
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Secne Data");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &m_showGuiDemo);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &m_showGuiDemo);
		ImGui::Checkbox("Bloom", &m_applyBloom);
		ImGui::Checkbox("Lock Input", &m_lockInput);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::Text("Sample: %d", m_camera->GetSampleNumber());
		ImGui::End();
	}

	m_windowRenderer->SetBloom(m_applyBloom);
}

void Application::Render()
{
	m_windowRenderer->Render(m_imageBuffer, m_windowInfo);
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Render();
	SDL_RenderSetScale(m_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(m_renderer);
}
