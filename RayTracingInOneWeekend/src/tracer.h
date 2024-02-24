#pragma once

#include <vector>

#include "tools/rtweekend.h"
#include "tools/color.h"

#include "objects/hittable.h"
#include "objects/hittablelist.h"
#include "objects/sphere.h"

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
	color RayColor(const Ray& ray, const Hittable& world);
}