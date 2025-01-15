#include "Maze.h"
#include <fstream>
#include <random>
#include <stack>
#include <iostream>

Maze::Maze() : width(0), height(0) {
    // 初始化 mazeData 为一个有效的空迷宫
    mazeData = std::vector<std::vector<int>>(1, std::vector<int>(1, PATH));
}

Maze::~Maze() {}

bool Maze::loadLevel(int level) {
    std::string filename = "levels/level" + std::to_string(level) + ".txt";
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Cannot open level file: " << filename << std::endl;
        return false;
    }
    
    std::vector<std::string> lines;
    std::string line;
    
    // 读取所有行
    while (std::getline(file, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    
    if (lines.empty()) {
        std::cerr << "Level file is empty: " << filename << std::endl;
        return false;
    }
    
    try {
        // 设置迷宫尺寸
        height = lines.size();
        width = lines[0].length();
        
        // 初始化迷宫数据
        mazeData.clear();
        mazeData.resize(height, std::vector<int>(width));
        
        // 解析迷宫数据
        for (int y = 0; y < height; y++) {
            if (lines[y].length() != width) {
                std::cerr << "Invalid maze format: inconsistent width at line " << y << std::endl;
                return false;
            }
            
            for (int x = 0; x < width; x++) {
                char c = lines[y][x];
                switch (c) {
                    case '#':
                        mazeData[y][x] = WALL;
                        break;
                    case '.':
                    case 'P': // 玩家起点也是路径
                        mazeData[y][x] = PATH;
                        break;
                    case 'G':
                        mazeData[y][x] = GOAL;
                        goalPosition = {x, y};
                        break;
                    default:
                        mazeData[y][x] = PATH;
                        break;
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading level: " << e.what() << std::endl;
        return false;
    }
}

void Maze::generateRandomMaze(int size) {
    width = height = size;
    // 初始化迷宫为全墙
    mazeData = std::vector<std::vector<int>>(size, std::vector<int>(size, WALL));
    
    // 使用DFS生成迷宫
    generateMazeUsingDFS(size);
    
    // 设置起点和终点
    mazeData[1][1] = PATH;  // 起点
    goalPosition = {size - 2, size - 2};
    mazeData[goalPosition.y][goalPosition.x] = GOAL;
}

void Maze::generateMazeUsingDFS(int size) {
    std::stack<SDL_Point> stack;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // 从(1,1)开始生成
    stack.push({1, 1});
    mazeData[1][1] = PATH;
    
    while (!stack.empty()) {
        SDL_Point current = stack.top();
        
        // 可能的移动方向：上、右、下、左
        std::vector<SDL_Point> directions = {
            {0, -2}, {2, 0}, {0, 2}, {-2, 0}
        };
        
        // 随机打乱方向
        std::shuffle(directions.begin(), directions.end(), gen);
        
        bool foundPath = false;
        for (const auto& dir : directions) {
            int newX = current.x + dir.x;
            int newY = current.y + dir.y;
            
            if (newX > 0 && newX < size - 1 && newY > 0 && newY < size - 1 
                && mazeData[newY][newX] == WALL) {
                // 打通墙壁
                mazeData[current.y + dir.y/2][current.x + dir.x/2] = PATH;
                mazeData[newY][newX] = PATH;
                stack.push({newX, newY});
                foundPath = true;
                break;
            }
        }
        
        if (!foundPath) {
            stack.pop();
        }
    }
}

void Maze::render(SDL_Renderer* renderer) {
    const int CELL_SIZE = 30;  // 每个格子的大小
    
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
            
            switch (mazeData[y][x]) {
                case WALL:
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // 黑色墙壁
                    break;
                case PATH:
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // 白色路径
                    break;
                case GOAL:
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // 绿色终点
                    break;
            }
            
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

bool Maze::isWall(int x, int y) const {
    if (!isValidPosition(x, y)) return true;
    return mazeData[y][x] == WALL;
}

bool Maze::isGoal(int x, int y) const {
    if (!isValidPosition(x, y)) return false;
    return mazeData[y][x] == GOAL;
}

bool Maze::isValidPosition(int x, int y) const {
    return x >= 0 && x < width && y >= 0 && y < height && !mazeData.empty() && !mazeData[0].empty();
}

bool Maze::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    mazeData.clear();
    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        for (char c : line) {
            switch (c) {
                case '#': row.push_back(WALL); break;
                case '.': row.push_back(PATH); break;
                case 'G': 
                    row.push_back(GOAL);
                    goalPosition = {static_cast<int>(row.size() - 1), 
                                  static_cast<int>(mazeData.size())};
                    break;
            }
        }
        if (!row.empty()) {
            mazeData.push_back(row);
        }
    }
    
    if (!mazeData.empty()) {
        height = mazeData.size();
        width = mazeData[0].size();
    }
    
    return true;
}
