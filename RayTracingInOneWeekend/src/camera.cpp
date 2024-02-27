#include "camera.h"

#include "objects/material.h"

Camera::Camera(const WindowInfo& windowInfo)
	: m_position(vec3(0.0, 0.0, 4.0))
{
	RecalculateViewport(windowInfo);
}

void Camera::RecalculateViewport(const WindowInfo& windowInfo)
{
	m_focalLength = 1.0;
	double theta = degreesToRadians(hfov);
	double h = tan(theta / 2);

	m_viewportWidth = 2 * h * m_focalLength;
	m_viewportHeight = m_viewportWidth * ((double)(windowInfo.height) / windowInfo.width);

	vec3 viewportU = { m_viewportWidth, 0, 0 };
	vec3 viewportV = { 0, -m_viewportHeight, 0 };

	m_pixelDeltaU = viewportU / windowInfo.width;
	m_pixelDeltaV = viewportV / windowInfo.height;

	// Calculate location of upper left pixel 
	point3 viewportUpperLeft = m_position - vec3(0, 0, m_focalLength) - viewportU / 2 - viewportV / 2;
	m_firstPixelLocation = viewportUpperLeft + m_pixelDeltaU / 2 + m_pixelDeltaV / 2;
}

void Camera::HandleInput(const Input& input, double deltaTime)
{
	double speed = 1.0;
	double speedUp = 2.0;
	vec3 change = { 0,0,0 };

	if (input.Up())
	{
		change += vec3(0, 0, -speed) * deltaTime;
		m_HasChanged = true;
	}
	if (input.Down())
	{
		change += vec3(0, 0, speed) * deltaTime;
		m_HasChanged = true;
	}
	if (input.Left())
	{
		change += vec3(-speed, 0, 0) * deltaTime;
		m_HasChanged = true;
	}
	if (input.Right())
	{
		change += vec3(speed, 0, 0) * deltaTime;
		m_HasChanged = true;
	}
	if (input.LeftShift())
	{
		change *= speedUp;
	}

	m_position += change;
}

void Camera::Update(const HittableList& world, color imageBuffer[], const WindowInfo& windowInfo)
{
	if (m_HasChanged)
	{ 
		RecalculateViewport(windowInfo);
		m_sampleCount = 0;
		m_HasChanged = false;
	}

	++m_sampleCount;

	for (int j = 0; j < windowInfo.height; j++)
	{
		for (int i = 0; i < windowInfo.width; i++)
		{
			m_seed = j * windowInfo.width + i;
			m_seed *= m_sampleCount;

			color pixelColor = { 0,0,0 };
			Ray ray = GetRay(i, j);
			color newColor= RayColor(ray, m_maxBounce, world, m_seed);

			if (m_sampleCount > 1)
			{
				pixelColor = imageBuffer[j * windowInfo.width + i];
				const double x = (double)m_sampleCount;
				pixelColor = pixelColor * ((x - 1) / x) + newColor * (1.0 / x);
			}
			else
			{
				pixelColor = newColor;
			}

			imageBuffer[j * windowInfo.width + i] = pixelColor;
		}
	}
	std::cout << "Sample: " << m_sampleCount << std::endl;
}

Ray Camera::GetRay(int i, int j)
{
	point3 pixelCenter = m_firstPixelLocation + m_pixelDeltaU * i + m_pixelDeltaV * j;
	point3 pixelSample = pixelCenter + PixelSampleSquare();

	point3 rayOrigin = m_position;
	vec3 rayDirection = pixelSample - rayOrigin;

	return Ray(rayOrigin, rayDirection);
}

vec3 Camera::PixelSampleSquare()
{
	double deltaX = -0.5 + fastRandomDouble(m_seed);
	double deltaY = -0.5 + fastRandomDouble(m_seed);
	return deltaX * m_pixelDeltaU + deltaY * m_pixelDeltaV;
}

color Camera::RayColor(const Ray& ray, int bounce, const Hittable& world, uint32_t seed)
{
	if (bounce <= 0)
	{
		return color(0, 0, 0);
	}

	seed += bounce;
	HitRecord rec;
	if (world.Hit(ray, Interval(0.001, infinity), rec))
	{
		Ray scatterd;
		color attenuation;

		if (rec.material->Scatter(ray, rec, attenuation, scatterd, seed))
		{
			return attenuation * RayColor(scatterd, bounce - 1, world, seed);
		}

		return color(0, 0, 0);
	}

	// Color background with gradient if not hit.
	// viewport height range 0..1
	vec3 unitdirection = unitVector(ray.direction());
	double a = 0.5 * (unitdirection.y() + 1.0);
	color startcolor = { 1.0, 1.0, 1.0 };
	color endcolor = { 0.5, 0.7, 1.0 };
	color lerpedcolor = startcolor * (1 - a) + endcolor * a;
	return lerpedcolor;
}

//// Cherno seed approach 
//seed = x + y * imageWidth;
//seed *= m_frameSample;
//seed += bounceCount