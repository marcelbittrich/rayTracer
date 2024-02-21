#include "iostream"

#include "color.h"
#include "vec3.h"

#include "memory"

int main()
{
	const double aspectRatio = 16.0 / 9.0;
	const int imageWidth = 100;

	int imageHeight = (int)(imageWidth / aspectRatio);
	imageHeight = (imageHeight < 1) ? 1 : imageHeight;

	const double viewportHeight = 2.0;
	const double viewportWidth = viewportHeight * ((double)(imageWidth) / imageHeight);

	// TODO: work with imageBuffer
	// std::unique_ptr<int[]> imageBuffer = std::make_unique<int[]>(imageHeight * imageWidth * 3);

	std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n" << std::endl;

	for (int j = 0; j < imageHeight; j++)
	{
		std::clog << "\rScanlines remaning: " << (imageHeight - j) << ' ' << std::flush;
		for (int i = 0; i < imageWidth; i++)
		{
			double r = (double)i / (imageWidth - 1);
			double g = (double)j / (imageHeight - 1);
			double b = 0.0;

			color pixelColor = color(r, g, b);
		
			write_color(std::cout, pixelColor);
		}
	}

	std::clog << "\rDone.                      \n";

	return 0;
}