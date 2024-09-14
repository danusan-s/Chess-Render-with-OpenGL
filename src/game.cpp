#include <vector>
#include "game.h"
#include "resource_manager.h"
#include "game_object.h"
#include "chess.h"
#include "stockfish.h"

// Game-related State data
SpriteRenderer* Renderer;
Chessboard* Board;
StockfishProcess* Stockfish;

std::vector<std::vector<GameObject*>> Pieces(8,std::vector<GameObject*>(8,nullptr));

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height)
{ 

}

Game::~Game()
{
    delete Renderer;
    delete Board;
    delete Stockfish;
}

void Game::Init()
{
    // load shaders
    std::cout << "Loading Shaders" << std::endl;
    ResourceManager::LoadShader("../shaders/sprite_vertex.glsl", "../shaders/sprite_fragment.glsl", nullptr, "sprite");
    // configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    // set render-specific controls
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    Board = new Chessboard();
    Stockfish = new StockfishProcess();
    // load textures
    std::cout << "Loading Textures" << std::endl;
    ResourceManager::LoadTexture("../textures/block.png",false,"block");
    ResourceManager::LoadTexture("../textures/king_w.png", true, "king_white");
    ResourceManager::LoadTexture("../textures/queen_w.png",true, "queen_white");
    ResourceManager::LoadTexture("../textures/rook_w.png",true, "rook_white");
    ResourceManager::LoadTexture("../textures/knight_w.png",true, "knight_white");
    ResourceManager::LoadTexture("../textures/bishop_w.png",true, "bishop_white");
    ResourceManager::LoadTexture("../textures/pawn_w.png",true, "pawn_white");
    ResourceManager::LoadTexture("../textures/king_b.png",true, "king_black");
    ResourceManager::LoadTexture("../textures/queen_b.png",true, "queen_black");
    ResourceManager::LoadTexture("../textures/rook_b.png",true,"rook_black");
    ResourceManager::LoadTexture("../textures/knight_b.png",true,"knight_black");
    ResourceManager::LoadTexture("../textures/bishop_b.png",true,"bishop_black");
    ResourceManager::LoadTexture("../textures/pawn_b.png",true,"pawn_black");
}

void Game::Update(float dt)
{
    // Smooth animation to be implemented here ig ?
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        if (this->mouseX > 0 && this->mouseY > 0 && this->mouseX < Width && this->mouseY < Height){
            float col = mouseX / Width;
            float row =  mouseY / Height;

        }    
    }
}

void Game::Render()
{
    if(this->State == GAME_ACTIVE)
    {
        // draw board
        for (int i=0;i<BOARD_SIZE;++i){
            for (int j=0;j<BOARD_SIZE;++j){
                Renderer->DrawSprite(ResourceManager::GetTexture("block"),
                        glm::vec2(this->Width*(0.1f + SQUARE_SIZE*i),this->Height*(0.1f + SQUARE_SIZE*j)),
                        glm::vec2(this->Width*SQUARE_SIZE,this->Height*SQUARE_SIZE),
                        0.0f, (((i%2)==(j%2))?glm::make_vec3(LIGHT_SQUARE) : glm::make_vec3(DARK_SQUARE)));
            }
        }
        // draw pieces 
    }
}

void Game::ResetGame()
{

}
