#pragma once

#include <vector>
#include <deque>

#include "SDL_render.h"

#include "tools/rtweekend.h"
#include "tools/windowinfo.h"
#include "tools/color.h"

class SDLWindowRenderer
{
public:
	SDLWindowRenderer(SDL_Renderer* renderer, const WindowInfo& windowInfo);
	~SDLWindowRenderer();
	void Render(color imageBuffer[], const WindowInfo& windowInfo);

	void SetBloom(bool value) { m_bloom = value; }

private:
	bool m_bloom = false;
	const float m_stdDev = 2.f;
	const float m_bloomIntensity = 1.5f;
	const float m_eulerNumber = 2.71828182845904523536f;
	SDL_Renderer* m_renderer = nullptr;
	color* m_rawImageBuffer = nullptr;
	color* m_finalFrameBuffer = nullptr;
	color* m_bloomBuffer;
	color* m_tempBloomBuffer;
	std::deque<float> m_gausianWeights;
	std::deque<int> m_heightIndices;
	std::deque<int> m_widthIndices;

	void OnWindowChange(const WindowInfo& windowInfo);
	void SetPixel(SDL_Surface* surface, int x, int y, Uint32 color);
	color* AddBloom(color imageBuffer[], const WindowInfo& windowInfo);
	inline color HorizontalGaussianBlur(int pixelIndex, int pixelCount);
	inline color VerticalGaussianBlur(int pixelIndex, int windowWidth, int pixelCount);
	inline color GaussianBlur(const std::vector<int>& sampleIndices, int pixelCount);
};

