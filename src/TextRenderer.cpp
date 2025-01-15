#include "TextRenderer.h"
// #include <iostream>

TTF_Font* TextRenderer::font = nullptr;
const char* TextRenderer::FONT_PATH = "assets/fonts/Arial.ttf";

bool TextRenderer::init() {
    font = TTF_OpenFont(FONT_PATH, 24);
    if (!font) {
        // 尝试系统字体
        font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 24);
    }
    return font != nullptr;
}

void TextRenderer::renderText(SDL_Renderer* renderer, const std::string& text, 
                            int x, int y, SDL_Color color, int fontSize) {
    if (!font) return;
    
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect rect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

void TextRenderer::cleanup() {
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
} 