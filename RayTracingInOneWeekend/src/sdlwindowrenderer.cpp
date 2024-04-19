#include "sdlwindowrenderer.h"

#include <algorithm>
#include <execution>
#include <cmath>

#define BloomMultithreading 1

SDLWindowRenderer::SDLWindowRenderer(SDL_Renderer* renderer, const WindowInfo& windowInfo)
	: m_renderer(renderer)
{
	m_bloomBuffer = new color[windowInfo.height * windowInfo.width];
	m_tempBloomBuffer = new color[windowInfo.height * windowInfo.width];

	int pixelSampleDistance = 0;
	float gausianSampleWeight = 1;
	while (true)
	{ 
		// One-dimensional Gaussian function
		// calculates the weight of neighboring pixels 
		// depending on the distance to the center pixel.	
		float distance = (float)pixelSampleDistance;
		float exponent = -(distance * distance) / (2 * m_stdDev * m_stdDev);
		float gausianConstants = (1.f / std::sqrtf(2.f * (float)M_PI * m_stdDev * m_stdDev));
		gausianSampleWeight = gausianConstants * std::pow(m_eulerNumber, exponent);
		
		if (gausianSampleWeight < 0.01)
			break;

		if (pixelSampleDistance == 0)
		{
			m_gausianWeights.push_back(gausianSampleWeight);
			m_widthIndices.push_back(pixelSampleDistance);
			m_heightIndices.push_back(pixelSampleDistance);
		}
		else
		{
			m_gausianWeights.push_back(gausianSampleWeight);
			m_gausianWeights.push_front(gausianSampleWeight);

			m_widthIndices.push_back(pixelSampleDistance);
			m_heightIndices.push_back(pixelSampleDistance);
			m_widthIndices.push_front(-pixelSampleDistance);
			m_heightIndices.push_front(-pixelSampleDistance);
		}
		pixelSampleDistance++;
	}
}

SDLWindowRenderer::~SDLWindowRenderer()
{
	delete[] m_bloomBuffer;
	delete[] m_tempBloomBuffer;
}

void SDLWindowRenderer::Render(color imageBuffer[], const WindowInfo& windowInfo)
{
	if (windowInfo.hasChanged)
		OnWindowChange(windowInfo);

	m_rawImageBuffer = imageBuffer;
	m_finalFrameBuffer = m_bloom ? AddBloom(m_rawImageBuffer, windowInfo) : m_rawImageBuffer;

	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);
	const int windowWidth = windowInfo.width;
	const int windowHeight = windowInfo.height;

	if (m_imageSurface != nullptr)
		SDL_FreeSurface(m_imageSurface);

	m_imageSurface = SDL_CreateRGBSurface(0, windowInfo.width, windowInfo.height, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_memset(m_imageSurface->pixels, 0, m_imageSurface->h * m_imageSurface->pitch);

	for (int j = 0; j < windowHeight; j++)
	{
		for (int i = 0; i < windowWidth; i++)
		{
			color readColor = m_finalFrameBuffer[j * windowWidth + i].Clamp(0.0, 1.0);

			double rd = lineraToGamma(readColor.x());
			double gd = lineraToGamma(readColor.y());
			double bd = lineraToGamma(readColor.z());

			uint8_t r = (uint8_t)(255.999 * rd);
			uint8_t g = (uint8_t)(255.999 * gd);
			uint8_t b = (uint8_t)(255.999 * bd);
			uint8_t a = (uint8_t)255;

			Uint32 localColor = SDL_MapRGBA(m_imageSurface->format, r, g, b, a);
			SetPixel(m_imageSurface, i, j, localColor);
		}
	}

	// Set sampling to nearest pixel
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, m_imageSurface);
	
	SDL_Rect imageRect = { 0, 0, windowWidth, windowHeight };
	SDL_RenderCopy(m_renderer, texture, &imageRect, &imageRect);

	SDL_DestroyTexture(texture);
}

void SDLWindowRenderer::SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* pixels = (Uint32*)surface->pixels;
	int position = (y * surface->w) + x;
	pixels[position] = pixel;
}

color* SDLWindowRenderer::AddBloom(color imageBuffer[], const WindowInfo& windowInfo)
{
	const int pixelCount = windowInfo.height * windowInfo.width;

#if BloomMultithreading
	std::vector<uint32_t> pixelIter(pixelCount, 0);
	for (int i = 0; i < pixelCount; i++)
	{
		pixelIter[i] = i;
	}

	std::for_each(std::execution::par, pixelIter.begin(), pixelIter.end(),
		[this](uint32_t i)
		{
			if (m_rawImageBuffer[i].length_squared() > 1.f)
			{
				m_tempBloomBuffer[i] = m_rawImageBuffer[i] / m_rawImageBuffer[i].length();
				m_rawImageBuffer[i].Clamp(0, 1);
			}
			else
			{
				m_tempBloomBuffer[i] = color(0, 0, 0);
			}
		});

	std::for_each(std::execution::par, pixelIter.begin(), pixelIter.end(),
		[this, pixelCount](uint32_t i)
		{
			m_bloomBuffer[i] = HorizontalGaussianBlur(i, pixelCount);

		});

	std::for_each(std::execution::par, pixelIter.begin(), pixelIter.end(),
		[this](uint32_t i)
		{
			m_tempBloomBuffer[i] = m_bloomBuffer[i];
		});

	std::for_each(std::execution::par, pixelIter.begin(), pixelIter.end(),
		[this, pixelCount, windowInfo](uint32_t i)
		{
			m_bloomBuffer[i] = VerticalGaussianBlur(i, windowInfo.width, pixelCount);
		});

	std::for_each(std::execution::par, pixelIter.begin(), pixelIter.end(),
		[this, pixelCount, windowInfo](uint32_t i)
		{
			m_tempBloomBuffer[i] = m_bloomBuffer[i] * m_bloomIntensity + m_rawImageBuffer[i];
		});
#else
	for (int i = 0; i < pixelCount; i++)
	{
		if (imageBuffer[i].length_squared() > 1.f) 
		{
			m_tempBloomBuffer[i] = m_rawImageBuffer[i] / m_rawImageBuffer[i].length();
			m_rawImageBuffer[i].Clamp(0, 1);
		}
		else
		{
			m_tempBloomBuffer[i] = color(0, 0, 0);
		}
	}

	for (int i = 0; i < pixelCount; i++)
	{
		m_bloomBuffer[i] = HorizontalGaussianBlur(i, pixelCount);
	}

	memcpy(m_tempBloomBuffer, m_bloomBuffer, sizeof(m_bloomBuffer));

	for (int i = 0; i < pixelCount; i++)
	{
		m_tempBloomBuffer[i] = m_bloomBuffer[i];
	}

	for (int i = 0; i < pixelCount; i++)
	{
		m_bloomBuffer[i] = VerticalGaussianBlur(i, windowInfo.width, pixelCount);
	}

	for (size_t i = 0; i < pixelCount; i++)
	{
		m_tempBloomBuffer[i] = m_bloomBuffer[i] * m_bloomIntensity + imageBuffer[i];
	}
#endif // BloomMultithreading

	return m_tempBloomBuffer;
}

void SDLWindowRenderer::OnWindowChange(const WindowInfo& windowInfo)
{
	delete[] m_bloomBuffer;
	delete[] m_tempBloomBuffer;
	m_bloomBuffer = new color[windowInfo.height * windowInfo.width];
	m_tempBloomBuffer = new color[windowInfo.height * windowInfo.width];
}

color SDLWindowRenderer::HorizontalGaussianBlur(int pixelIndex, int pixelCount)
{
	std::vector<int>horizontalSamples(m_widthIndices.size(), 0);
	for (int i = 0; i < m_widthIndices.size(); i++)
	{
		horizontalSamples[i] = pixelIndex + m_widthIndices[i];
	};
	return GaussianBlur(horizontalSamples, pixelCount);
}

color SDLWindowRenderer::VerticalGaussianBlur(int pixelIndex, int windowWidth, int pixelCount)
{
	std::vector<int>verticalSamples(m_heightIndices.size(),0);
	for (int i = 0; i < m_heightIndices.size(); i++)
	{
		verticalSamples[i] = pixelIndex + m_heightIndices[i] * windowWidth;
	};
	return GaussianBlur(verticalSamples, pixelCount);
}

color SDLWindowRenderer::GaussianBlur(const std::vector<int>& sampleIndices, int pixelCount)
{
	color sampleSum = color(0, 0, 0);
	for (int i = 0; i < sampleIndices.size(); i++)
	{
		const int sampleIndex = sampleIndices[i];
		if (sampleIndex >= 0 && sampleIndex < pixelCount)
		{
			sampleSum += m_tempBloomBuffer[sampleIndex] * m_gausianWeights[i];
		}
	}
	return sampleSum;
}