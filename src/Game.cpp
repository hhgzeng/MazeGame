#include "Game.h"
#include "Utils.h"
#include <SDL2/SDL_ttf.h>
#include <fstream>
// #include <ctime>
#include <iostream>
#include "Effects.h"
// #include "TextRenderer.h"
#include <queue>
#include <map>
#include <algorithm>
#include <cmath>

Game::Game() : 
    window(nullptr), 
    renderer(nullptr), 
    currentState(GameState::MENU),
    currentMaze(nullptr),  // 先设为 nullptr
    player(nullptr),      // 先设为 nullptr
    maxUnlockedLevel(1),
    currentLevel(1),
    isRunning(false),
    moveCount(0),
    gameTime(0.0f),
    showHint(false)
{
    try {
        // 加载游戏进度
        loadGameProgress();
        
        // 初始化迷宫和玩家
        currentMaze = new Maze();
        if (!currentMaze) {
            throw std::runtime_error("Failed to create Maze");
        }
        
        player = new Player();
        if (!player) {
            throw std::runtime_error("Failed to create Player");
        }
        
        // 初始化主菜单按钮矩形区域
        startButtonRect = {300 - 10, 200 - 5, 220, 40};
        settingsButtonRect = {300 - 10, 270 - 5, 220, 40};
        exitButtonRect = {300 - 10, 340 - 5, 220, 40};

        // 初始化关卡选择按钮矩形区域（20个关卡，每行5个）
        int startX = 100;
        int startY = 150;
        int buttonWidth = 100;
        int buttonHeight = 60;
        int horizontalSpacing = 120;
        int verticalSpacing = 80;
        
        for(int row = 0; row < 4; ++row) {  // 4行
            for(int col = 0; col < 5; ++col) {  // 每行5个
                SDL_Rect rect = {
                    startX + col * horizontalSpacing,
                    startY + row * verticalSpacing,
                    buttonWidth,
                    buttonHeight
                };
                levelButtonRects.push_back(rect);
            }
        }

        // 初始化关卡完成按钮矩形区域
        nextLevelButtonRect = {300, 300, 200, 50};
        exitButtonRectComplete = {300, 400, 200, 50};

        // 初始化返回按钮的矩形区域
        backButtonRect = {50, 500, 150, 40};

        // 初始化游戏界面中的返回按钮（左侧）
        gamingBackButtonRect = {20, 20, 120, 40};

        // 计算迷宫的渲染位置和大小
        calculateMazeRenderArea();
    } catch (const std::exception& e) {
        std::cerr << "Error in Game constructor: " << e.what() << std::endl;
        cleanup();
        throw;
    }
}

void Game::calculateMazeRenderArea() {
    // 假设窗口大小为 800x600
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const int LEFT_PANEL_WIDTH = 160; // 左侧面板宽度

    // 计算迷宫区域（右侧区域）
    mazeRenderArea.x = LEFT_PANEL_WIDTH + 20; // 左侧面板宽度 + 间距
    mazeRenderArea.y = 20; // 顶部间距
    mazeRenderArea.w = WINDOW_WIDTH - LEFT_PANEL_WIDTH - 40; // 减去左侧面板和间距
    mazeRenderArea.h = WINDOW_HEIGHT - 40; // 减去上下间距
}

Game::~Game() {
    cleanup();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        return false;
    }

    // 设置固定窗口大小
    WINDOW_WIDTH = 800;
    WINDOW_HEIGHT = 600;

    window = SDL_CreateWindow("Maze Runner", 
                            SDL_WINDOWPOS_CENTERED, 
                            SDL_WINDOWPOS_CENTERED,
                            WINDOW_WIDTH, 
                            WINDOW_HEIGHT, 
                            SDL_WINDOW_SHOWN);
    
    if (!window) {
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 
                                SDL_RENDERER_ACCELERATED | 
                                SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        return false;
    }

    // 初始化 TTF
    if (TTF_Init() < 0) {
        return false;
    }

    // 初始化 TextRenderer
    if (!TextRenderer::init()) {
        return false;
    }

    return true;
}

void Game::run() {
    isRunning = true;
    Uint32 lastTime = SDL_GetTicks();
    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    while (isRunning) {
        Uint32 frameStart = SDL_GetTicks();
        
        // 处理事件
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
            switch (currentState) {
                case GameState::MENU:
                    handleMenuMouseEvents(event);
                    break;
                case GameState::LEVEL_SELECT:
                    handleLevelSelectMouseEvents(event);
                    break;
                case GameState::PLAYING:
                    handlePlayingEvents(event);
                    break;
                case GameState::LEVEL_COMPLETE:
                    handleLevelCompleteMouseEvents(event);
                    break;
                default:
                    break;
            }
        }

        // 更新游戏状态
        update();

        // 清除屏幕
        SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        SDL_RenderClear(renderer);

        // 渲染当前状态
        render();

        // 显示渲染结果
        SDL_RenderPresent(renderer);

        // 帧率控制
        int frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

void Game::update() {
    if (currentState == GameState::PLAYING) {
        static Uint32 lastTime = SDL_GetTicks();
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        gameTime += deltaTime;
    }
}

void Game::render() {
    // 根据当前状态渲染
    switch (currentState) {
        case GameState::MENU:
            renderMenu();
            break;
        case GameState::LEVEL_SELECT:
            renderLevelSelect();
            break;
        case GameState::PLAYING:
            renderGame();
            break;
        case GameState::LEVEL_COMPLETE:
            renderLevelComplete();
            break;
        case GameState::GAME_OVER:
            renderGameOver();
            break;
        default:
            break;
    }
}

void Game::cleanup() {
    saveGameProgress();
    
    if (player) {
        delete player;
        player = nullptr;
    }
    
    if (currentMaze) {
        delete currentMaze;
        currentMaze = nullptr;
    }
    
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
    
    SDL_Quit();
    TextRenderer::cleanup();
}

void Game::handleMenuEvents(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
            case SDLK_1:
                currentState = GameState::LEVEL_SELECT;
                break;
            case SDLK_2:
                currentState = GameState::RANDOM_CHALLENGE;
                generateRandomLevel();
                currentState = GameState::PLAYING;
                break;
            case SDLK_ESCAPE:
                isRunning = false;
                break;
        }
    }
}

void Game::handleLevelSelectEvents(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym >= SDLK_1 && event.key.keysym.sym <= SDLK_9) {
            int level = event.key.keysym.sym - SDLK_0;
            if (level <= currentLevel) {
                loadLevel(level);
                currentState = GameState::PLAYING;
            }
        } else if (event.key.keysym.sym == SDLK_ESCAPE) {
            currentState = GameState::MENU;
        }
    }
}

void Game::handlePlayingEvents(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        
        // 检查返回按钮
        if (isMouseOver(backButton)) {
            currentState = GameState::LEVEL_SELECT;
            return;
        }
        
        // 检查重试按钮
        if (isMouseOver(retryButton)) {
            moveCount = 0;
            gameTime = 0.0f;
            loadLevel(currentLevel);
            return;
        }
        
        // 检查提示按钮
        if (isMouseOver(hintButton)) {
            showHint = !showHint;
            // 如果需要显示提示，计算路径
            if (showHint) {
                calculateHintPath();
            }
            return;
        }
    }
    else if (event.type == SDL_KEYDOWN) {
        bool moved = false;
        switch (event.key.keysym.sym) {
            case SDLK_UP:
                moved = player->move(0, -1, currentMaze);
                break;
            case SDLK_DOWN:
                moved = player->move(0, 1, currentMaze);
                break;
            case SDLK_LEFT:
                moved = player->move(-1, 0, currentMaze);
                break;
            case SDLK_RIGHT:
                moved = player->move(1, 0, currentMaze);
                break;
        }

        if (moved) {
            moveCount++;
            if (currentMaze->isGoal(player->getX(), player->getY())) {
                if (currentLevel == maxUnlockedLevel && maxUnlockedLevel < 20) {
                    maxUnlockedLevel++;
                    saveGameProgress();
                }
                currentState = GameState::LEVEL_COMPLETE;
            }
        }
    }
}

void Game::handleGameOverEvents(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        currentState = GameState::MENU;
    }
}

void Game::handleRandomChallengeEvents(const SDL_Event& event) {
    handlePlayingEvents(event);
}

void Game::handleLevelCompleteMouseEvents(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        if (x >= nextLevelButtonRect.x && x <= (nextLevelButtonRect.x + nextLevelButtonRect.w) &&
            y >= nextLevelButtonRect.y && y <= (nextLevelButtonRect.y + nextLevelButtonRect.h)) {
            // 更新最高解锁关卡
            if (currentLevel == maxUnlockedLevel && maxUnlockedLevel < 20) {
                maxUnlockedLevel++;
                saveGameProgress();
            }
            // 加载下一关
            currentLevel++;
            if (currentLevel > 20) {
                currentLevel = 20;
            }
            loadLevel(currentLevel);
            currentState = GameState::PLAYING;
            return;
        }

        if (x >= exitButtonRectComplete.x && x <= (exitButtonRectComplete.x + exitButtonRectComplete.w) &&
            y >= exitButtonRectComplete.y && y <= (exitButtonRectComplete.y + exitButtonRectComplete.h)) {
            currentState = GameState::LEVEL_SELECT;
            return;
        }
    }
}

void Game::handleMenuMouseEvents(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        // 检测是否点击在开始按钮上
        if (x >= startButtonRect.x && x <= (startButtonRect.x + startButtonRect.w) &&
            y >= startButtonRect.y && y <= (startButtonRect.y + startButtonRect.h)) {
            currentState = GameState::LEVEL_SELECT;
            return;
        }

        // 检测是否点击在退出按钮上
        if (x >= exitButtonRect.x && x <= (exitButtonRect.x + exitButtonRect.w) &&
            y >= exitButtonRect.y && y <= (exitButtonRect.y + exitButtonRect.h)) {
            isRunning = false;
            return;
        }
    }
}

void Game::handleLevelSelectMouseEvents(const SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);

        // 检测是否点击在返回按钮上
        if (x >= backButtonRect.x && x <= (backButtonRect.x + backButtonRect.w) &&
            y >= backButtonRect.y && y <= (backButtonRect.y + backButtonRect.h)) {
            currentState = GameState::MENU;
            return;
        }

        // 检测关卡按钮的点击
        for(int i = 0; i < levelButtonRects.size(); ++i) {
            SDL_Rect rect = levelButtonRects[i];
            if (x >= rect.x && x <= (rect.x + rect.w) &&
                y >= rect.y && y <= (rect.y + rect.h)) {
                int selectedLevel = i + 1;
                if(selectedLevel <= maxUnlockedLevel) { // 只能选择已解锁的关卡
                    currentLevel = selectedLevel;  // 设置当前选择的关卡
                    loadLevel(currentLevel);
                    currentState = GameState::PLAYING;
                }
                return;
            }
        }
    }
}

void Game::renderMenu() {
    // 渐变背景
    renderGradientBackground();
    
    // 游戏标题区域
    SDL_Rect titleArea = {WINDOW_WIDTH/4, 50, WINDOW_WIDTH/2, 120};
    Effects::renderBlurPanel(renderer, titleArea);
    
    // 标题文本
    SDL_Color titleColor = {255, 255, 255, 255};
    TextRenderer::renderText(renderer, "MAZE RUNNER", 
                           titleArea.x + titleArea.w/4, 
                           titleArea.y + 40, 
                           titleColor, 48);

    // 主菜单按钮区域
    SDL_Rect menuArea = {WINDOW_WIDTH/4, 200, WINDOW_WIDTH/2, 350};
    Effects::renderBlurPanel(renderer, menuArea);

    // 更新按钮位置和大小
    int buttonWidth = 280;
    int buttonHeight = 60;
    int buttonSpacing = 80;
    int startY = 250;

    // 更新按钮矩形位置
    startButtonRect = {
        (WINDOW_WIDTH - buttonWidth)/2,
        startY,
        buttonWidth,
        buttonHeight
    };
    
    settingsButtonRect = {
        (WINDOW_WIDTH - buttonWidth)/2,
        startY + buttonSpacing,
        buttonWidth,
        buttonHeight
    };
    
    exitButtonRect = {
        (WINDOW_WIDTH - buttonWidth)/2,
        startY + buttonSpacing * 2,
        buttonWidth,
        buttonHeight
    };

    // 渲染按钮
    renderMenuButton("START GAME", startButtonRect, isMouseOver(startButtonRect));
    renderMenuButton("SETTINGS", settingsButtonRect, isMouseOver(settingsButtonRect));
    renderMenuButton("EXIT", exitButtonRect, isMouseOver(exitButtonRect));

    // 渲染粒子效果
    Effects::renderParticles(renderer);
}

void Game::renderMenuButton(const std::string& text, SDL_Rect& rect, bool isHighlighted) {
    // 按钮背景
    SDL_Color bgColor = isHighlighted ? 
                       SDL_Color{60, 60, 80, 255} : 
                       SDL_Color{40, 40, 60, 255};
    
    // 如果鼠标按下，稍微移动按钮位置
    static bool isMouseDown = false;
    if (isHighlighted && SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
        rect.x += 2;
        rect.y += 2;
        isMouseDown = true;
    } else if (isMouseDown) {
        rect.x -= 2;
        rect.y -= 2;
        isMouseDown = false;
    }
    
    // 渲染圆角矩形按钮
    Effects::renderRoundedRect(renderer, rect, 10);
    
    // 按钮发光效果
    if (isHighlighted) {
        SDL_Color glowColor = {65, 105, 225, 50};
        Effects::renderGlow(renderer, rect, glowColor);
    }
    
    // 按钮文本
    SDL_Color textColor = isHighlighted ? 
                         SDL_Color{255, 255, 255, 255} : 
                         SDL_Color{200, 200, 200, 255};
    
    int textX = rect.x + (rect.w - text.length() * 14) / 2;
    int textY = rect.y + (rect.h - 24) / 2;
    
    TextRenderer::renderText(renderer, text, textX, textY, textColor, 24);
}

bool Game::isMouseOver(const SDL_Rect& rect) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return (x >= rect.x && x <= rect.x + rect.w &&
            y >= rect.y && y <= rect.y + rect.h);
}

void Game::renderGradientBackground() {
    // 创建深色渐变背景
    for(int y = 0; y < WINDOW_HEIGHT; ++y) {
        float factor = (float)y / WINDOW_HEIGHT;
        SDL_SetRenderDrawColor(renderer, 
                             20 + factor * 20, 
                             20 + factor * 20, 
                             30 + factor * 30, 
                             255);
        SDL_RenderDrawLine(renderer, 0, y, WINDOW_WIDTH, y);
    }
}

void Game::renderButton(SDL_Renderer* renderer, 
                       const std::string& text, 
                       int x, int y, 
                       SDL_Color color) {
    // 绘制按钮背景
    SDL_Rect buttonRect = {x - 10, y - 5, 220, 40};
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &buttonRect);
    
    // 绘制按钮边框
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &buttonRect);
    
    // 绘制文本
    TextRenderer::renderText(renderer, text, x, y, color);
}

void Game::renderLevelSelect() {
    // 渐变背景
    renderGradientBackground();
    
    // 标题区域
    SDL_Rect titleArea = {100, 20, WINDOW_WIDTH - 200, 80};
    Effects::renderBlurPanel(renderer, titleArea);
    
    // 标题文本
    SDL_Color titleColor = {255, 255, 255, 255};
    TextRenderer::renderText(renderer, "SELECT LEVEL", 
                           titleArea.x + titleArea.w/3, 
                           titleArea.y + 25, 
                           titleColor, 36);

    // 关卡选择区域
    SDL_Rect levelArea = {50, 120, WINDOW_WIDTH - 100, 400};
    Effects::renderBlurPanel(renderer, levelArea);

    // 渲染关卡按钮
    SDL_Color buttonColor = {200, 200, 255, 255};
    SDL_Color lockedColor = {100, 100, 100, 255};
    SDL_Color completedColor = {100, 255, 100, 255};

    for(int i = 0; i < levelButtonRects.size(); ++i) {
        SDL_Rect& rect = levelButtonRects[i];
        bool isLocked = (i + 1) > maxUnlockedLevel;
        bool isCompleted = (i + 1) < maxUnlockedLevel;
        bool isHovered = isMouseOver(rect);
        
        // 渲染圆角按钮
        Effects::renderRoundedRect(renderer, rect, 8);
        
        // 按钮颜色
        SDL_Color color = isLocked ? lockedColor : 
                         (isCompleted ? completedColor : buttonColor);
        
        // 发光效果
        if (isHovered && !isLocked) {
            SDL_Color glowColor = {65, 105, 225, 50};
            Effects::renderGlow(renderer, rect, glowColor);
        }
        
        // 关卡数字
        std::string levelText = std::to_string(i + 1);
        SDL_Color textColor = isLocked ? 
                             SDL_Color{150, 150, 150, 255} : 
                             SDL_Color{255, 255, 255, 255};
        
        int textX = rect.x + (rect.w - levelText.length() * 20) / 2;
        int textY = rect.y + (rect.h - 24) / 2;
        TextRenderer::renderText(renderer, levelText, textX, textY, textColor, 24);
    }

    // 返回按钮
    renderMenuButton("BACK", backButtonRect, isMouseOver(backButtonRect));
    
    // 渲染粒子效果
    Effects::renderParticles(renderer);
}

void Game::renderGame() {
    // 渐变背景
    renderGradientBackground();
    
    // 左侧面板
    SDL_Rect leftPanel = {0, 0, 160, WINDOW_HEIGHT};
    Effects::renderBlurPanel(renderer, leftPanel);
    
    // 游戏信息面板
    renderGameInfoPanel();
    
    // 迷宫区域
    SDL_Rect mazeArea = mazeRenderArea;
    Effects::renderBlurPanel(renderer, mazeArea);
    
    // 渲染迷宫
    renderMaze();
    
    // 动态光效
    renderDynamicLighting();
    
    // 粒子效果
    Effects::renderParticles(renderer);
}

void Game::renderGameInfoPanel() {
    SDL_Color textColor = {255, 255, 255, 255};
    SDL_Color highlightColor = {100, 200, 255, 255};
    
    // 关卡信息
    std::string levelText = "LEVEL " + std::to_string(currentLevel);
    TextRenderer::renderText(renderer, levelText, 20, 30, highlightColor, 28);
    
    // 分割线
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 100);
    SDL_RenderDrawLine(renderer, 20, 70, 140, 70);
    
    // 游戏统计信息
    TextRenderer::renderText(renderer, "MOVES", 20, 90, textColor, 20);
    TextRenderer::renderText(renderer, std::to_string(moveCount), 20, 120, highlightColor, 24);
    
    TextRenderer::renderText(renderer, "TIME", 20, 160, textColor, 20);
    TextRenderer::renderText(renderer, formatTime(gameTime), 20, 190, highlightColor, 24);
    
    // 控制按钮
    renderControlButtons();
    
    // 小地图（可选）
    renderMiniMap();
}

void Game::renderControlButtons() {
    // 更新按钮位置
    retryButton = {20, WINDOW_HEIGHT - 180, 120, 40};
    backButton = {20, WINDOW_HEIGHT - 130, 120, 40};
    hintButton = {20, WINDOW_HEIGHT - 80, 120, 40};
    
    // 渲染按钮
    renderMenuButton("RETRY", retryButton, isMouseOver(retryButton));
    renderMenuButton("BACK", backButton, isMouseOver(backButton));
    renderMenuButton("HINT", hintButton, isMouseOver(hintButton));
}

void Game::renderMiniMap() {
    // 小地图区域
    SDL_Rect miniMapArea = {20, 240, 120, 120};
    Effects::renderBlurPanel(renderer, miniMapArea);
    
    if (!currentMaze) return;
    
    int mazeWidth = currentMaze->getWidth();
    int mazeHeight = currentMaze->getHeight();
    float cellSize = std::min(120.0f / mazeWidth, 120.0f / mazeHeight);
    
    float startX = miniMapArea.x + (120 - mazeWidth * cellSize) / 2;
    float startY = miniMapArea.y + (120 - mazeHeight * cellSize) / 2;
    
    // 渲染迷宫缩略图
    for (int y = 0; y < mazeHeight; y++) {
        for (int x = 0; x < mazeWidth; x++) {
            SDL_Rect cell = {
                static_cast<int>(startX + x * cellSize),
                static_cast<int>(startY + y * cellSize),
                static_cast<int>(cellSize),
                static_cast<int>(cellSize)
            };
            
            if (currentMaze->isWall(x, y)) {
                SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
                SDL_RenderFillRect(renderer, &cell);
            } else if (currentMaze->isGoal(x, y)) {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
    
    // 渲染玩家位置
    SDL_Rect playerPos = {
        static_cast<int>(startX + player->getX() * cellSize),
        static_cast<int>(startY + player->getY() * cellSize),
        static_cast<int>(cellSize),
        static_cast<int>(cellSize)
    };
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_RenderFillRect(renderer, &playerPos);
}

std::string Game::formatTime(float seconds) {
    int minutes = static_cast<int>(seconds) / 60;
    int secs = static_cast<int>(seconds) % 60;
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "%02d:%02d", minutes, secs);
    return std::string(buffer);
}

void Game::renderMaze() {
    if (!currentMaze || !player) return;
    
    int mazeWidth = currentMaze->getWidth();
    int mazeHeight = currentMaze->getHeight();
    
    // 计算单元格大小
    float cellWidth = static_cast<float>(mazeRenderArea.w) / mazeWidth;
    float cellHeight = static_cast<float>(mazeRenderArea.h) / mazeHeight;
    float cellSize = std::min(cellWidth, cellHeight);
    
    // 计算居中偏移
    float offsetX = mazeRenderArea.x + (mazeRenderArea.w - cellSize * mazeWidth) / 2;
    float offsetY = mazeRenderArea.y + (mazeRenderArea.h - cellSize * mazeHeight) / 2;
    
    // 渲染迷宫
    for (int y = 0; y < mazeHeight; y++) {
        for (int x = 0; x < mazeWidth; x++) {
            SDL_Rect cell = {
                static_cast<int>(offsetX + x * cellSize),
                static_cast<int>(offsetY + y * cellSize),
                static_cast<int>(cellSize + 1), // 添加1像素重叠
                static_cast<int>(cellSize + 1)  // 添加1像素重叠
            };
            
            if (currentMaze->isWall(x, y)) {
                // 墙壁
                SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
                SDL_RenderFillRect(renderer, &cell);
                // 墙壁阴影效果
                Effects::renderWallShadow(renderer, cell);
            } else if (currentMaze->isGoal(x, y)) {
                // 终点
                SDL_SetRenderDrawColor(renderer, 0, 200, 100, 255);
                SDL_RenderFillRect(renderer, &cell);
                // 终点发光效果
                Effects::renderGlow(renderer, cell, {0, 255, 100, 50});
            } else {
                // 路径
                SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
                SDL_RenderFillRect(renderer, &cell);
            }
        }
    }
    
    // 渲染提示路径（确保在玩家下方）
    if (showHint && !hintPath.empty()) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        // 首先渲染路径的主体
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 80);
        
        // 创建一个稍大的矩形来覆盖整个路径区域
        SDL_Rect pathRect = {
            static_cast<int>(offsetX + hintPath[0].x * cellSize) - 1,
            static_cast<int>(offsetY + hintPath[0].y * cellSize) - 1,
            static_cast<int>(cellSize + 2),
            static_cast<int>(cellSize + 2)
        };
        
        // 计算路径的边界
        for (const auto& point : hintPath) {
            int newWidth = static_cast<int>(offsetX + point.x * cellSize + cellSize + 2 - pathRect.x);
            int newHeight = static_cast<int>(offsetY + point.y * cellSize + cellSize + 2 - pathRect.y);
            pathRect.x = std::min(pathRect.x, static_cast<int>(offsetX + point.x * cellSize) - 1);
            pathRect.y = std::min(pathRect.y, static_cast<int>(offsetY + point.y * cellSize) - 1);
            pathRect.w = std::max(pathRect.w, newWidth);
            pathRect.h = std::max(pathRect.h, newHeight);
        }
        
        // 渲染整个路径区域
        for (size_t i = 0; i < hintPath.size(); ++i) {
            const auto& point = hintPath[i];
            
            // 渲染当前格子
            SDL_Rect cellRect = {
                static_cast<int>(offsetX + point.x * cellSize),
                static_cast<int>(offsetY + point.y * cellSize),
                static_cast<int>(cellSize),
                static_cast<int>(cellSize)
            };
            SDL_RenderFillRect(renderer, &cellRect);
            
            // 如果不是最后一个点，渲染连接
            if (i < hintPath.size() - 1) {
                const auto& nextPoint = hintPath[i + 1];
                
                // 计算连接区域
                if (point.x == nextPoint.x) { // 垂直连接
                    SDL_Rect connector = {
                        static_cast<int>(offsetX + point.x * cellSize),
                        static_cast<int>(offsetY + std::min(point.y, nextPoint.y) * cellSize),
                        static_cast<int>(cellSize),
                        static_cast<int>(std::abs(nextPoint.y - point.y) * cellSize + cellSize)
                    };
                    SDL_RenderFillRect(renderer, &connector);
                } else { // 水平连接
                    SDL_Rect connector = {
                        static_cast<int>(offsetX + std::min(point.x, nextPoint.x) * cellSize),
                        static_cast<int>(offsetY + point.y * cellSize),
                        static_cast<int>(std::abs(nextPoint.x - point.x) * cellSize + cellSize),
                        static_cast<int>(cellSize)
                    };
                    SDL_RenderFillRect(renderer, &connector);
                }
            }
        }
        
        // 添加柔和的边缘效果
        SDL_SetRenderDrawColor(renderer, 255, 235, 100, 30);
        for (const auto& point : hintPath) {
            SDL_Rect edgeRect = {
                static_cast<int>(offsetX + point.x * cellSize) - 1,
                static_cast<int>(offsetY + point.y * cellSize) - 1,
                static_cast<int>(cellSize + 2),
                static_cast<int>(cellSize + 2)
            };
            SDL_RenderDrawRect(renderer, &edgeRect);
        }
    }
    
    // 渲染玩家
    SDL_Rect playerRect = {
        static_cast<int>(offsetX + player->getX() * cellSize),
        static_cast<int>(offsetY + player->getY() * cellSize),
        static_cast<int>(cellSize),
        static_cast<int>(cellSize)
    };
    
    // 玩家发光效果
    Effects::renderGlow(renderer, playerRect, {255, 100, 100, 50});
    
    // 玩家主体
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    Effects::renderRoundedRect(renderer, playerRect, static_cast<int>(cellSize/4));
}

void Game::renderGameOver() {
    SDL_Color white = {255, 255, 255, 255};
    TextRenderer::renderText(renderer, "CONGRATULATIONS!", 350, 250, white, 36);
    TextRenderer::renderText(renderer, "PRESS ANY KEY TO RETURN", 300, 350, white);
}

void Game::renderLevelComplete() {
    // 渐变背景
    renderGradientBackground();
    
    // 全屏半透明遮罩
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect fullScreen = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &fullScreen);
    
    // 完成信息面板（充满屏幕）
    SDL_Rect completePanel = {50, 50, WINDOW_WIDTH - 100, WINDOW_HEIGHT - 100};
    Effects::renderBlurPanel(renderer, completePanel);
    
    // 标题
    SDL_Color titleColor = {255, 255, 255, 255};
    TextRenderer::renderText(renderer, "LEVEL COMPLETE!", 
                           completePanel.x + completePanel.w/3, 
                           completePanel.y + 60, 
                           titleColor, 48);
    
    // 统计信息
    SDL_Color textColor = {200, 200, 200, 255};
    SDL_Color valueColor = {100, 200, 255, 255};
    
    int infoY = completePanel.y + 180;
    
    // 移动次数
    TextRenderer::renderText(renderer, "Moves:", 
                           completePanel.x + 200, 
                           infoY, 
                           textColor, 32);
    TextRenderer::renderText(renderer, std::to_string(moveCount), 
                           completePanel.x + 500, 
                           infoY, 
                           valueColor, 32);
    
    // 完成时间
    TextRenderer::renderText(renderer, "Time:", 
                           completePanel.x + 200, 
                           infoY + 60, 
                           textColor, 32);
    TextRenderer::renderText(renderer, formatTime(gameTime), 
                           completePanel.x + 500, 
                           infoY + 60, 
                           valueColor, 32);
    
    // 竖直排列按钮
    int buttonWidth = 300;
    int buttonHeight = 60;
    int buttonSpacing = 80;
    int startY = infoY + 160;
    
    // 更新按钮位置
    nextLevelButtonRect = {
        (WINDOW_WIDTH - buttonWidth) / 2,
        startY,
        buttonWidth,
        buttonHeight
    };
    
    exitButtonRectComplete = {
        (WINDOW_WIDTH - buttonWidth) / 2,
        startY + buttonSpacing,
        buttonWidth,
        buttonHeight
    };
    
    // 渲染按钮
    renderMenuButton("NEXT LEVEL", nextLevelButtonRect, 
                    isMouseOver(nextLevelButtonRect));
    renderMenuButton("BACK TO MENU", exitButtonRectComplete, 
                    isMouseOver(exitButtonRectComplete));
    
    // 添加粒子效果
    Effects::renderParticles(renderer);
}

void Game::saveGameProgress() {
    std::ofstream file("save/progress.dat", std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&maxUnlockedLevel), sizeof(maxUnlockedLevel));
        file.close();
    }
}

void Game::loadGameProgress() {
    std::ifstream file("save/progress.dat", std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&maxUnlockedLevel), sizeof(maxUnlockedLevel));
        file.close();
    } else {
        // 如果文件不存在，创建目录和文件
        system("mkdir -p save");
        maxUnlockedLevel = 1;
        saveGameProgress();
    }
}

void Game::updateProgress() {
    // 如果完成当前关卡，解锁下一关
    if (currentState == GameState::LEVEL_COMPLETE) {
        if (currentLevel < 20) { // 最多20关
            currentLevel++;
            saveGameProgress(); // 每次更新进度后立即保存
        }
    }
}

bool Game::loadLevel(int level) {
    if (!currentMaze || !player) {
        std::cerr << "Maze or player is null" << std::endl;
        return false;
    }
    
    try {
        if (!currentMaze->loadLevel(level)) {
            std::cerr << "Failed to load level " << level << std::endl;
            return false;
        }
        
        // 重置玩家位置和游戏统计
        player->setPosition(1, 1);
        player->setMaze(currentMaze);
        currentLevel = level;
        moveCount = 0;
        gameTime = 0.0f;
        showHint = false;
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error in loadLevel: " << e.what() << std::endl;
        return false;
    }
}

void Game::generateRandomLevel() {
    int size = 3 + (maxRandomLevel - 1) * 2;
    if (size > 10) size = 10;
    currentMaze->generateRandomMaze(size);
    player->setPosition(1, 1);
}

void Game::transitionToState(GameState newState) {
    // 淡出效果
    for(int alpha = 0; alpha < 255; alpha += 5) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }
    
    currentState = newState;
    
    // 淡入效果
    for(int alpha = 255; alpha > 0; alpha -= 5) {
        render();
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, alpha);
        SDL_RenderFillRect(renderer, nullptr);
        SDL_RenderPresent(renderer);
        SDL_Delay(1);
    }
}

void Game::loadResources() {
    // 初始化 TTF
    if (TTF_Init() < 0) {
        std::cout << "TTF initialization failed: " << TTF_GetError() << std::endl;
        return;
    }
    
    // 加载字体和其他资源
    // TODO: 添加具体的资源加载代码
}

void Game::renderButtonGlow(const SDL_Rect& rect) {
    SDL_Color glowColor = {65, 105, 225, 100}; // 蓝色发光效果
    Effects::renderGlow(renderer, rect, glowColor);
}

void Game::renderParticles() {
    static std::vector<SDL_Point> particles;
    static int frame = 0;
    
    // 每隔一段时间添加新粒子
    if (frame++ % 10 == 0) {
        SDL_Point particle = {
            rand() % WINDOW_WIDTH,
            rand() % WINDOW_HEIGHT
        };
        particles.push_back(particle);
    }
    
    // 渲染并更新粒子
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 50);
    for (auto& p : particles) {
        SDL_RenderDrawPoint(renderer, p.x, p.y);
        p.y = (p.y + 1) % WINDOW_HEIGHT;
    }
    
    // 限制粒子数量
    if (particles.size() > 100) {
        particles.erase(particles.begin());
    }
}

void Game::renderMazeGlow(const SDL_Rect& area) {
    SDL_Color glowColor = {65, 105, 225, 50}; // 蓝色发光效果
    Effects::renderGlow(renderer, area, glowColor);
}

void Game::renderPlayerTrail() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    for (const auto& point : playerTrail) {
        SDL_RenderDrawPoint(renderer, point.x, point.y);
    }
}

void Game::renderDynamicLighting() {
    // 获取玩家位置
    SDL_Point playerPos = {
        static_cast<int>(player->getX() * mazeRenderArea.w),
        static_cast<int>(player->getY() * mazeRenderArea.h)
    };
    
    // 创建动态光照效果
    SDL_Color lightColor = {255, 255, 200, 100};
    SDL_Rect lightArea = {
        playerPos.x - 50,
        playerPos.y - 50,
        100,
        100
    };
    Effects::renderGlow(renderer, lightArea, lightColor);
}

void Game::calculateHintPath() {
    if (!currentMaze || !player) return;
    
    // 清除旧的提示路径
    hintPath.clear();
    
    // 使用简单的BFS寻路算法
    std::queue<SDL_Point> queue;
    std::map<SDL_Point, SDL_Point, PointCompare> cameFrom;
    SDL_Point start = {player->getX(), player->getY()};  // 从当前位置开始
    SDL_Point goal;
    
    // 找到终点位置
    for (int y = 0; y < currentMaze->getHeight(); y++) {
        for (int x = 0; x < currentMaze->getWidth(); x++) {
            if (currentMaze->isGoal(x, y)) {
                goal = {x, y};
                break;
            }
        }
    }
    
    queue.push(start);
    cameFrom[start] = start;
    
    // BFS搜索
    while (!queue.empty()) {
        SDL_Point current = queue.front();
        queue.pop();
        
        if (current.x == goal.x && current.y == goal.y) {
            break;
        }
        
        // 检查四个方向
        const int dx[] = {0, 1, 0, -1};
        const int dy[] = {-1, 0, 1, 0};
        
        for (int i = 0; i < 4; i++) {
            SDL_Point next = {current.x + dx[i], current.y + dy[i]};
            if (!currentMaze->isWall(next.x, next.y) && cameFrom.find(next) == cameFrom.end()) {
                queue.push(next);
                cameFrom[next] = current;
            }
        }
    }
    
    // 重建路径
    SDL_Point current = goal;
    while (!(current.x == start.x && current.y == start.y)) {
        hintPath.push_back(current);
        current = cameFrom[current];
    }
    std::reverse(hintPath.begin(), hintPath.end());
}

// 私有辅助方法的实现...
