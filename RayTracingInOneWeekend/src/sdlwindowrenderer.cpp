#include "sdlwindowrenderer.h"

void SDLWindowRenderer::Render(color imageBuffer[], const WindowInfo& windowInfo)
{
	SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
	SDL_RenderClear(m_renderer);

	int windowWidth = windowInfo.width;
	int windowHeight = windowInfo.height;
	SDL_Surface* surface = SDL_CreateRGBSurface(0, windowWidth, windowHeight, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

	SDL_memset(surface->pixels, 0, surface->h * surface->pitch);

	//std::cout << "P3\n" << m_imageWidth << ' ' << m_imageHeight << "\n255\n" << std::endl;

	for (int j = 0; j < windowHeight; j++)
	{
		for (int i = 0; i < windowWidth; i++)
		{
			imageBuffer[j * windowWidth + i].Clamp(0.0, 1.0);
			//write_color(std::cout, m_imageBuffer.get()[j * m_imageWidth + i]);

			double rd = lineraToGamma(imageBuffer[j * windowWidth + i].x());
			double gd = lineraToGamma(imageBuffer[j * windowWidth + i].y());
			double bd = lineraToGamma(imageBuffer[j * windowWidth + i].z());

			uint8_t r = (uint8_t)(255.999 * rd);
			uint8_t g = (uint8_t)(255.999 * gd);
			uint8_t b = (uint8_t)(255.999 * bd);
			uint8_t a = (uint8_t)255;

			Uint32 localColor = SDL_MapRGBA(surface->format, r, g, b, a);
			SetPixel(surface, i, j, localColor);
		}
	}

	// Set sampling to nearest pixel
	SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
	SDL_FreeSurface(surface);
	SDL_Rect imageRect = { 0, 0, windowWidth, windowHeight };
	SDL_RenderCopy(m_renderer, texture, &imageRect, &imageRect);

	SDL_DestroyTexture(texture);

	SDL_RenderPresent(m_renderer);
}

void SDLWindowRenderer::SetPixel(SDL_Surface* surface, int x, int y, Uint32 pixel)
{
	Uint32* pixels = (Uint32*)surface->pixels;
	int position = (y * surface->w) + x;
	pixels[position] = pixel;
}