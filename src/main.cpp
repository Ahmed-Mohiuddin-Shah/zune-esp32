#include <zyngine.h>

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------
#define SQUARE_SIZE 15

#define GRID_HORIZONTAL_SIZE 12
#define GRID_VERTICAL_SIZE 20

#define LATERAL_SPEED 10
#define TURNING_SPEED 12
#define FAST_FALL_AWAIT_COUNTER 12

#define FADING_TIME 10

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GridSquare
{
    EMPTY,
    MOVING,
    FULL,
    BLOCK,
    FADING
} GridSquare;

//------------------------------------------------------------------------------------
// Global Variables Declaration
//------------------------------------------------------------------------------------
static const int screenWidth = 320;
static const int screenHeight = 480;

static bool gameOver = false;
static bool pauseGame = false;

// Matrices
static GridSquare grid[GRID_HORIZONTAL_SIZE][GRID_VERTICAL_SIZE];
static GridSquare piece[4][4];
static GridSquare incomingPiece[4][4];

// Theese variables keep track of the active piece position
static int piecePositionX = 0;
static int piecePositionY = 0;

// Game parameters
static uint16_t fadingColor;
// static int fallingSpeed;           // In frames

static bool beginPlay = true; // This var is only true at the begining of the game, used for the first matrix creations
static bool pieceActive = false;
static bool detection = false;
static bool lineToDelete = false;

// Statistics
static int level = 1;
static int lines = 0;

// Counters
static int gravityMovementCounter = 0;
static int lateralMovementCounter = 0;
static int turnMovementCounter = 0;
static int fastFallMovementCounter = 0;

static int fadeLineCounter = 0;

// Based on level
static int gravitySpeed = 4;

class Tetris : public Zyngine
{
private:
    ZynTexture pauseScreen;
    ZynTexture gameOverScreen;
    ZynTexture backGround;

    void initGame(void)
    {
        // Initialize game statistics
        level = 1;
        lines = 0;

        fadingColor = ZYN_GRAY;

        piecePositionX = 0;
        piecePositionY = 0;

        pauseGame = false;

        beginPlay = true;
        pieceActive = false;
        detection = false;
        lineToDelete = false;

        // Counters
        gravityMovementCounter = 0;
        lateralMovementCounter = 0;
        turnMovementCounter = 0;
        fastFallMovementCounter = 0;

        fadeLineCounter = 0;
        gravitySpeed = 3;

        // Initialize grid matrices
        for (int i = 0; i < GRID_HORIZONTAL_SIZE; i++)
        {
            for (int j = 0; j < GRID_VERTICAL_SIZE; j++)
            {
                if ((j == GRID_VERTICAL_SIZE - 1) || (i == 0) || (i == GRID_HORIZONTAL_SIZE - 1))
                    grid[i][j] = BLOCK;
                else
                    grid[i][j] = EMPTY;
            }
        }

        // Initialize incoming piece matrices
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                incomingPiece[i][j] = EMPTY;
            }
        }
    }

    static bool Createpiece()
    {
        piecePositionX = (int)((GRID_HORIZONTAL_SIZE - 4) / 2);
        piecePositionY = 0;

        // If the game is starting and you are going to create the first piece, we create an extra one
        if (beginPlay)
        {
            GetRandompiece();
            beginPlay = false;
        }

        // We assign the incoming piece to the actual piece
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                piece[i][j] = incomingPiece[i][j];
            }
        }

        // We assign a random piece to the incoming one
        GetRandompiece();

        // Assign the piece to the grid
        for (int i = piecePositionX; i < piecePositionX + 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (piece[i - (int)piecePositionX][j] == MOVING)
                    grid[i][j] = MOVING;
            }
        }

        return true;
    }

    static void GetRandompiece()
    {
        int random = rand() % 6;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                incomingPiece[i][j] = EMPTY;
            }
        }

        switch (random)
        {
        case 0:
        {
            incomingPiece[1][1] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][2] = MOVING;
        }
        break; // Cube
        case 1:
        {
            incomingPiece[1][0] = MOVING;
            incomingPiece[1][1] = MOVING;
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][2] = MOVING;
        }
        break; // L
        case 2:
        {
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][0] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[2][2] = MOVING;
        }
        break; // L inversa
        case 3:
        {
            incomingPiece[0][1] = MOVING;
            incomingPiece[1][1] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[3][1] = MOVING;
        }
        break; // Recta
        case 4:
        {
            incomingPiece[1][0] = MOVING;
            incomingPiece[1][1] = MOVING;
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][1] = MOVING;
        }
        break; // Creu tallada
        case 5:
        {
            incomingPiece[1][1] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[2][2] = MOVING;
            incomingPiece[3][2] = MOVING;
        }
        break; // S
        case 6:
        {
            incomingPiece[1][2] = MOVING;
            incomingPiece[2][2] = MOVING;
            incomingPiece[2][1] = MOVING;
            incomingPiece[3][1] = MOVING;
        }
        break; // S inversa
        }
    }

    static void ResolveFallingMovement(bool *detection, bool *pieceActive)
    {
        // If we finished moving this piece, we stop it
        if (*detection)
        {
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
            {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                {
                    if (grid[i][j] == MOVING)
                    {
                        grid[i][j] = FULL;
                        *detection = false;
                        *pieceActive = false;
                    }
                }
            }
        }
        else // We move down the piece
        {
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
            {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                {
                    if (grid[i][j] == MOVING)
                    {
                        grid[i][j + 1] = MOVING;
                        grid[i][j] = EMPTY;
                    }
                }
            }

            piecePositionY++;
        }
    }

    bool ResolveLateralMovement()
    {
        bool collision = false;

        // Piece movement
        if (gui.encoderSteps < 0) // Move left
        {
            // Check if is possible to move to left
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
            {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                {
                    if (grid[i][j] == MOVING)
                    {
                        // Check if we are touching the left wall or we have a full square at the left
                        if ((i - 1 == 0) || (grid[i - 1][j] == FULL))
                            collision = true;
                    }
                }
            }

            // If able, move left
            if (!collision)
            {
                for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
                {
                    for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++) // We check the matrix from left to right
                    {
                        // Move everything to the left
                        if (grid[i][j] == MOVING)
                        {
                            grid[i - 1][j] = MOVING;
                            grid[i][j] = EMPTY;
                        }
                    }
                }

                piecePositionX--;
            }
        }
        else if (gui.encoderSteps > 0) // Move right
        {
            // Check if is possible to move to right
            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
            {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                {
                    if (grid[i][j] == MOVING)
                    {
                        // Check if we are touching the right wall or we have a full square at the right
                        if ((i + 1 == GRID_HORIZONTAL_SIZE - 1) || (grid[i + 1][j] == FULL))
                        {
                            collision = true;
                        }
                    }
                }
            }

            // If able move right
            if (!collision)
            {
                for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
                {
                    for (int i = GRID_HORIZONTAL_SIZE - 1; i >= 1; i--) // We check the matrix from right to left
                    {
                        // Move everything to the right
                        if (grid[i][j] == MOVING)
                        {
                            grid[i + 1][j] = MOVING;
                            grid[i][j] = EMPTY;
                        }
                    }
                }

                piecePositionX++;
            }
        }

        return collision;
    }

    bool ResolveTurnMovement()
    {
        // Input for turning the piece
        if (!digitalRead(ENCODER_BUTTON))
        {
            GridSquare aux;
            bool checker = false;

            // Check all turning possibilities
            if ((grid[piecePositionX + 3][piecePositionY] == MOVING) &&
                (grid[piecePositionX][piecePositionY] != EMPTY) &&
                (grid[piecePositionX][piecePositionY] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 3][piecePositionY + 3] == MOVING) &&
                (grid[piecePositionX + 3][piecePositionY] != EMPTY) &&
                (grid[piecePositionX + 3][piecePositionY] != MOVING))
                checker = true;

            if ((grid[piecePositionX][piecePositionY + 3] == MOVING) &&
                (grid[piecePositionX + 3][piecePositionY + 3] != EMPTY) &&
                (grid[piecePositionX + 3][piecePositionY + 3] != MOVING))
                checker = true;

            if ((grid[piecePositionX][piecePositionY] == MOVING) &&
                (grid[piecePositionX][piecePositionY + 3] != EMPTY) &&
                (grid[piecePositionX][piecePositionY + 3] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 1][piecePositionY] == MOVING) &&
                (grid[piecePositionX][piecePositionY + 2] != EMPTY) &&
                (grid[piecePositionX][piecePositionY + 2] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 3][piecePositionY + 1] == MOVING) &&
                (grid[piecePositionX + 1][piecePositionY] != EMPTY) &&
                (grid[piecePositionX + 1][piecePositionY] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 2][piecePositionY + 3] == MOVING) &&
                (grid[piecePositionX + 3][piecePositionY + 1] != EMPTY) &&
                (grid[piecePositionX + 3][piecePositionY + 1] != MOVING))
                checker = true;

            if ((grid[piecePositionX][piecePositionY + 2] == MOVING) &&
                (grid[piecePositionX + 2][piecePositionY + 3] != EMPTY) &&
                (grid[piecePositionX + 2][piecePositionY + 3] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 2][piecePositionY] == MOVING) &&
                (grid[piecePositionX][piecePositionY + 1] != EMPTY) &&
                (grid[piecePositionX][piecePositionY + 1] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 3][piecePositionY + 2] == MOVING) &&
                (grid[piecePositionX + 2][piecePositionY] != EMPTY) &&
                (grid[piecePositionX + 2][piecePositionY] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 1][piecePositionY + 3] == MOVING) &&
                (grid[piecePositionX + 3][piecePositionY + 2] != EMPTY) &&
                (grid[piecePositionX + 3][piecePositionY + 2] != MOVING))
                checker = true;

            if ((grid[piecePositionX][piecePositionY + 1] == MOVING) &&
                (grid[piecePositionX + 1][piecePositionY + 3] != EMPTY) &&
                (grid[piecePositionX + 1][piecePositionY + 3] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 1][piecePositionY + 1] == MOVING) &&
                (grid[piecePositionX + 1][piecePositionY + 2] != EMPTY) &&
                (grid[piecePositionX + 1][piecePositionY + 2] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 2][piecePositionY + 1] == MOVING) &&
                (grid[piecePositionX + 1][piecePositionY + 1] != EMPTY) &&
                (grid[piecePositionX + 1][piecePositionY + 1] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 2][piecePositionY + 2] == MOVING) &&
                (grid[piecePositionX + 2][piecePositionY + 1] != EMPTY) &&
                (grid[piecePositionX + 2][piecePositionY + 1] != MOVING))
                checker = true;

            if ((grid[piecePositionX + 1][piecePositionY + 2] == MOVING) &&
                (grid[piecePositionX + 2][piecePositionY + 2] != EMPTY) &&
                (grid[piecePositionX + 2][piecePositionY + 2] != MOVING))
                checker = true;

            if (!checker)
            {
                aux = piece[0][0];
                piece[0][0] = piece[3][0];
                piece[3][0] = piece[3][3];
                piece[3][3] = piece[0][3];
                piece[0][3] = aux;

                aux = piece[1][0];
                piece[1][0] = piece[3][1];
                piece[3][1] = piece[2][3];
                piece[2][3] = piece[0][2];
                piece[0][2] = aux;

                aux = piece[2][0];
                piece[2][0] = piece[3][2];
                piece[3][2] = piece[1][3];
                piece[1][3] = piece[0][1];
                piece[0][1] = aux;

                aux = piece[1][1];
                piece[1][1] = piece[2][1];
                piece[2][1] = piece[2][2];
                piece[2][2] = piece[1][2];
                piece[1][2] = aux;
            }

            for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
            {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                {
                    if (grid[i][j] == MOVING)
                    {
                        grid[i][j] = EMPTY;
                    }
                }
            }

            for (int i = piecePositionX; i < piecePositionX + 4; i++)
            {
                for (int j = piecePositionY; j < piecePositionY + 4; j++)
                {
                    if (piece[i - piecePositionX][j - piecePositionY] == MOVING)
                    {
                        grid[i][j] = MOVING;
                    }
                }
            }

            return true;
        }

        return false;
    }

    void CheckDetection(bool *detection)
    {
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                if ((grid[i][j] == MOVING) && ((grid[i][j + 1] == FULL) || (grid[i][j + 1] == BLOCK)))
                    *detection = true;
            }
        }
    }

    void CheckCompletion(bool *lineToDelete)
    {
        int calculator = 0;

        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            calculator = 0;
            for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
            {
                // Count each square of the line
                if (grid[i][j] == FULL)
                {
                    calculator++;
                }

                // Check if we completed the whole line
                if (calculator == GRID_HORIZONTAL_SIZE - 2)
                {
                    *lineToDelete = true;
                    calculator = 0;
                    // points++;

                    // Mark the completed line
                    for (int z = 1; z < GRID_HORIZONTAL_SIZE - 1; z++)
                    {
                        grid[z][j] = FADING;
                    }
                }
            }
        }
    }

    int DeleteCompleteLines()
    {
        int deletedLines = 0;

        // Erase the completed line
        for (int j = GRID_VERTICAL_SIZE - 2; j >= 0; j--)
        {
            while (grid[1][j] == FADING)
            {
                for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                {
                    grid[i][j] = EMPTY;
                }

                for (int j2 = j - 1; j2 >= 0; j2--)
                {
                    for (int i2 = 1; i2 < GRID_HORIZONTAL_SIZE - 1; i2++)
                    {
                        if (grid[i2][j2] == FULL)
                        {
                            grid[i2][j2 + 1] = FULL;
                            grid[i2][j2] = EMPTY;
                        }
                        else if (grid[i2][j2] == FADING)
                        {
                            grid[i2][j2 + 1] = FADING;
                            grid[i2][j2] = EMPTY;
                        }
                    }
                }

                deletedLines++;
            }
        }

        return deletedLines;
    }

    // Update game(one frame)
    void updateGame(void)
    {
        if (!gameOver)
        {
            if (!digitalRead(POWER_BUTTON))
                pauseGame = !pauseGame;

            if (!pauseGame)
            {
                if (!lineToDelete)
                {
                    if (!pieceActive)
                    {
                        // Get another piece
                        pieceActive = Createpiece();

                        // We leave a little time before starting the fast falling down
                        fastFallMovementCounter = 0;
                    }
                    else // Piece falling
                    {
                        // Counters update
                        fastFallMovementCounter++;
                        gravityMovementCounter++;
                        lateralMovementCounter++;
                        turnMovementCounter++;

                        // We make sure to move if we've moved the knob
                        if (gui.encoderSteps != 0)
                            lateralMovementCounter = LATERAL_SPEED;
                        if (digitalRead(ENCODER_BUTTON))
                            turnMovementCounter = TURNING_SPEED;

                        // Fall down
                        if (!digitalRead(HOME_BUTTON) && (fastFallMovementCounter >= FAST_FALL_AWAIT_COUNTER))
                        {
                            // We make sure the piece is going to fall this frame
                            gravityMovementCounter += gravitySpeed;
                        }

                        if (gravityMovementCounter >= gravitySpeed)
                        {
                            // Basic falling movement
                            CheckDetection(&detection);

                            // Check if the piece has collided with another piece or with the boundings
                            ResolveFallingMovement(&detection, &pieceActive);

                            // Check if we fullfilled a line and if so, erase the line and pull down the the lines above
                            CheckCompletion(&lineToDelete);

                            gravityMovementCounter = 0;
                        }

                        // Move laterally at player's will
                        if (lateralMovementCounter >= LATERAL_SPEED)
                        {
                            // Update the lateral movement and if success, reset the lateral counter
                            if (!ResolveLateralMovement())
                                lateralMovementCounter = 0;
                        }
                            // Turn the piece at player's will
                            if (turnMovementCounter >= TURNING_SPEED)
                        {
                            // Update the turning movement and reset the turning counter
                            if (ResolveTurnMovement())
                                turnMovementCounter = 0;
                        }
                    }

                    // Game over logic
                    for (int j = 0; j < 2; j++)
                    {
                        for (int i = 1; i < GRID_HORIZONTAL_SIZE - 1; i++)
                        {
                            if (grid[i][j] == FULL)
                            {
                                gameOver = true;
                            }
                        }
                    }
                }
                else
                {
                    // Animation when deleting lines
                    fadeLineCounter++;

                    if (fadeLineCounter % 8 < 4)
                        fadingColor = ZYN_MAROON;
                    else
                        fadingColor = ZYN_GRAY;

                    if (fadeLineCounter >= FADING_TIME)
                    {
                        int deletedLines = 0;
                        deletedLines = DeleteCompleteLines();
                        fadeLineCounter = 0;
                        lineToDelete = false;

                        lines += deletedLines;
                    }
                }
            }
        }
        else
        {
            if (!digitalRead(POWER_BUTTON))
            {
                initGame();
                gameOver = false;
            }
        }
    }

    // Draw game (one frame)
    void drawGame(void)
    {

        renderer->clear(ZYN_BLACK);

        if (!gameOver)
        {
            // Draw gameplay area

            renderer->drawTextureToBox(&backGround, 0, 0, screenWidth, screenHeight);

            ZVec2 offset;
            offset.x = (GRID_HORIZONTAL_SIZE * SQUARE_SIZE / 2) - 50;
            offset.y = screenHeight / 2 - ((GRID_VERTICAL_SIZE - 1) * SQUARE_SIZE / 2) + SQUARE_SIZE * 2;

            offset.y -= 50; // NOTE: Harcoded position!

            int controller = offset.x;

            for (int j = 0; j < GRID_VERTICAL_SIZE; j++)
            {
                for (int i = 0; i < GRID_HORIZONTAL_SIZE; i++)
                {
                    // Draw each square of the grid
                    if (grid[i][j] == EMPTY)
                    {
                        renderer->drawLine(offset.x, offset.y, offset.x + SQUARE_SIZE, offset.y, ZYN_LIGHTGRAY);
                        renderer->drawLine(offset.x, offset.y, offset.x, offset.y + SQUARE_SIZE, ZYN_LIGHTGRAY);
                        renderer->drawLine(offset.x + SQUARE_SIZE, offset.y, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, ZYN_LIGHTGRAY);
                        renderer->drawLine(offset.x, offset.y + SQUARE_SIZE, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, ZYN_LIGHTGRAY);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (grid[i][j] == FULL)
                    {
                        renderer->fillRect(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, ZYN_GRAY);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (grid[i][j] == MOVING)
                    {
                        renderer->fillRect(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, ZYN_DARKBLUE);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (grid[i][j] == BLOCK)
                    {
                        renderer->fillRect(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, ZYN_LIGHTGRAY);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (grid[i][j] == FADING)
                    {
                        renderer->fillRect(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, fadingColor);
                        offset.x += SQUARE_SIZE;
                    }
                }

                offset.x = controller;
                offset.y += SQUARE_SIZE;
            }

            // Draw incoming piece (hardcoded)
            offset.x = 230;
            offset.y = 75;

            int controler = offset.x;

            for (int j = 0; j < 4; j++)
            {
                for (int i = 0; i < 4; i++)
                {
                    if (incomingPiece[i][j] == EMPTY)
                    {
                        renderer->drawLine(offset.x, offset.y, offset.x + SQUARE_SIZE, offset.y, ZYN_LIGHTGRAY);
                        renderer->drawLine(offset.x, offset.y, offset.x, offset.y + SQUARE_SIZE, ZYN_LIGHTGRAY);
                        renderer->drawLine(offset.x + SQUARE_SIZE, offset.y, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, ZYN_LIGHTGRAY);
                        renderer->drawLine(offset.x, offset.y + SQUARE_SIZE, offset.x + SQUARE_SIZE, offset.y + SQUARE_SIZE, ZYN_LIGHTGRAY);
                        offset.x += SQUARE_SIZE;
                    }
                    else if (incomingPiece[i][j] == MOVING)
                    {
                        renderer->fillRect(offset.x, offset.y, SQUARE_SIZE, SQUARE_SIZE, ZYN_GRAY);
                        offset.x += SQUARE_SIZE;
                    }
                }

                offset.x = controler;
                offset.y += SQUARE_SIZE;
            }

            if (pauseGame)
                renderer->drawTextureToBox(&pauseScreen, 0, 0, screenWidth, screenHeight);
        }
        else
            renderer->drawTextureToBox(&gameOverScreen, 0, 0, screenWidth, screenHeight);
    }

    void onUserCreate() override
    {
        pauseScreen.loadFromFile("/resources/optimized_assets/wallpapers/pause.zyntex");
        gameOverScreen.loadFromFile("/resources/optimized_assets/wallpapers/game_over.zyntex");
        backGround.loadFromFile("/resources/optimized_assets/wallpapers/Bingo Melon Wallpaper (Dark).zyntex");
        initGame();
    }
    void onUserUpdate(float deltaTime) override
    {
        updateGame();
        drawGame();
    }
};

Tetris engine;

void setup()
{
    engine.initialize(320, 480, 60);
}

void loop()
{
    engine.run();
}

#ifdef ZYNGINE_NATIVE_RAYLIB
int main()
{
    setup();
    loop();
}
#endif