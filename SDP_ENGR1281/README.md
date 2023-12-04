# SDP_ENGR1281
The software design project for Fundementals of Engineering honors 1281.H

Authors: Jacob Cook and Saurav Poudyel 
copyrighted all the time; 

Enjoy your time playing the game and observe the following toruble shooting options if the game runs into issues

// ------------------- Troubleshooting Options -------------------
Running into choppy gameplay:
    - Try restarting your computer or vscode
    - You can also comment on the FPS tracker to see the state of the game
        - The game was tested on multiple computers to run at roughly 60 fps

Sounds aren't correctly playing 
    - The file paths to the songs should be relative, but check that everything is in the right place
        - Images.pic\GuideFEH.pic etc... for the FEH pics

Images aren't correctly displaying 
    - The file paths to the images should be relative, but check that everything is in the right place
        - game_files\mainMenu.wav for MainMenu
        - game_files\Piano\b4.wav etc... for the piano notes

Stats aren't correctly displaying 
    - The file paths to the stats should be relative, but check that everything is in the right place
        - game_files\songStats.txt for the songStats

The game isn't running correctly at all
    - Check that all the game libraries are downloaded and in the correct files
        - game_libraries\GameTrackers.h etc... for all the libraries
    - The game utilizes windows.h to play sound but smooth functioning requires a d.ll library flag to be added to the makeFile present in "simulator_libraries"
        // ------------------- THIS FUNCTION IN simulator_libraries\Makefile -------------------

            ifeq ($(OS),Windows_NT)
                LDFLAGS = -lopengl32 -lgdi32
                EXEC = game.exe

        // ------------------- SHOULD LOOK LIKE THIS -------------------

            ifeq ($(OS),Windows_NT)
                LDFLAGS = -lopengl32 -lgdi32 -w -lwinmm 
                EXEC = game.exe
            
