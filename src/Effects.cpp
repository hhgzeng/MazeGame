#include "Effects.h"
#include <vector>
#include <cmath>

struct Particle {
    float x, y;
    float vx, vy;
    float life;
    SDL_Color color;
};

static std::vector<Particle> particles;

void Effects::renderBlurPanel(SDL_Renderer* renderer, const SDL_Rect& rect) {
    // 半透明背景
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 200);
    SDL_RenderFillRect(renderer, &rect);
    
    // 边框
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderDrawRect(renderer, &rect);
}

void Effects::renderGlow(SDL_Renderer* renderer, const SDL_Rect& rect, SDL_Color color) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

void Effects::renderParticles(SDL_Renderer* renderer) {
    static std::vector<Particle> particles;
    
    // 添加新粒子
    if(particles.size() < 50 && rand() % 10 == 0) {
        Particle p;
        p.x = rand() % 800;
        p.y = rand() % 600;
        p.vx = (rand() % 100 - 50) / 100.0f;
        p.vy = (rand() % 100 - 50) / 100.0f;
        p.life = 1.0f;
        p.color = {255, 255, 255, 100};
        particles.push_back(p);
    }
    
    // 更新和渲染粒子
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    for(auto it = particles.begin(); it != particles.end();) {
        it->x += it->vx;
        it->y += it->vy;
        it->life -= 0.01f;
        
        if(it->life <= 0) {
            it = particles.erase(it);
            continue;
        }
        
        SDL_SetRenderDrawColor(renderer, 
                             it->color.r, 
                             it->color.g, 
                             it->color.b, 
                             (Uint8)(it->color.a * it->life));
        SDL_RenderDrawPoint(renderer, (int)it->x, (int)it->y);
        ++it;
    }
}

void Effects::renderRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, int radius) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // 填充主体
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    SDL_Rect innerRect = {
        rect.x + radius,
        rect.y + radius,
        rect.w - 2 * radius,
        rect.h - 2 * radius
    };
    SDL_RenderFillRect(renderer, &innerRect);
    
    // 绘制圆角
    for(int w = 0; w < radius * 2; w++) {
        for(int h = 0; h < radius * 2; h++) {
            float distance = sqrt((radius - w) * (radius - w) + 
                                (radius - h) * (radius - h));
            
            if(distance <= radius) {
                // 左上角
                SDL_RenderDrawPoint(renderer, 
                                  rect.x + w, 
                                  rect.y + h);
                // 右上角
                SDL_RenderDrawPoint(renderer, 
                                  rect.x + rect.w - w - 1, 
                                  rect.y + h);
                // 左下角
                SDL_RenderDrawPoint(renderer, 
                                  rect.x + w, 
                                  rect.y + rect.h - h - 1);
                // 右下角
                SDL_RenderDrawPoint(renderer, 
                                  rect.x + rect.w - w - 1, 
                                  rect.y + rect.h - h - 1);
            }
        }
    }
}

void Effects::renderWallShadow(SDL_Renderer* renderer, const SDL_Rect& rect) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // 右侧阴影
    SDL_Rect shadowRight = {
        rect.x + rect.w,
        rect.y + 2,
        2,
        rect.h - 2
    };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
    SDL_RenderFillRect(renderer, &shadowRight);
    
    // 底部阴影
    SDL_Rect shadowBottom = {
        rect.x + 2,
        rect.y + rect.h,
        rect.w - 2,
        2
    };
    SDL_RenderFillRect(renderer, &shadowBottom);
}

void Effects::renderProgressBar(SDL_Renderer* renderer, const SDL_Rect& rect, float progress) {
    // 背景
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    // 进度条
    SDL_Rect progressRect = {
        rect.x,
        rect.y,
        static_cast<int>(rect.w * progress),
        rect.h
    };
    SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
    SDL_RenderFillRect(renderer, &progressRect);
    
    // 边框
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderDrawRect(renderer, &rect);
} 