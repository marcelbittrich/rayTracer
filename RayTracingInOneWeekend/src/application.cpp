#include "application.h"

#include <chrono>

#include "SDL.h"
#include "SDL_image.h"

#include "objects/bvh.h"
#include "objects/sphere.h"
#include "objects/triangle.h"
#include "objects/diamond.h"
#include "objects/material.h"
#include "objects/polygonobject.h"
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

	int IMGflags = IMG_INIT_JPG | IMG_INIT_PNG;
	IMG_Init(IMGflags);

	m_ui = std::make_unique<UI>(m_window, m_renderer);
	m_camera = std::make_unique<Camera>(m_windowInfo);
	m_windowRenderer = std::make_unique<SDLWindowRenderer>(m_renderer, m_windowInfo);
	m_exporter = std::make_unique<Exporter>();

	m_imageBuffer = new color[m_windowInfo.width * m_windowInfo.height];

	SetWorld();

	m_world.SetupBVH();

	if (auto rootNode = m_world.m_rootNode)
	{
		std::cout << "Root Node Leaf Count: " << rootNode->GetLeafCount() << std::endl;
		int leftSum = ((BVH_Node*)rootNode->left)->GetLeafCount();
		int rightSum = ((BVH_Node*)rootNode->right)->GetLeafCount();
		std::cout << "Left Node Leaf Count: " << leftSum << std::endl;
		std::cout << "Right Node Leaf Count: " << rightSum << std::endl;
	}
}

void Application::SetWorld()
{
	auto materialGround = make_shared<Lambertian>(color(0.5, 0.5, 0.5));
	auto materialCenter = make_shared<Lambertian>(color(0.7, 0.3, 0.3));
	auto materialLeft   = make_shared<Metal>(color(0.8, 0.8, 0.8), 0.0 );
	auto materialRight  = make_shared<Metal>(color(0.82, 0.68, 0.22), 0.7);

	// Refraction Index: glass 1.3 - 1.7, diamond 2.4.  
	auto materialGlass = make_shared<Dielectric>(color(1.0, 1.0, 1.0), 1.5);
	auto materialDiamond = make_shared<Dielectric>(color(1.0, 1.0, 1.0), 2.4);

	auto lightMaterial = make_shared<DiffuseLight>(color(1, 1, 1), 3.0);
	auto lightMaterialGreen = make_shared<DiffuseLight>(color(0.2, 1, 0.2), 3.0);
	auto lightMaterialRed = make_shared<DiffuseLight>(color(1, 0.2, 0.2), 3.0);
	auto lightMaterialBlue = make_shared<DiffuseLight>(color(0.2, 0.2, 1.0), 3.0);


	Scene sphereScene;
	sphereScene.AddSphere(Sphere(point3(0, 3, -2), 1.0, lightMaterial));
	sphereScene.AddSphere(Sphere(point3(-2, 0.5, 1), .2, lightMaterialGreen));
	sphereScene.AddSphere(Sphere(point3(4, 0.7, -1), .35, lightMaterialRed));
	sphereScene.AddSphere(Sphere(point3(2, 0, -5), 1.5, materialRight));
	sphereScene.AddSphere(Sphere(point3(0, 0, -1), 0.5, materialCenter));
	sphereScene.AddSphere(Sphere(point3(-3, 0.5, -3), 1.0, materialLeft));
	sphereScene.AddSphere(Sphere(point3(0, -1000.5, -1), 1000, materialGround));
	sphereScene.AddSphere(Sphere(point3(-1.5, -0.1, -1.5), 0.4, materialGlass));

	sphereScene.AddDiamond(Diamond(point3(0, 1, -1), 1.2, materialLeft));
	sphereScene.AddDiamond(Diamond(point3(-1, 1, 1), 0.7, materialLeft));
	sphereScene.AddDiamond(Diamond(point3(1, 1, -0.5), 2, materialRight));
	sphereScene.AddDiamond(Diamond(point3(2, 1, 0.5), 0.5, materialDiamond));
	sphereScene.AddDiamond(Diamond(point3(-2.2, -0.1, -1.5), 0.5, materialDiamond));

	Scene polyObjectScene;
	polyObjectScene.AddSphere(Sphere(point3(2, 4, 0), 1.0, lightMaterialBlue));
	polyObjectScene.AddSphere(Sphere(point3(-1, 4, 1.732), 1.0, lightMaterial));
	polyObjectScene.AddSphere(Sphere(point3(-1, 4, -1.732), 1.0, lightMaterialRed));
	polyObjectScene.AddSphere(Sphere(point3(0, -1000.5, -1), 1000, materialGround));
	ObjectImport::ObjectBuffer buffers = ObjectImport::readOBJ("D:/Dokumente/GameDev/RayTracingInOneWeekend/objects/diamond.obj");
	polyObjectScene.AddPolygonObject(PolygonObject(buffers, point3(-2.7, 0.5, -2.7), materialDiamond));
	polyObjectScene.AddDiamond(Diamond(point3(2.1, 1.2, 2.1), 1.5, materialCenter));

	m_world = polyObjectScene;

	if(m_hasRandomSpheres)
		AddRandomSpheres(m_world);
}

void Application::AddRandomSpheres(Scene& world)
{
	const int count = 3;
	const double size = 0.15;
	const double distance = 3;

	for (int i = -count; i <= count; i++)
	{
		for (int j = -count; j <= count; j++)
		{
			const double x = fastRandomDouble(-1, 1) * size + (double)i * size * 4.0;
			const double y = -0.4;
			const double z = fastRandomDouble(-1, 1) * size + (double)j * size * 4.0;
			const color sphereColor = vec3::fastRandom(0.0, 1.0);
			const int materialSelector = (int)fastRandomDouble(0.0, 2.99);
			
			if (materialSelector == 0)
			{
				auto labertian = make_shared<Lambertian>(sphereColor);
				m_world.AddSphere(Sphere(point3(x, y, z), 0.1, labertian));
			}
			else if (materialSelector == 1)
			{
				auto metal = make_shared<Metal>(sphereColor, fastRandomDouble(0.0, 1.0));
				m_world.AddSphere(Sphere(point3(x, y, z), 0.1, metal));
			}
			else if (materialSelector == 2)
			{
				auto glass = make_shared<Dielectric>(sphereColor, fastRandomDouble(1.3, 2.4));
				m_world.AddSphere(Sphere(point3(x, y, z), 0.1, glass));
			}
		}
	}
}

Application::~Application()
{
	delete[] m_imageBuffer;

	IMG_Quit();
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

	if (dataHashBefore != dataHashAfter)
	{
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

	if (m_uiData.nonCritical.exportImage)
	{
		ExportInfo info;
		info.imageSurface = m_windowRenderer->GetImageSurface();
		info.fileName = "Default";
		info.filePath = "../img/";
		info.format = m_uiData.nonCritical.exportImageFormat;
		info.quality = m_uiData.nonCritical.exportImageQuality;

		m_exporter->ExportImage(info);
	}
}