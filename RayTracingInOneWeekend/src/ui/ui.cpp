#include "ui.h"

#include <iostream>
#include <string>

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

void UI::Update(UIData& data, HittableList& world)
{
	UIData& uiData = data;

	ImGui_ImplSDLRenderer2_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuiDockNodeFlags flags = ImGuiDockNodeFlags_PassthruCentralNode;
	ImGui::DockSpaceOverViewport((const ImGuiViewport*)0, flags, (const ImGuiWindowClass*)0);

	ImGuiIO& io = ImGui::GetIO();
	{
		ImGui::Begin("Camera");
		{
			ImGui::Checkbox("Lock Camera Input", &uiData.nonCritical.lockInput);
			if (ImGui::CollapsingHeader("Critical"))
			{
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

				ImGui::Checkbox("Focus Blur", &uiData.critical.hasFocusBlur);
				{
					float foucsDistance = (float)uiData.critical.focusDistance;
					float upperLimit = 100.0f;
					float lowerLimit = 1.0f;
					if (ImGui::DragFloat("Focus Distance", &foucsDistance, 1.0F, lowerLimit, upperLimit))
					{
						if (foucsDistance > upperLimit)
							uiData.critical.focusDistance = upperLimit;
						else if (foucsDistance < lowerLimit)
							uiData.critical.focusDistance = lowerLimit;
						else
							uiData.critical.focusDistance = foucsDistance;
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

		ImGui::Begin("Scene");
		{
			ImGui::SeparatorText("Ojects");
			bool& hasWorldChanged = uiData.critical.hasWorldChanged;
			hasWorldChanged = false;
			int sphereIndex = 0;
			for (Sphere& object : world.m_spheres)
			{
				sphereIndex++;
				std::string name = "Sphere " + std::to_string(sphereIndex);
				if (ImGui::TreeNode(name.c_str()))
				{
					{
						vec3& objectPos = object.m_center;
						float vec[3] = { (float)objectPos.x(), (float)objectPos.y(), (float)objectPos.z() };
						if (ImGui::InputFloat3("Position", vec))
						{
							objectPos[0] = (double)vec[0];
							objectPos[1] = (double)vec[1];
							objectPos[2] = (double)vec[2];
							hasWorldChanged = true;
						};
					}

					{
						float objectRadius = (float)object.m_radius;
						if (ImGui::DragFloat("Radius", &objectRadius, 0.1f, 0.01f, 100.f))
						{
							object.m_radius = objectRadius;
							hasWorldChanged = true;
						};
					}

					ImGui::TreePop();
				};
			}
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
