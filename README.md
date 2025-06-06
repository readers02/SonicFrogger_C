# SonicFrogger_C
Similar to frogger in python, but made in the C language

Compiling and Running the Program
1 - frogger.c\n
To compile frogger.c:\n
gcc-o frogger frogger.c
Then run the ’frogger’ exectuable by typing ’./frogger’ in the terminal
2 - frogger_GUI.c
To compiling frogger GUI.c:
**Ensure SDL2, SDL2 ttf, SDL2 mixer, and SDL2 image are installed** 
**Ensure that all file addresses are set correctly to access the image, video, and text necessary**
gcc-o frogger_GUI frogger_GUI.c-lSDL2-lSDL2 ttf-lSDL2 mixer-lSDL2 image
Then run the ’frogger_GUI’ exectuable by typing ’./frogger GUI’ in the terminal
