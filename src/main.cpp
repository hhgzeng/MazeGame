#include "Game.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Game game;
    
    if (!game.init()) {
        std::cerr << "游戏初始化失败！" << std::endl;
        return 1;
    }
    
    game.run();
    
    return 0;
}