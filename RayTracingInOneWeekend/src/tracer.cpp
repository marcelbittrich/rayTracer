#include "tracer.h"

#include "tools/vec3.h"


Tracer::Tracer(const Camera& camera, const WindowInfo& windowInfo, SDL_Renderer* renderer)
	: m_renderer(renderer)
{
	m_camera = camera;

	// Image
	double aspectRatio = windowInfo.aspectRatio;
	m_imageWidth = windowInfo.width;

	m_imageHeight = (int)(m_imageWidth / aspectRatio);
	m_imageHeight = (m_imageHeight < 1) ? 1 : m_imageHeight;


	// TODO: work with imageBuffer
	m_imageBuffer = std::make_unique<color[]>(m_imageHeight * m_imageWidth);
}

Tracer::~Tracer()
{
}


void Tracer::Update()
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

			m_imageBuffer.get()[j * m_imageWidth + i] = pixelColor;
		}
	}
}

void Tracer::Render()
{
	SDL_Surface* surface = SDL_CreateRGBSurface(0, m_imageWidth, m_imageHeight, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_memset(surface->pixels, 0, surface->h * surface->pitch);

	//std::cout << "P3\n" << m_imageWidth << ' ' << m_imageHeight << "\n255\n" << std::endl;

	for (int j = 0; j < m_imageHeight; j++)
	{
		for (int i = 0; i < m_imageWidth; i++)
		{
			//write_color(std::cout, m_imageBuffer.get()[j * m_imageWidth + i]);

			uint8_t r = (uint8_t)(255.999 * m_imageBuffer.get()[j * m_imageWidth + i].x());
			uint8_t g = (uint8_t)(255.999 * m_imageBuffer.get()[j * m_imageWidth + i].y());
			uint8_t b = (uint8_t)(255.999 * m_imageBuffer.get()[j * m_imageWidth + i].z());
			uint8_t a = (uint8_t)255;

			Uint32 color = SDL_MapRGBA(surface->format, r, g, b, a);
			SetPixel(surface, i, j, color);
		}
	}

	// Set sampling to nearest pixel
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);
	SDL_Rect imageRect = { 0, 0, m_imageWidth, m_imageHeight };
	SDL_RenderCopy(m_renderer, texture, &imageRect, &imageRect);

	SDL_DestroyTexture(texture);
}

void Tracer::SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* pixels = (Uint32*)surface->pixels;
	int position = (y * surface->w) + x;
	pixels[position] = pixel;
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
