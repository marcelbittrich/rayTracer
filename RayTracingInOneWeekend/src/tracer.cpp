#include "tracer.h"

#include "tools/vec3.h"


Tracer::Tracer(const Camera& camera, const WindowInfo& windowInfo)
{
	m_camera = camera;

	m_imageWidth = windowInfo.width;
	m_imageHeight = windowInfo.heigth;
}

Tracer::~Tracer()
{
}


void Tracer::Update(color imageBuffer[])
{
	// World 

	HittableList world;

	world.add(make_shared<Sphere>(point3(2, 0, -5), 1.5));
	world.add(make_shared<Sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<Sphere>(point3(-3, 0.5, -3), 1.0));
	world.add(make_shared<Sphere>(point3(0, -100.5, -1), 100));

	// Camera
	double focalLenght = 4.0;
	const double viewportHeight = 2.0;
	const double viewportWidth = viewportHeight * ((double)(m_imageWidth) / m_imageHeight);
	point3 cameraCenter = m_camera.GetPosition();

	vec3 viewportU = { viewportWidth, 0, 0 };
	vec3 viewportV = { 0, -viewportHeight, 0 };

	vec3 pixelDeltaU = viewportU / m_imageWidth;
	vec3 pixelDeltaV = viewportV / m_imageHeight;

	// Calculate location of upper left pixel 
	point3 viewportUpperLeft = cameraCenter - vec3(0, 0, focalLenght) - viewportU / 2 - viewportV / 2;
	point3 firstPixelLocation = viewportUpperLeft + pixelDeltaU / 2 + pixelDeltaV / 2;

	for (int j = 0; j < m_imageHeight; j++)
	{
		for (int i = 0; i < m_imageWidth; i++)
		{
			point3 pixelCenter = firstPixelLocation + pixelDeltaU * i + pixelDeltaV * j;
			vec3 rayDirection = pixelCenter - cameraCenter;

			Ray ray(cameraCenter, rayDirection);

			color pixelColor = Tracing::RayColor(ray, world);

			imageBuffer[j * m_imageWidth + i] = pixelColor;
		}
	}
}

namespace Tracing
{
	color RayColor(const Ray& ray, const Hittable& world)
	{
		HitRecord rec;
		if (world.Hit(ray, Interval(0, infinity), rec))
		{
			return 0.5 * (rec.normal + color(1, 1, 1));
		}

		// Color background with gradient if not hit.
		// viewport height range 0..1
		vec3 unitdirection = unit_vector(ray.direction());
		double a = 0.5 * (unitdirection.y() + 1.0);
		color startcolor = { 1.0, 1.0, 1.0 };
		color endcolor = { 0.5, 0.7, 1.0 };
		color lerpedcolor = startcolor * (1 - a) + endcolor * a;
		return lerpedcolor;
	}
}
