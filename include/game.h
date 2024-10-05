#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "clickable_object.h"
#include <vector>
#include "sprite_renderer.h"
#include "game_logic.h"
#include "text_renderer.h"

// Represents the current state of the game
enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};


constexpr float PLAY_AREA = 0.8f;
constexpr int BOARD_SIZE = 8;
constexpr float SQUARE_SIZE = PLAY_AREA / BOARD_SIZE;

const float DARK_SQUARE[3] = { 0.3f, 0.45f, 0.6f };
const float LIGHT_SQUARE[3] = { 0.92f, 0.91f, 0.82f };

const float HIGHLIGHTED_DARK_SQAURE[3] = { 0.16f,0.55f,0.80f };
const float HIGHLIGHTED_LIGHT_SQUARE[3] = { 0.47f,0.77f,0.93f };

const float CAPTURE_DARK_SQUARE[3] = { 0.92f, 0.25f, 0.20f };
const float CAPTURE_LIGHT_SQUARE[3] = { 0.92f, 0.25f, 0.20f };


// Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
class Game
{
private:
    bool isPlayerBlack;
    unsigned int BOT_LVL;
    // Game-related State data
    SpriteRenderer* Renderer;
    GameLogicHandler* LogicHandler;
    TextRenderer* Text;
public:
    // game state
    GameState               State;	
    bool                    Keys[1024];
    float                   clickX,clickY;
    int                     dragging;
    float                   mouseX,mouseY;
    unsigned int            Width, Height;
    std::vector<ClickableObject> buttons;
    // constructor/destructor
    Game(unsigned int width, unsigned int height);
    ~Game();
    // initialize game state (load all shaders/textures/levels)
    void Init();
    // game loop
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void DoCollisions();
    // reset
    void ResetGame();
};

#endif
