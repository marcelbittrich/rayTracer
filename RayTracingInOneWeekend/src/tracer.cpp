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
	// Camera
	double focalLenght = 1.0;
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

			color pixelColor = Tracing::RayColor(ray);

			imageBuffer[j * m_imageWidth + i] = pixelColor;
		}
	}
}

namespace Tracing
{
	color RayColor(const Ray& ray)
	{
		point3 spherecenter = point3(0.0, 0.0, -1.0);
		double t = HitSphere(spherecenter, 0.5, ray);

		if (t > 0.0)
		{
			vec3 n = unit_vector(ray.at(t) - spherecenter);
			return 0.5 * color(n.x() + 1, n.y() + 1, n.z() + 1);
		}

		// viewport height range 0..1
		vec3 unitdirection = unit_vector(ray.direction());
		double a = 0.5 * (unitdirection.y() + 1.0);

		color startcolor = { 1.0, 1.0, 1.0 };
		color endcolor = { 0.5, 0.7, 1.0 };
		color lerpedcolor = startcolor * (1 - a) + endcolor * a;

		return lerpedcolor;
	}

	double HitSphere(const point3& center, double radius, const Ray& ray)
	{
		// solve quadratic equation for line-sphere-intersection.
		vec3 oc = ray.origin() - center;
		double a = ray.direction().length_squared();
		double half_b = dot(oc, ray.direction());
		double c = oc.length_squared() - radius * radius;
		double discriminant = half_b * half_b - a * c;

		if (discriminant < 0)
		{
			return -1.0;
		}
		else
		{
			return (-half_b - sqrt(discriminant)) / a;
		}
	}
}
