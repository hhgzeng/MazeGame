# 迷宫游戏 (Maze Game)

一个基于SDL2开发的迷宫探索游戏。玩家需要在不同难度的迷宫中找到出口，支持多个关卡和随机迷宫挑战。

## 功能特点

- 20个预设关卡
- 随机迷宫挑战模式
- 关卡进度保存
- 提示系统
- 移动计数和计时器
- 美观的用户界面和视觉效果

## 系统要求

- macOS/Linux/Windows
- C++20 或更高版本
- SDL2
- SDL2_ttf
- CMake 3.10或更高版本

## 安装依赖

### macOS (使用Homebrew)

```bash
brew install sdl2 sdl2_ttf
```

### Linux (使用包管理器)

```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

## 构建和运行

1. 克隆仓库：

```bash
git clone <repository-url>
cd MazeGame
```

2. 构建项目：

```bash
mkdir -p build && cd build
cmake ..
make
```

3. 运行游戏：

```bash
./MazeGame
```

## 项目结构

```
MazeGame/
├── src/                 # 源代码文件
│   ├── main.cpp        # 主程序入口
│   ├── Game.cpp        # 游戏核心逻辑
│   ├── Maze.cpp        # 迷宫生成与管理
│   ├── Player.cpp      # 玩家控制
│   ├── Effects.cpp     # 特效系统
│   ├── Utils.cpp       # 工具函数
│   └── TextRenderer.cpp # 文本渲染
├── include/            # 头文件
│   ├── Game.h         # 游戏类定义
│   ├── Maze.h         # 迷宫类定义
│   ├── Player.h       # 玩家类定义
│   ├── Effects.h      # 特效系统定义
│   ├── Utils.h        # 工具函数定义
│   └── TextRenderer.h # 文本渲染定义
├── levels/             # 关卡文件
│   ├── level1.txt     # 第1关 - 31x31迷宫
│   ├── level2.txt     # 第2关 - 5x5迷宫
│   └── ...            # 更多关卡
├── save/               # 存档目录
│   └── progress.dat   # 游戏进度存档
├── build/             # 构建目录
├── .clangd            # clangd配置
├── compile_commands.json # 编译命令数据库
├── CMakeLists.txt     # CMake构建配置
└── Makefile           # 自动生成的构建文件
```

## 游戏玩法

### 控制方式
- ↑：向上移动
- ↓：向下移动
- ←：向左移动
- →：向右移动
- ESC：返回主菜单

### 游戏规则
1. 玩家从起点(P)出发，需要到达终点(G)
2. 黑色区域代表墙壁(#)，不能穿越
3. 白色区域代表通道(.)，可以移动
4. 每次只能移动一格
5. 完成关卡后自动解锁下一关
6. 随机挑战模式难度递增（3x3到10x10）

## 开发说明

### 编译配置
- 使用CMake构建系统（最低版本3.10）
- 支持C++20标准
- 包含SDL2和SDL2_ttf依赖
- 自动复制关卡资源文件
- 自动创建存档目录

### IDE支持
- 包含完整的compile_commands.json
- 配置.clangd文件支持代码补全
- 支持VSCode/CLion等主流IDE

### 存档系统
- 存档位置：save/progress.dat
- 记录内容：
  - 当前关卡进度
  - 随机迷宫最高记录
  - 游戏统计数据
  - 每关完成时间

## 调试和开发

### 环境配置
1. 确保已安装所需依赖
2. 检查CMake版本 (>= 3.10)
3. 确保编译器支持C++20
4. 配置IDE的clangd支持

### 常见问题
1. 编译错误：
   - 检查SDL2相关库是否正确安装
   - 确认CMake版本是否满足要求
   - 验证编译器C++20支持
2. 运行崩溃：
   - 确认关卡文件完整性
   - 检查文件权限
3. 存档问题：
   - 确保save目录存在
   - 检查写入权限

## 未来计划

- [ ] 添加背景音乐和音效
- [ ] 实现关卡编辑器
- [ ] 优化随机迷宫生成算法
- [ ] 添加更多视觉效果
- [ ] 支持手柄控制

## 贡献

欢迎提交Issue和Pull Request来改进游戏。在提交代码前，请确保：
1. 代码符合项目规范
2. 通过所有测试
3. 更新相关文档

## 许可证

[待添加许可证信息]