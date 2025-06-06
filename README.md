# SonicFrogger_C<br>
Similar to frogger in python, but made in the C language<br>
<br>
Compiling and Running the Program<br>
1 - frogger.c<br>
To compile frogger.c:<br>
gcc -o frogger frogger.c<br>
Then run the ’frogger’ exectuable by typing ’./frogger’ in the terminal<br>
2 - frogger_GUI.c<br>
To compiling frogger_GUI.c:<br>
**Ensure SDL2, SDL2 ttf, SDL2 mixer, and SDL2 image are installed**<br>
**Ensure that all file addresses are set correctly to access the image, video, and text necessary**<br>
gcc -o frogger_GUI frogger_GUI.c -lSDL2 -lSDL2_ttf -lSDL2_mixer -lSDL2_image<br>
Then run the ’frogger_GUI’ exectuable by typing ’./frogger_GUI’ in the terminal
