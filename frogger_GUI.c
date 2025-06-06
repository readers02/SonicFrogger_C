#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>

#define WIDTH 64
#define HEIGHT 10
#define TILE_SIZE 20
#define WINDOW_WIDTH (WIDTH * TILE_SIZE)
#define WINDOW_HEIGHT (HEIGHT * TILE_SIZE)
#define FROG_SIZE (2 * TILE_SIZE)
#define SPRITE_FRAME_COUNT 4
#define SPRITE_FRAME_WIDTH 20

int frogX, frogY;
char lanes[HEIGHT][WIDTH + 1] = {
    "xxx..xxx..xxx..xxxxxxxxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxx..xxxxx", // lane0
    "...xxxx..xxxxxx.......xxxx.....xx...xxxx.....xxxxxx...xxxxx.....", // lane1
    "....xxxx.....xxxx.....xxxx.......xxxxxxx.....xx....xxxxxx.......", // lane2
    "..xxx.....xxx.....xxx.....xxx...xxx....xx....xxxx....xx......xx.", // lane3
    "................................................................", // lane4
    "....xxxx.......xxxx.........xxxx.......xxxx......xxxx....xxxx...", // lane5
    ".....xx...xx...xx......xx....xx.......xx..xx.xx......xx.......xx", // lane6
    "..xxx.....xx......xxxx..xx......xxxx......xxxx.......xxx...xxx..", // lane7
    "..xx.....xx.......xx.....xx.....xx..xx.xx........xx....xx.......", // lane8
    "................................................................"  // lane9
};
int speeds[HEIGHT] = {0, -2, +1, -1, 0, +2, -1, -1, +1, 0};

//SDL variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* frogTexture = NULL;
SDL_Texture* obstacleLeftTexture = NULL;
SDL_Texture* obstacleRightTexture = NULL;
SDL_Texture* obstacleStationaryTexture = NULL;
SDL_Texture* backgroundTexture = NULL;
TTF_Font* font = NULL;
Mix_Music* backgroundMusic = NULL;
Mix_Chunk* moveSound = NULL;
Mix_Chunk* hitSound = NULL;
Mix_Chunk* victorySound = NULL;

//Function to draw text on the screen
void drawText(const char* message, int x, int y);

//Function to initialize the game state
void initGame() {
    frogX = WIDTH / 2;
    frogY = HEIGHT - 1;
}

//Function to update the hazard positions in the lanes
void updateLane(int lane) {
    int speed = speeds[lane];
    if (speed != 0) {
        char newLane[WIDTH + 1];
        for (int i = 0; i < WIDTH; i++) {
            int newIndex = (i + speed + WIDTH) % WIDTH;
            newLane[newIndex] = lanes[lane][i];
        }
        for (int i = 0; i < WIDTH; i++) {
            lanes[lane][i] = newLane[i];
        }
    }
}

//Function to draw the game screen
void draw() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    static int frame = 0;
    SDL_Rect destRect;
    SDL_Rect srcRect;
    srcRect.y = 0;
    srcRect.w = SPRITE_FRAME_WIDTH;
    srcRect.h = TILE_SIZE;

    //Draw the background
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            destRect.x = x * TILE_SIZE;
            destRect.y = y * TILE_SIZE;
            destRect.w = TILE_SIZE;
            destRect.h = TILE_SIZE;
            SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
        }
    }

    //Draw obstacles and background at each array index
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            destRect.x = x * TILE_SIZE;
            destRect.y = y * TILE_SIZE;
            destRect.w = TILE_SIZE;
            destRect.h = TILE_SIZE;

            if (lanes[y][x] == 'x') {
                if (speeds[y] < 0) {
                    srcRect.x = (SPRITE_FRAME_COUNT - 1 - (frame % SPRITE_FRAME_COUNT)) * SPRITE_FRAME_WIDTH;
                    SDL_RenderCopy(renderer, obstacleLeftTexture, &srcRect, &destRect);
                } else if (speeds[y] > 0) {
                    srcRect.x = (frame % SPRITE_FRAME_COUNT) * SPRITE_FRAME_WIDTH;
                    SDL_RenderCopy(renderer, obstacleRightTexture, &srcRect, &destRect);
                } else {
                    SDL_RenderCopy(renderer, obstacleStationaryTexture, NULL, &destRect);
                }
            } else {
                SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);
            }
        }
    }

    // Draw the players's position
    destRect.x = frogX * TILE_SIZE;
    destRect.y = frogY * TILE_SIZE;
    destRect.w = TILE_SIZE;
    destRect.h = TILE_SIZE;
    SDL_RenderCopy(renderer, backgroundTexture, NULL, &destRect);

    SDL_RenderCopy(renderer, frogTexture, NULL, &destRect);

    SDL_RenderPresent(renderer);
}

//Function to handle end game scenarios
bool handleEndGame(const char* message) {
    draw();
    if (strcmp(message, "Would you like to quit?") == 0) {
        drawText(message, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 50);
        drawText("Yes (Y) / No (N)", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2);
    } else {
        drawText(message, WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2 - 50);
        drawText("Play Again? Yes (Y) / No (N)", WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2);
    }
    SDL_RenderPresent(renderer);

    SDL_Event e;
    bool answered = false;
    bool playAgain = false;

    //Wait for player to decide to play again
    while (!answered) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                return false;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_y:
                        playAgain = true;
                        answered = true;
                        break;
                    case SDLK_n:
                        playAgain = false;
                        answered = true;
                        break;
                }
            }
        }
    }
    return playAgain;
}

//Function used for game logic
void logic(bool updateLanes) {
    if (updateLanes) {
        for (int i = 0; i < HEIGHT; i++) {
            updateLane(i);
        }
    }

    //Check if the player hits a hazard
    if (lanes[frogY][frogX] == 'x') {
        Mix_PlayChannel(-1, hitSound, 0);
        if (handleEndGame("Game Over!")) {
            initGame();
        } else {
            SDL_Quit();
            exit(0);
        }
    }

    //Check if the player reaches the goal
    if (frogY == 0) {
        Mix_PlayChannel(-1, victorySound, 0);
        if (handleEndGame("You Win!")) {
            initGame();
        } else {
            SDL_Quit();
            exit(0);
        }
    }
}

//Function to draw text on the screen
void drawText(const char* message, int x, int y) {
    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, message, color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    // Initialize SDL_mixer
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    // Initialize SDL_ttf
    TTF_Init();

    // Create window
    window = SDL_CreateWindow("Frogger", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    // Load font
    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 24); // Ensure you have a valid path to a TTF font file

    // Load images
    SDL_Surface* surface = IMG_Load("/home/majora/Documents/COMPSCI 1XC3/Assignment 2/Player Sonic.png"); // Replace with your frog image path
    frogTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("/home/majora/Documents/COMPSCI 1XC3/Assignment 2/Running Sonic Left.png"); // Replace with your obstacle images paths
    obstacleLeftTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("/home/majora/Documents/COMPSCI 1XC3/Assignment 2/Running Sonic Right.png");
    obstacleRightTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("/home/majora/Documents/COMPSCI 1XC3/Assignment 2/Wall.png");
    obstacleStationaryTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = IMG_Load("Frogger Background.png");
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // Load audio
    backgroundMusic = Mix_LoadMUS("/home/majora/Documents/COMPSCI 1XC3/Assignment 2/Green Hill Zone Theme.mp3");
    moveSound = Mix_LoadWAV("Move.wav");
    hitSound = Mix_LoadWAV("Hit.wav");
    victorySound = Mix_LoadWAV("Win.wav");

    //Play the background music
    Mix_PlayMusic(backgroundMusic, -1);

    //Initialize the game
    initGame();

    //main game loop with keyboard input using SDL
    bool quit = false;
    SDL_Event e;
    int counter = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_w:
                        if (frogY > 0) {
                            frogY--;
                            Mix_PlayChannel(-1, moveSound, 0);
                        }
                        break;
                    case SDLK_s:
                        if (frogY < HEIGHT - 1) {
                            frogY++;
                            Mix_PlayChannel(-1, moveSound, 0);
                        }
                        break;
                    case SDLK_a:
                        if (frogX > 0) {
                            frogX--;
                            Mix_PlayChannel(-1, moveSound, 0);
                        }
                        break;
                    case SDLK_d:
                        if (frogX < WIDTH - 1) {
                            frogX++;
                            Mix_PlayChannel(-1, moveSound, 0);
                        }
                        break;
                    case SDLK_q:
                        if (!handleEndGame("Would you like to quit?")) {
                            initGame();
                        } else {
                            SDL_Quit();
                            exit(0);
                        }
                        break;
                }
            }
        }

        logic(counter % 5 == 0);
        draw();
        SDL_Delay(200);
        counter++;
    }

    //Clean up when user quits
    SDL_DestroyTexture(frogTexture);
    SDL_DestroyTexture(obstacleLeftTexture);
    SDL_DestroyTexture(obstacleRightTexture);
    SDL_DestroyTexture(obstacleStationaryTexture);
    SDL_DestroyTexture(backgroundTexture);
    TTF_CloseFont(font);
    Mix_FreeChunk(hitSound);
    Mix_FreeChunk(moveSound);
    Mix_FreeChunk(victorySound);
    Mix_FreeMusic(backgroundMusic);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
