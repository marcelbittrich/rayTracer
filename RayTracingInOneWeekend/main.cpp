#include "iostream"

#include "color.h"
#include "vec3.h"
#include "ray.h"

#include "memory"
#include "chrono"


double HitSphere(const point3& center, double radius, const Ray& ray)
{
	// Solve quadratic equation for line-sphere-intersection.
	vec3 oc = ray.origin() - center;
	double a = ray.direction().length_squared();
	double half_b = dot(oc, ray.direction());
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;

	if (discriminant < 0)
		return -1.0;
	else
		return (-half_b - sqrt(discriminant)) / a;
}


color RayColor(const Ray& ray)
{
	point3 sphereCenter = point3(0.0, 0.0, -1.0);
	double t = HitSphere(sphereCenter, 0.5, ray);

	if (t > 0.0)
	{
		vec3 N = unit_vector(ray.at(t) - sphereCenter);
		return 0.5 * color(N.x() + 1, N.y() + 1, N.z() + 1);
	}

	// Viewport height range 0..1
	vec3 unitDirection = unit_vector(ray.direction());
	double a = 0.5 * (unitDirection.y() + 1.0);

	color startColor = { 1.0, 1.0, 1.0 };
	color endColor = { 0.5, 0.7, 1.0 };
	color lerpedColor = startColor * (1 - a) + endColor * a;

	return lerpedColor;
}

int main()
{
	// Image
	const double aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;

	int imageHeight = (int)(imageWidth / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight;

	// Camera
	double focalLenght = 1.0;
	const double viewportHeight = 2.0;
	const double viewportWidth = viewportHeight * ((double)(imageWidth) / imageHeight);
	point3 cameraCenter = { 0, 0, 0 };

	vec3 viewportU = { viewportWidth, 0, 0 };
	vec3 viewportV = { 0, -viewportHeight, 0 };

	vec3 pixelDeltaU = viewportU / imageWidth;
	vec3 pixelDeltaV = viewportV / imageHeight;

	// Calculate location of upper left pixel 
	point3 viewportUpperLeft = cameraCenter - vec3(0, 0, focalLenght) - viewportU / 2 - viewportV / 2;
	point3 firstPixelLocation = viewportUpperLeft + pixelDeltaU / 2 + pixelDeltaV / 2;


	// Performance time start
	std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

	// TODO: work with imageBuffer
	std::unique_ptr<color[]> imageBuffer = std::make_unique<color[]>(imageHeight * imageWidth);

	for (int j = 0; j < imageHeight; j++)
	{
		std::clog << "\rScanlines remaning: " << (imageHeight - j) << ' ' << std::flush;
		for (int i = 0; i < imageWidth; i++)
		{
			point3 pixelCenter = firstPixelLocation + pixelDeltaU * i + pixelDeltaV * j;
			vec3 rayDirection = pixelCenter - cameraCenter;

			Ray ray(pixelCenter, rayDirection);

			color pixelColor = RayColor(ray);

			imageBuffer.get()[j * imageWidth + i] = pixelColor;
		}
	}

	// Performance time end
	std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
	std::clog << "Took " << timeSpan.count() << "s to calculate" << std::endl;


	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n" << std::endl;

	for (int i = 0; i < imageHeight * imageWidth; i++)
	{
		write_color(std::cout, imageBuffer.get()[i]);
	}


	std::clog << "\rDone.                      \n";



	return 0;
}