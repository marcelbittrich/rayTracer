#pragma once

#include <vector>

#include "tools/color.h"
#include "tools/ray.h"
#include "camera.h" 

#include "SDL_render.h"

struct WindowInfo
{
	double aspectRatio;
	int width;
	int heigth;
};


class Tracer
{
public:
	Tracer(const Camera& Camera, const WindowInfo& windowInfo);
	~Tracer();

	void Update(color imageBuffer[]);

private:
	int m_imageWidth;
	int m_imageHeight;
	Camera m_camera;
};

namespace Tracing
{
	color RayColor(const Ray& ray);
	double HitSphere(const point3& center, double radius, const Ray& ray);
}