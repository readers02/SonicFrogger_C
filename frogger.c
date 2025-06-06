#include <unistd.h>  // used in kbhit()
#include <termios.h> // used in kbhit()
#include <fcntl.h>   // used in kbhit()
#include <stdio.h>
#include <stdlib.h>  // Standard library for memory allocation, process control, etc.

// Game constants
#define WIDTH 64
#define HEIGHT 10

// Function declarations
void initGame();
void draw();
void input();
void logic(int updateLanes);
void updateLane(int lane);
int kbhit();

// Game global variables
int frogX, frogY;
char lanes[HEIGHT][WIDTH + 1] = {
    "xxx..xxx..xxx..xxxxxxxxxxxxxxx..xxxxxxxxxxxxxxxxxxxxxxxxx..xxxxx", // lane0 (wall/end lane)
    "...xxxx..xxxxxx.......xxxx.....xx...xxxx.....xxxxxx...xxxxx.....", // lane1
    "....xxxx.....xxxx.....xxxx.......xxxxxxx.....xx....xxxxxx.......", // lane2
    "..xxx.....xxx.....xxx.....xxx...xxx....xx....xxxx....xx......xx.", // lane3
    "................................................................", // lane4 (safe lane)
    "....xxxx.......xxxx.........xxxx.......xxxx......xxxx....xxxx...", // lane5
    ".....xx...xx...xx......xx....xx.......xx..xx.xx......xx.......xx", // lane6
    "..xxx.....xx......xxxx..xx......xxxx......xxxx.......xxx...xxx..", // lane7
    "..xx.....xx.......xx.....xx.....xx..xx.xx........xx....xx.......", // lane8
    "................................................................"  // lane9 (start lane)
};
int speeds[HEIGHT] = {0, -2, +1, -1, 0, +2, -1, -1, +1, 0};

int main()
{
    //initialize the game state
    initGame();
    int counter = 0;
    while (1)
    {
        input();
        // (counter % 5 == 0) return either 0 or 1
        logic(counter % 5 == 0); // Update lanes every 5th iteration (t seconds)
        draw();
        usleep(200000); // Sleep for t/5 seconds (200 milliseconds)
        counter++;

        //check win condition
        if(frogY==0){
            printf("\nCONGRATULATIONS!\nYOU WIN\n");
            exit(0);
        }
    }
    return 0;
}

//initialize the game state
void initGame()
{
    // The initial position of "F" (Frog) in lane9
    // When frogY == 0 the game is over
    frogX = WIDTH / 2;
    frogY = HEIGHT - 1;
}

// printing the board every time the game is updated
void draw()
{
    //Following line everytime cleans the previous prints
    system("clear");

    //printing game board array
    for(int x=0; x<HEIGHT; x++){
        for(int y=0; y<WIDTH; y++){
            if(x == frogY && y == frogX){
                printf("F");
            } else{
                printf("%c", lanes[x][y]);
            }
        }
        printf("\n");
    }
    
    // Everytime the following line is going to be printed
    printf("Use W, A, S, D to move. Reach the top to win!\n");
}

//handles user input
void input()
{
    if (kbhit())
    {
        switch (getchar())
        {
            //quit case
            case 'q':
                printf("\nThe typed key: q\n");
                printf("Do you want to quit? (y/n) then press Enter: ");
                kbhit();
                switch (getchar())
                {
                case 'y':
                    exit(0);
                    break;
                case 'n':
                    break; 
                }
            case 'w':
                if(frogY > 0){
                    frogY--;
                    break;
                }
                break;
            case 'a':
                if(frogX > 0){
                    frogX--;
                    break;
                }
                break;
            case 's':
                if(frogY < HEIGHT-1){
                    frogY++;
                    break;
                }
                break;
            case 'd':
                if(frogX < WIDTH-1){
                    frogX++;
                    break;
                }
                break;
        }

        //check for collision with any 'x'
        if(lanes[frogY][frogX] == 'x'){
            printf("\nGAME OVER\n");
            exit(0);
        }
    }
}

//update game logic
void logic(int updateLanes)
{
    if(updateLanes){
        for(int i=0; i<HEIGHT; i++){
            updateLane(i);
        }
    }

}

// Update each lane based on its speed
void updateLane(int lane)
{
    if(speeds[lane]!=0){
        char temp[WIDTH];
        if(speeds[lane] > 0){
            // Move right
            for(int i=0; i<WIDTH; i++){
                temp[(i+speeds[lane])%WIDTH] = lanes[lane][i];
            }
        }else{
            // Move left
            for(int i=0; i<WIDTH; i++){
                temp[i] = lanes[lane][(i-speeds[lane]+WIDTH)%WIDTH];
            }
        }
        //Update the lane positions
        for(int i=0; i<WIDTH; i++){
            lanes[lane][i] = temp[i];
        }
    }
}

// receiving inputs from keyboard
int kbhit(void)
{
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}
