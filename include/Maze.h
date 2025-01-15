#ifndef MAZE_H
#define MAZE_H

#include <SDL2/SDL.h>
#include <vector>
#include <string>

class Maze {
public:
    Maze();
    ~Maze();

    // 迷宫生成和加载
    bool loadLevel(int level);
    void generateRandomMaze(int size);
    
    // 渲染
    void render(SDL_Renderer* renderer);
    
    // 迷宫属性
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    bool isWall(int x, int y) const;
    bool isGoal(int x, int y) const;
    bool isValidPosition(int x, int y) const;

    // 将常量定义改为 static constexpr
    static constexpr int WALL = 1;
    static constexpr int PATH = 0;
    static constexpr int GOAL = 2;

private:
    std::vector<std::vector<int>> mazeData;
    int width;
    int height;
    SDL_Point goalPosition;
    
    // 迷宫生成算法
    void generateMazeUsingDFS(int size);
    void carvePassages(int x, int y);
    
    // 文件操作
    bool loadFromFile(const std::string& filename);
};

#endif
