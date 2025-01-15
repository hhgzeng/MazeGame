#ifndef EFFECTS_H
#define EFFECTS_H

#include <SDL2/SDL.h>

class Effects {
public:
    static void renderBlurPanel(SDL_Renderer* renderer, const SDL_Rect& rect);
    static void renderGlow(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color);
    static void renderParticles(SDL_Renderer* renderer);
    static void renderRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, int radius);
    static void renderWallShadow(SDL_Renderer* renderer, const SDL_Rect& rect);
    static void renderProgressBar(SDL_Renderer* renderer, const SDL_Rect& rect, float progress);
};

#endif 