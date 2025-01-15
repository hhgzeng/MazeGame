#include "Player.h"
#include <iostream>

void Player::handleInput(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN) return;
    
    switch (event.key.keysym.sym) {
        case SDLK_UP:
            move(0, -1, currentMaze);
            break;
        case SDLK_DOWN:
            move(0, 1, currentMaze);
            break;
        case SDLK_LEFT:
            move(-1, 0, currentMaze);
            break;
        case SDLK_RIGHT:
            move(1, 0, currentMaze);
            break;
    }
}

void Player::render(SDL_Renderer* renderer) {
    // 计算玩家在屏幕上的位置
    SDL_Rect playerRect = {
        x * PLAYER_SIZE,
        y * PLAYER_SIZE,
        PLAYER_SIZE,
        PLAYER_SIZE
    };
    
    // 绘制玩家（红色方块）
    SDL_SetRenderDrawColor(renderer, 
                          playerColor.r, 
                          playerColor.g, 
                          playerColor.b, 
                          playerColor.a);
    SDL_RenderFillRect(renderer, &playerRect);
    
    // 添加边框使玩家更容易看见
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &playerRect);
}

void Player::setPosition(int newX, int newY) {
    if (!currentMaze) {
        std::cerr << "Error: currentMaze is null in setPosition" << std::endl;
        return;
    }
    
    if (currentMaze->isValidPosition(newX, newY) && !currentMaze->isWall(newX, newY)) {
        x = newX;
        y = newY;
    } else {
        std::cerr << "Invalid position: " << newX << "," << newY << std::endl;
    }
}

bool Player::move(int dx, int dy, Maze* maze) {
    if (!maze) {
        std::cerr << "Error: maze is null in move" << std::endl;
        return false;
    }
    
    int newX = x + dx;
    int newY = y + dy;
    
    if (maze->isValidPosition(newX, newY) && !maze->isWall(newX, newY)) {
        x = newX;
        y = newY;
        return true;
    }
    return false;
}

bool Player::hasReachedGoal() const {
    return currentMaze->isGoal(x, y);
}