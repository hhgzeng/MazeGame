#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h>
#include "Maze.h"

class Player {
public:
    Player() : x(1), y(1), currentMaze(nullptr) {}
    
    void handleInput(const SDL_Event& event);
    void render(SDL_Renderer* renderer);
    
    // 位置相关
    int getX() const { return x; }
    int getY() const { return y; }
    void setPosition(int newX, int newY);
    
    bool hasReachedGoal() const;
    bool move(int dx, int dy, Maze* maze);
    
    // 设置当前迷宫
    void setMaze(Maze* maze) { currentMaze = maze; }

private:
    int x, y;
    Maze* currentMaze;
    
    // 渲染属性
    static const int PLAYER_SIZE = 30;
    SDL_Color playerColor = {255, 0, 0, 255}; // 红色
};

#endif