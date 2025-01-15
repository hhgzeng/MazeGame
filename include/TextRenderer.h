#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextRenderer {
public:
    static bool init();
    static void renderText(SDL_Renderer* renderer, const std::string& text, 
                         int x, int y, SDL_Color color, int fontSize = 24);
    static void cleanup();

private:
    static TTF_Font* font;
    static const char* FONT_PATH;
};

#endif 