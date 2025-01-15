#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "Maze.h"
#include "Player.h"
#include <string>
#include <vector>

// 前向声明
class TextRenderer;

enum class GameState {
    MENU,           // 主菜单
    LEVEL_SELECT,   // 关卡选择
    PLAYING,        // 游戏进行中
    GAME_OVER,      // 游戏结束
    RANDOM_CHALLENGE, // 随机迷宫挑战
    LEVEL_COMPLETE // 关卡完成
};

struct PointCompare {
    bool operator()(const SDL_Point& a, const SDL_Point& b) const {
        return a.x < b.x || (a.x == b.x && a.y < b.y);
    }
};

class Game {
public:
    Game();
    ~Game();
    
    bool init();
    void run();
    void handleEvents();
    void update();
    void render();
    void cleanup();

    // 新增状态转换方法
    void transitionToState(GameState newState);

private:
    int WINDOW_WIDTH;
    int WINDOW_HEIGHT;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    GameState currentState;
    
    // 游戏组件
    Maze* currentMaze;
    Player* player;
    
    // 游戏数据
    int currentLevel;
    int maxRandomLevel;
    int maxUnlockedLevel;
    bool isRunning;
    
    // 事件处理方法
    void handleMenuEvents(const SDL_Event& event);
    void handleLevelSelectEvents(const SDL_Event& event);
    void handlePlayingEvents(const SDL_Event& event);
    void handleGameOverEvents(const SDL_Event& event);
    void handleRandomChallengeEvents(const SDL_Event& event);
    
    // 渲染方法
    void renderMenu();
    void renderLevelSelect();
    void renderGame();
    void renderGameOver();
    void renderLevelComplete();
    
    // 游戏逻辑方法
    bool loadLevel(int level);
    void generateRandomLevel();
    
    // UI 渲染方法
    void renderButton(SDL_Renderer* renderer, 
                     const std::string& text, 
                     int x, int y, 
                     SDL_Color color);
    
    struct GameProgress {
        int currentLevel;
        int maxRandomLevel;
        std::vector<float> levelTimes;
        int totalGames;
        std::string lastPlayDate;
    };
    
    GameProgress progress;
    void saveGameProgress();
    void loadGameProgress();
    void updateProgress();
    
    // 添加按钮的矩形区域
    SDL_Rect startButtonRect;
    SDL_Rect settingsButtonRect;
    SDL_Rect exitButtonRect;
    
    // 添加关卡按钮的矩形区域
    std::vector<SDL_Rect> levelButtonRects;
    
    // 关卡完成按钮的矩形区域
    SDL_Rect nextLevelButtonRect;
    SDL_Rect exitButtonRectComplete;
    
    // 添加返回按钮的矩形区域
    SDL_Rect backButtonRect;
    
    // 添加游戏界面中的返回按钮矩形区域
    SDL_Rect gamingBackButtonRect;
    
    // 迷宫渲染区域
    SDL_Rect mazeRenderArea;
    
    // 新增特效相关成员
    std::vector<SDL_Point> playerTrail;
    SDL_Color currentThemeColor;
    
    // 新增渲染方法
    void renderGradientBackground();
    void renderMenuButton(const std::string& text, SDL_Rect& rect, bool isHighlighted);
    void renderControlButton(const std::string& text, const SDL_Rect& rect);
    void renderControlButtons();
    void renderMazeGlow(const SDL_Rect& area);
    void renderPlayerTrail();
    void renderDynamicLighting();
    
    // 辅助方法
    bool isMouseOver(const SDL_Rect& rect);
    std::string formatTime(float time);
    void updateThemeColor();
    
    // 私有辅助方法
    void handleMenuMouseEvents(const SDL_Event& event);
    void handleLevelSelectMouseEvents(const SDL_Event& event);
    void handleLevelCompleteMouseEvents(const SDL_Event& event);
    
    // 新增方法
    void calculateMazeRenderArea();
    void renderGameLeftPanel();
    void renderMaze();
    void loadResources();
    void renderMiniMap();
    void renderGameInfoPanel();
    
    void renderButtonGlow(const SDL_Rect& rect);
    
    // 新增渲染方法
    void renderParticles();
    
    void renderGameInfo();
    
    void renderMazeWithEffects(const SDL_Rect& area);
    
    // 游戏统计
    int moveCount;
    float gameTime;
    bool showHint;
    std::vector<SDL_Point> hintPath;
    
    // 新增按钮
    SDL_Rect retryButton;
    SDL_Rect hintButton;
    
    void calculateHintPath();
    SDL_Rect backButton;
};

#endif
