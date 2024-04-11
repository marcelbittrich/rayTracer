#include "ui.h"

#include <iostream>

#include "SDL_render.h"

UI::UI(SDL_Window* window, SDL_Renderer* renderer)
	: m_window(window), m_renderer(renderer)
{
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
}

UI::~UI()
{
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void UI::Update(UIData& data)
{
	UIData& uiData = data;

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport((const ImGuiViewport*)0, flags, (const ImGuiWindowClass*)0);

	ImGuiIO& io = ImGui::GetIO();
	{
		ImGui::Begin("Scene");
		{
			ImGui::Checkbox("Lock Camera Input", &uiData.nonCritical.lockInput);
			if (ImGui::CollapsingHeader("Critical"))
			{
				ImGui::SeparatorText("Camera");
				{
					vec3& camPos = uiData.critical.camPosition;
					float vec[3] = { (float)camPos.x(), (float)camPos.y(), (float)camPos.z() };
					if (ImGui::InputFloat3("Position", vec))
					{
						camPos[0] = (double)vec[0];
						camPos[1] = (double)vec[1];
						camPos[2] = (double)vec[2];
					};
				}
				{
					vec3& camRot = uiData.critical.camRotation;
					float vec[3] = { (float)camRot.x(), (float)camRot.y(), (float)camRot.z() };
					if (ImGui::InputFloat3("Rotation", vec))
					{
						camRot[0] = (double)vec[0];
						camRot[1] = (double)vec[1];
						camRot[2] = (double)vec[2];
					};
				}
				{
					int bounces = uiData.critical.rayBounces;
					if (ImGui::InputInt("Ray Bounces", &bounces))
					{
						if (bounces <= 10 && bounces >= 1)
							uiData.critical.rayBounces = bounces;
					}
				}

				{
					float backgroundBrightness = (float)uiData.critical.backgroundBrightness;
					float upperLimit = 1.0f;
					float lowerLimit = 0.0f;
					if (ImGui::DragFloat("Background Brightness", &backgroundBrightness, 0.01F, lowerLimit, upperLimit))
					{
						if (backgroundBrightness > upperLimit)
							uiData.critical.backgroundBrightness = upperLimit;
						else if (backgroundBrightness < lowerLimit)
							uiData.critical.backgroundBrightness = lowerLimit;
						else
							uiData.critical.backgroundBrightness = backgroundBrightness;
					}
				}

				{
					float hfov = (float)uiData.critical.hfov;
					float upperLimit = 180.0f;
					float lowerLimit = 5.0f;
					if (ImGui::DragFloat("HFOV", &hfov, 1.0F, lowerLimit, upperLimit))
					{
						if (hfov > upperLimit)
							uiData.critical.hfov = upperLimit;
						else if (hfov < lowerLimit)
							uiData.critical.hfov = lowerLimit;
						else
							uiData.critical.hfov = hfov;
					}
				}

				{
					float defocusAngle = (float)uiData.critical.defocusAngle;
					float upperLimit = 5.0f;
					float lowerLimit = 0.0f;
					if (ImGui::DragFloat("Defocus Angle", &defocusAngle, 0.01F, lowerLimit, upperLimit))
					{
						if (defocusAngle > upperLimit)
							uiData.critical.defocusAngle = upperLimit;
						else if (defocusAngle < lowerLimit)
							uiData.critical.defocusAngle = lowerLimit;
						else
							uiData.critical.defocusAngle = defocusAngle;
					}
				}
			}

			if (ImGui::CollapsingHeader("Non Critical"))
			{
				ImGui::Checkbox("Bloom", &uiData.nonCritical.bloom);
			}

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::Text("Sample: %d", uiData.nonCritical.sample);
		}
		ImGui::End();

		ImGui::Begin("Export");
		{
			if (ImGui::Button("Export Image"))
			{
				//TODO
				std::cout << "Export Image Pressed" << std::endl;
			}
		}
		ImGui::End();
	}
}

void UI::Render()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGui::Render();
	SDL_RenderSetScale(m_renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}
