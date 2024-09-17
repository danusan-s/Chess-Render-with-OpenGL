#include "game.h"
#include "resource_manager.h"
#include "game_logic.h"
#include "sprite_renderer.h"
#include <iostream>

// Game-related State data
SpriteRenderer* Renderer;
GameLogicHandler* LogicHandler;

Game::Game(unsigned int width, unsigned int height) 
    : State(GAME_ACTIVE), Keys(), Width(width), Height(height) , dragging(-1)
{ 

}

Game::~Game()
{
    delete Renderer;
    delete LogicHandler;
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
    LogicHandler = new GameLogicHandler(!playerIsWhite);
    // load textures
    std::cout << "Loading Textures" << std::endl;
    ResourceManager::LoadTexture("../textures/block.png",false,"block");
    ResourceManager::LoadTexture("../textures/awesomeface.png",true,"face");
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

void Game::Update(float dt){
   if (LogicHandler->blackTurn == playerIsWhite){
       LogicHandler->makeStockfishMove();
   } 
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        if (this->clickX > 0 && this->clickY > 0){
            float scaledX = clickX / Width;
            float scaledY =  clickY / Height;
            int row = static_cast<int>((scaledY - 0.1f)*10);
            int col = static_cast<int>((scaledX - 0.1f)*10);
            if (!playerIsWhite){
                row =  7-row;
                col = 7-col;
            }
            std::cout << row << " " << col << std::endl;
            if (dragging==row*BOARD_SIZE+col){
                clickX = -1;
                clickY = -1;
                dragging = -1;
                return;
            }
            if (row>=0 && col>=0 && row<BOARD_SIZE && col<BOARD_SIZE){
                LogicHandler->clickSquare(row,col);
            }
            if (dragging==-1){
                dragging = row*BOARD_SIZE + col;
            }
            else{
                dragging=-1;
            }
            clickX = -1;
            clickY = -1;
        }    
    }
}

std::string textureOf(Piece piece){
    switch (piece.getType()){
        case Type::King:
            if (piece.getColor()==Color::White)
                return "king_white";
            return "king_black";
        case Type::Queen:
            if (piece.getColor()==Color::White)
                return "queen_white";
            return "queen_black";
        case Type::Knight:
            if (piece.getColor()==Color::White)
                return "knight_white";
            return "knight_black";
        case Type::Bishop:
            if (piece.getColor()==Color::White)
                return "bishop_white";
            return "bishop_black";
        case Type::Rook:
            if (piece.getColor()==Color::White)
                return "rook_white";
            return "rook_black";
        case Type::Pawn:
            if (piece.getColor()==Color::White)
                return "pawn_white";
            return "pawn_black";
        default:
            return "";
    }
    return "";
}

glm::vec3 colorOf(int type){
    switch(type){
        case 1:
            return glm::make_vec3(LIGHT_SQUARE);
        case 2:
            return glm::make_vec3(HIGHLIGHTED_LIGHT_SQUARE);
        case 3:
            return glm::make_vec3(CAPTURE_LIGHT_SQUARE);
        case -1:
            return glm::make_vec3(DARK_SQUARE);
        case -2:
            return glm::make_vec3(HIGHLIGHTED_DARK_SQAURE);
        case -3:
            return glm::make_vec3(CAPTURE_DARK_SQUARE);
        default:
            break;
    }
    return glm::vec3(1.0f);
}

void Game::Render()
{
    if(this->State == GAME_ACTIVE)
    {
        float size = std::min(this->Width,this->Height) * (SQUARE_SIZE);

        for (int i=0;i<BOARD_SIZE;++i){
            for (int j=0;j<BOARD_SIZE;++j){
                int row = playerIsWhite?i:7-i;
                int col = playerIsWhite?j:7-j;
                float squareX = this->Width*(0.1f + SQUARE_SIZE*col);
                float squareY = this->Height*(0.1f + SQUARE_SIZE*row);
                Renderer->DrawSprite(ResourceManager::GetTexture("block"),
                        glm::vec2(squareX,squareY),
                        glm::vec2(size),
                        0.0f, colorOf(LogicHandler->tileColorState[i][j]));
            }
        }

        for (int i=0;i<BOARD_SIZE;++i){
            for (int j=0;j<BOARD_SIZE;++j){
                if (i*8+j==dragging) continue;
                int row = playerIsWhite?i:7-i;
                int col = playerIsWhite?j:7-j;
                float squareX = this->Width*(0.1f + SQUARE_SIZE*col);
                float squareY = this->Height*(0.1f + SQUARE_SIZE*row);
                Piece piece = LogicHandler->Board.getPiece(i,j);
                if (piece.getColor()!=Color::None){
                    std::string texture = textureOf(piece);
                    Renderer->DrawSprite(ResourceManager::GetTexture(texture),
                            glm::vec2(squareX,squareY),
                            glm::vec2(size),
                            0.0f);
                }
            }
        }
        
        // Draw dragged piece last to be on top of every piece
        if (dragging!=-1){
            float squareX = mouseX-(size/2);
            float squareY = mouseY-(size/2);
            Piece piece = LogicHandler->Board.getPiece(dragging/8,dragging%8);
            if (piece.getColor()!=Color::None){
                std::string texture = textureOf(piece);
                Renderer->DrawSprite(ResourceManager::GetTexture(texture),
                        glm::vec2(squareX,squareY),
                        glm::vec2(size),
                        0.0f);
            }
        }
    }
}


void Game::ResetGame()
{
    delete LogicHandler;
    LogicHandler = new GameLogicHandler(!playerIsWhite);
}
