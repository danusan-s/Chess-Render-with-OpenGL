#include "game.h"
#include "resource_manager.h"
#include "game_object.h"
#include "chess.h"
#include "stockfish.h"

// Game-related State data
SpriteRenderer* Renderer;
Chessboard* Board;
StockfishProcess* Stockfish;

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
    GLint maxTextureSize;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    std::cout << "Maximum texture size: " << maxTextureSize << std::endl;
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

void Game::Render()
{
    if(this->State == GAME_ACTIVE)
    {
        for (int i=0;i<BOARD_SIZE;++i){
            for (int j=0;j<BOARD_SIZE;++j){
                float squareX = this->Width*(0.1f + SQUARE_SIZE*i);
                float squareY = this->Height*(0.1f + SQUARE_SIZE*j);
                float size = std::min(this->Width,this->Height) * SQUARE_SIZE;
                Renderer->DrawSprite(ResourceManager::GetTexture("block"),
                        glm::vec2(squareX,squareY),
                        glm::vec2(size),
                        0.0f, (((i%2)==(j%2))?glm::make_vec3(LIGHT_SQUARE) : glm::make_vec3(DARK_SQUARE)));
            }
        }

        for (int i=0;i<BOARD_SIZE;++i){
            for (int j=0;j<BOARD_SIZE;++j){
                float squareX = this->Width*(0.1f + SQUARE_SIZE*i);
                float squareY = this->Height*(0.1f + SQUARE_SIZE*j);
                float size = std::min(this->Width,this->Height) * (SQUARE_SIZE);
                Piece piece = Board->getPiece(i,j);
                if (piece.getColor()!=Color::None){
                    std::string texture = textureOf(piece);
                    Renderer->DrawSprite(ResourceManager::GetTexture(texture),
                            glm::vec2(squareY,squareX),
                            glm::vec2(size),
                            0.0f);
                }
            }
        }
    }
}



void Game::ResetGame()
{

}
