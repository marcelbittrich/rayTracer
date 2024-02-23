#pragma once

#include "tools/color.h"
#include "tools/ray.h"
#include "camera.h" 

#include "SDL_render.h"

struct WindowInfo
{
	double aspectRatio;
	int width;
};


class Tracer
{
public:
	Tracer(const Camera& Camera, const WindowInfo& windowInfo, SDL_Renderer* renderer);
	~Tracer();

	void Update();
	void Render();

private:
	SDL_Renderer* m_renderer = nullptr;
	int m_imageWidth;
	int m_imageHeight;
	Camera m_camera;
	std::unique_ptr<color[]> m_imageBuffer;

	void SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel);
};

namespace Tracing
{
	color RayColor(const Ray& ray);
	double HitSphere(const point3& center, double radius, const Ray& ray);
}