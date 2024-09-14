std::vector<int> currMove = { -1, -1, -1, -1 };
int step = 0;

bool moving = false;

const float ANIM_FRAMES = 20.0;

std::string moves = "position startpos moves";

struct WindowData {
    Chessboard* chessboard;
    StockfishProcess* stockfish;
    std::vector<int> move;
    std::vector<float>& colors;
    bool black;
};


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        windowData->chessboard->setPromotionType('q');
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS){
        windowData->chessboard->setPromotionType('r');
    }
    else if (key == GLFW_KEY_B && action == GLFW_PRESS) {
        windowData->chessboard->setPromotionType('b');
    }
    else if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        windowData->chessboard->setPromotionType('n');
    }
}

void updateSquare(int row, int col, std::vector<float>& colors,const float light [3], const float dark [3]) {
    bool isWhite = (row + col) % 2 == 0;
    int startIndex = 18*(8 * row + col);
    if (!playerIsWhite) {
        startIndex = 1152 - 18 * (8 * row + col + 1);
    }
    for (int k = 0; k < 18; ++k) {
        if (isWhite) {
            colors[startIndex + k] = light[k % 3];
        }
        else {
            colors[startIndex + k] = dark[k % 3];
        }
    }
}

void clearHighlights(WindowData* windowData) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (windowData->chessboard->isValidMove(windowData->move[0], windowData->move[1], i, j)!=M_Result::INVALID) {
                updateSquare(i, j, windowData->colors, lightSquare, darkSquare);
            }
        }
    }
    updateSquare(windowData->move[0], windowData->move[1], windowData->colors, lightSquare, darkSquare);
}

void setHighlights(WindowData* windowData) {
    updateSquare(windowData->move[0], windowData->move[1], windowData->colors, highlightedLightSquare, highlightedDarkSquare);
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            M_Result result = windowData->chessboard->isValidMove(windowData->move[0], windowData->move[1], i, j);
            if (result != M_Result::INVALID) {
                if (windowData->chessboard->getPiece(i, j).getColor() != Color::None || result==M_Result::EN_PASSANT) {
                    updateSquare(i, j, windowData->colors, captureLightSquare, captureDarkSquare);
                }
                else {
                    updateSquare(i, j, windowData->colors, highlightedLightSquare, highlightedDarkSquare);
                }
            }
        }
    }
}

std::string cordToStringMove(std::vector<int>& move) {
    std::string res = "";
    res += char(move[1] + 'a'); 
    res += char(8 - move[0] + '0');
    res += char(move[3] + 'a'); 
    res+=char(8 - move[2] + '0');
    return res;
}

std::vector<int> stringToCordMove(std::string& move) {
    return { 8 - move[1] + '0',move[0] - 'a',8 - move[3] + '0',move[2] - 'a' };
}

std::string getBestMove(std::string& result,WindowData* windowData) {
    size_t pos = result.rfind("bestmove ");
    while (pos==std::string::npos) {
        result = windowData->stockfish->readOutput();
        pos = result.rfind("bestmove ");
    }
    if (pos+5>=result.size()) return result.substr(pos+9,4);
    std::string bestMove = result.substr(pos+9,5);
    std::cout << bestMove << std::endl;
    return bestMove;
}

void makeStockFishMove(WindowData* windowData) {
    if (moves == "position startpos moves") windowData->stockfish->sendCommand("position startpos");
    else windowData->stockfish->sendCommand(moves);
    windowData->stockfish->sendCommand("go movetime 200");
    std::string output = windowData->stockfish->readOutput();
    std::string bestMove = getBestMove(output,windowData);
    std::vector<int> move = stringToCordMove(bestMove);
    if (bestMove[4]!=' ') {
        windowData->chessboard->setPromotionType(bestMove[4]);
        moves+= " "+bestMove;
    }
    else{
        moves+= " "+bestMove.substr(0,4); 
    }
    windowData->chessboard->movePiece(move, windowData->black);
    currMove = move;
    moving = true;
    windowData->black = !windowData->black;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && !moving) {
        double xPos, yPos;
        glfwGetCursorPos(window, &xPos, &yPos);

        int width, height;
        glfwGetWindowSize(window, &width, &height);

        xPos /= static_cast<double>(width);
        yPos /= static_cast<double>(height);

        xPos -= 0.1;
        yPos -= 0.1;

        int row = yPos < 0 ? -1 : yPos * 10;
        int col = xPos < 0 ? -1 : xPos * 10;

        if (!playerIsWhite) {
            row = 7 - row;
            col = 7 - col;
        }

        std::cout << row << " " << col << std::endl;

        WindowData* windowData = static_cast<WindowData*>(glfwGetWindowUserPointer(window));
        if (windowData && windowData->black!=playerIsWhite) {
            if (row >= 0 && row < 8 && col >= 0 && col < 8) {
                if (windowData->move.empty()) {
                    if (windowData->chessboard->checkValidSource(row, col, !playerIsWhite)) {
                        windowData->move.push_back(row); windowData->move.push_back(col);
                        setHighlights(windowData);
                    }
                }
                else {
                    clearHighlights(windowData);
                    windowData->move.push_back(row); windowData->move.push_back(col);
                    if (windowData->chessboard->movePiece(windowData->move, !playerIsWhite)) {
                        moves += " " + cordToStringMove(windowData->move);
                        currMove = windowData->move;
                        moving = true;
                        windowData->black = !windowData->black;
                        windowData->move.clear();
                    }
                    else if ((row != windowData->move[0] || col != windowData->move[1]) && windowData->chessboard->checkValidSource(row, col, !playerIsWhite)) {
                        windowData->move.clear();
                        windowData->move.push_back(row); windowData->move.push_back(col);
                        setHighlights(windowData);
                    }
                    else {
                        windowData->move.clear();
                    }
                }
            }
            else {
                if (!windowData->move.empty()) {
                    clearHighlights(windowData);
                }
                windowData->move.clear();
            }
            
        }
        else {
            std::cout << "Window Data not initialized" << std::endl;
        }
    }
}

void generateChessboard(std::vector<float>& vertices, std::vector<float>& colors) {

    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            float x = j * squareSize + 0.1f;
            float y = 0.8f - i * squareSize;

            // Define vertices for two triangles making up a square
            vertices.insert(vertices.end(), {
                x, y,
                x + squareSize, y,
                x, y + squareSize,
                x + squareSize, y,
                x + squareSize, y + squareSize,
                x, y + squareSize
                });

            // Define colors for the square
            bool isWhite = (i + j) % 2 == 0;
            for (int k = 0; k < 6; ++k) {
                if (isWhite) {
                    colors.insert(colors.end(), lightSquare,lightSquare+3);
                }
                else {
                    colors.insert(colors.end(), darkSquare, darkSquare+3);
                }
            }
        }
    }
}

void renderPiece(int row, int col,Chessboard& Game, std::vector<unsigned int>& textures,Shader& texShader) {
    float x, y;
    if (row == currMove[2] && col == currMove[3]) {
        if (playerIsWhite) {
            x = currMove[1] * squareSize + 0.1f + (step * squareSize * (currMove[3] - currMove[1]) / ANIM_FRAMES);
            y = 0.8f - currMove[0] * squareSize - (step * squareSize * (currMove[2] - currMove[0]) / ANIM_FRAMES);
        }
        else {
            x = 0.8f - currMove[1] * squareSize - (step * squareSize * (currMove[3] - currMove[1]) / ANIM_FRAMES);
            y = currMove[0] * squareSize + 0.1f + (step * squareSize * (currMove[2] - currMove[0]) / ANIM_FRAMES);
        }
        ++step;
        if (step == ANIM_FRAMES + 1) {
            currMove = { -1,-1,-1,-1 };
            step = 0;
            moving = false;
        }
    }
    else {
        if (playerIsWhite) {
            x = col * squareSize + 0.1f;
            y = 0.8f - row * squareSize;
        }
        else {
            x = 0.8f - col * squareSize;
            y = row * squareSize + 0.1f;
        }
    }

    Piece curr = Game.getPiece(row, col);
    if (curr.getType() == Type::None) return;
    int index = 0;
    switch (curr.getType()) {
    case Type::King:
        index = 0;
        break;
    case Type::Queen:
        index = 1;
        break;
    case Type::Rook:
        index = 2;
        break;
    case Type::Knight:
        index = 3;
        break;
    case Type::Bishop:
        index = 4;
        break;
    case Type::Pawn:
        index = 5;
        break;
    }
    if (curr.getColor() == Color::Black) index += 6;
    unsigned int textureId = textures[index];
    float vertices[] = {
        x, y,
        x + squareSize, y,
        x, y + squareSize,
        x + squareSize, y,
        x + squareSize, y + squareSize,
        x, y + squareSize
    };
    float texCoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
    };
    
    unsigned int VAO, VBO[2];
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    // Bind vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Bind texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // Use the texture shader program
    texShader.use();

    glBindTexture(GL_TEXTURE_2D, textureId);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBO);
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // Flip loading of images vertically
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, STBI_rgb_alpha);
    if (data) {

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        // Set texture parameters
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Generate Mipmap
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }
    else {
        std::cout << "Failed to load texture: " << path <<" "<<stbi_failure_reason()<< std::endl;
    }

    return textureID;
}

// Load the piece textures
void getTextures(std::vector<uint32_t>& textures) {
    textures[0] = loadTexture("../images/king_w.png");
    textures[1] = loadTexture("../images/queen_w.png");
    textures[2] = loadTexture("../images/rook_w.png");
    textures[3] = loadTexture("../images/knight_w.png");
    textures[4] = loadTexture("../images/bishop_w.png");
    textures[5] = loadTexture("../images/pawn_w.png");
    textures[6] = loadTexture("../images/king_b.png");
    textures[7] = loadTexture("../images/queen_b.png");
    textures[8] = loadTexture("../images/rook_b.png");
    textures[9] = loadTexture("../images/knight_b.png");
    textures[10] = loadTexture("../images/bishop_b.png");
    textures[11] = loadTexture("../images/pawn_b.png");
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version to 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(DEFAULT_SIZE, DEFAULT_SIZE, "Chessboard", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Set viewport
    glViewport(0, 0, DEFAULT_SIZE, DEFAULT_SIZE);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    std::vector<float> vertices;
    std::vector<float> colors;
    std::vector<uint32_t> textures(12);
    generateChessboard(vertices, colors);

    // Create VBOs and VAO
    unsigned int VBO[2], VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    Chessboard Game;
    Shader OurShader("../shaders/vshader.glsl", "../shaders/fshader.glsl");
    Shader TexShader("../shaders/tvshader.glsl", "../shaders/tfshader.glsl");

    // Insert stockfish executable path here
    StockfishProcess stockfish("../stockfish/stockfish-ubuntu-x86-64-avx2");
    
    stockfish.readOutput();
    stockfish.sendCommand("uci");
    std::cout << "Output from Stockfish:\n" << stockfish.readOutput() << std::endl;
    stockfish.sendCommand("setoption name Skill Level value "+std::to_string(BOT_LVL));
    stockfish.sendCommand("isready");
    std::cout << "Output from Stockfish:\n" << stockfish.readOutput() << std::endl;
    

    getTextures(textures);

    WindowData windowData = { &Game,&stockfish,{},colors,false };
    glfwSetWindowUserPointer(window, &windowData);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the color buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render the chessboard
        OurShader.use();

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
        glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size()/2);
        glBindVertexArray(0);

        for (int row = 0; row < boardSize; ++row) {
            for (int col = 0; col < boardSize; ++col) {
                renderPiece(row, col, Game,textures,TexShader);
            }
        }


        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();

        if (windowData.black==playerIsWhite && !moving) {
            makeStockFishMove(&windowData);
        }

    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

