#ifndef UTILS_H
#define UTILS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class TextRenderer {
public:
    static bool init();
    static void cleanup();
    
    static void renderText(SDL_Renderer* renderer, 
                          const std::string& text,
                          int x, int y,
                          SDL_Color color = {255, 255, 255, 255},
                          int fontSize = 24);
                          
private:
    static TTF_Font* font;
    static const char* FONT_PATH;
};

#endif
