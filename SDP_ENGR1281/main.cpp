#include <glob.h>
#include <string.h>
#include <windows.h> 
#include <math.h>
#include <vector> 
#include <time.h>
#include <thread> 

// all the simulator libraries
#include "game_libraries\simulatorLibraries.h"

// useful global variables
    #define LENGTH 320
    #define HEIGHT 240
    #define CENTERX 160
    #define CENTERY 120
    #define GAME true
    #define BGCOLOR 0;
    #include "game_libraries\GamePiano.h" // piano frequencies

// all the Game altering libraries
#include "game_libraries\GameTrackers.h" 
    TimeManager timeManager; 

// menu Class defintion 
#include "game_libraries\GameMenus.h"
    // all menus Instances
    StartMenu startMenu; 
    StatsMenu statsMenu; 
    GuideMenu guideMenu; 
    CreditsMenu creditsMenu; 
    DifficultyMenu difficultyMenu; 
    PerformanceMenu performanceMenu;
    Menu playing; 
    int DIFFICULTY; // difficulty of game determined by difficulty menu

// all Button definitions
#include "game_libraries\GameButton.h"
    // main menu buttons Instances used by startmenu
    Button startMenuButtons[4] = {
        {176, 42, 70, 20, "Guide", &guideMenu},
        {200, 82, 70, 20, "Start", &difficultyMenu},
        {200, 122, 70, 20, "Stats", &statsMenu},
        {175, 164, 80, 20, "Credits", &creditsMenu}
    };

    // back button for credit, stats, performance, difficulty, and instruction menus instances
    Button backButton(10, 10, 50, 20, "Back", &startMenu);

    // other buttons
    DifficultyButton difficultyButtons[3] = {
        {55, 88, 50, 20, "Easy", &playing, &DIFFICULTY, 1},
        {55, 135, 70, 20, "Medium", &playing, &DIFFICULTY, 2}, 
        {55, 180, 50, 20, "Hard", &playing, &DIFFICULTY, 3}
    };
    ResetButton resetStatsButton(250, 10, 50, 20, "Reset"); 

    // buttons to change the time during game selection 
    IncrementButton timeUpButton(190, 170, 30, 20, &(timeManager.totalTime), 10); 
    IncrementButton timeDownButton(230, 170, 30, 20, &(timeManager.totalTime), -10); 

// all gameplay objects
#include "game_libraries\RhythmObjects.h"
    // ring instances: 1 - innermost, 5 - outermost
    RhythmRing Ring1(CENTERX, CENTERY, 20); 
    RhythmRing Ring2(CENTERX, CENTERY, 50); 
    RhythmRing Ring3(CENTERX, CENTERY, 80); 
    RhythmRing Ring4(CENTERX, CENTERY, 110); 
    
    // array with the radii of all the Rhythm Rings
    int rhythmRingsRadii[4] = {Ring1.radius, Ring2.radius, Ring3.radius, Ring4.radius};  

    // moving ring default constructors these are changed per difficulty selection 
    float movingRingVelocity = 0.6; 
    float movingRingForgiveness = 3; 
    MovingRing movingRing(CENTERX, CENTERY, 2, movingRingVelocity, movingRingForgiveness, &rhythmRingsRadii); 
    
    // Instantiate stats object to store and track game stats over multiple games
    GameStats currentGameStats;

    // Instance of Dot Manager -- dot manager that deletes, manages, and creates dots 
    std::vector<RhythmDot> dotVector = {};
    DotManager dotManager(dotVector, &Ring1, &Ring2, &Ring3, &Ring4, &movingRing, &currentGameStats); 

// START THE GAME FUNCTION
int main() {
    // GAME DEFINING VARIABLES
        startMenu.menuState = true; // the game starts with the start menu

    // default position retrieval variables to pass into fucntions
    int x_position, y_position;
    int x_trash, y_trash;

    // ------------------- Piano Note Files -------------------
    // Made by NEMETI TIVADAR with CC0, FOR ALL OTHER INSTANCES OF THIS SOUND REFER HERE
    bool menuMusicIsPlaying = PlaySound(TEXT("game_files\\mainMenu.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); // start the main menu music when game loads

    // MASTER loop 
    while (GAME) {   
        if (startMenu.menuState) {   
            // If the main menu music is already playing, don't restart it when you enter the main menu
            if(!menuMusicIsPlaying){
                PlaySound(TEXT("game_files\\mainMenu.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP); // see original reference
            }
            currentGameStats.resetStats(); // reset the game stats once you enter the main menu -- this is mainly done after performance menu
            startMenu.drawMenu(); // draw the initial instance of the start menu 

            // start menu loop
            while (startMenu.menuState) {
                LCD.Touch(&x_position, &y_position); // constantly grab hover position 
                for(int i  = 0; i < 4; i++) {
                    startMenuButtons[i].updateButton(x_position, y_position); // constantly draw menu buttons
                    if(LCD.Touch(&x_position, &y_position)) { // if touched check the button states
                        startMenuButtons[i].checkState(x_position, y_position); 
                    }
                }
            }
       }

        if (statsMenu.menuState) {
            statsMenu.drawMenu(); // draw the initial stats menu
            // loop for stats menu
            while(statsMenu.menuState) {
                // back button and stats button logic
                LCD.Touch(&x_position, &y_position);// constantly grab hover position
                // constantly draw menu buttons
                backButton.updateButton(x_position, y_position); 
                resetStatsButton.updateButton(x_position, y_position); 
                if(LCD.Touch(&x_position, &y_position)) { // if touched check the button states
                    backButton.checkState(x_position, y_position);
                    resetStatsButton.checkState(x_position, y_position);
                }
            }
            
        }

        if (guideMenu.menuState) {
            guideMenu.drawMenu(); // draw the initial guide menu 
            // loop for guide menu 
            while(guideMenu.menuState) {
                // back button logic
                LCD.Touch(&x_position, &y_position); // constantly grab hover position 
                backButton.updateButton(x_position, y_position); // constantly draw back button 
                if(LCD.Touch(&x_position, &y_position)) {backButton.checkState(x_position, y_position); } // if touched check back button state
            }
        }

        if (creditsMenu.menuState) {
            guideMenu.drawMenu(); // draw the initial credits menu 
            // loop for credits menu 
            while(creditsMenu.menuState) {
                // back button logic
                LCD.Touch(&x_position, &y_position); // constantly grab hover position 
                backButton.updateButton(x_position, y_position); // constantly draw back button 
                if(LCD.Touch(&x_position, &y_position)) {backButton.checkState(x_position, y_position); } // if touched check back button state
            }
        }
    
        // loop for difficulty menu 
        if(difficultyMenu.menuState){ 
            difficultyMenu.drawMenu(); // draw the initial guide menu 
            // loop for difficulty menu
            while (difficultyMenu.menuState){
                LCD.Touch(&x_position, &y_position); // constantly grab hover position 

                // print the total time that the game will play for
                LCD.SetFontColor(WHITE); LCD.WriteAt(timeManager.totalTime, 210, 130);

                // button logic
                for (int i = 0; i < 3; i++ ) {
                    // constantly draw difficulty and back buttons -- TIME BUTTONS ARE TECHNICALLY INVISIBLE SO THEY DON'T NEED TO BE DRAWN
                    difficultyButtons[i].updateButton(x_position, y_position); 
                    backButton.updateButton(x_position, y_position);
                    if (LCD.Touch(&x_position, &y_position)) { // if touched check the button states
                        backButton.checkState(x_position, y_position);
                        difficultyButtons[i].checkState(x_position, y_position); 
                        timeDownButton.checkState(x_position, y_position); 
                        timeUpButton.checkState(x_position, y_position); 
                    }
                }
            }
        }

        if(playing.menuState){
            // PRE GAME FUNCTIONS
            dotManager.totalDots.clear(); // Clear vector of any leftover dots (bug fix)
            dotManager.setDifficulty(DIFFICULTY); // grab the game difficulty chosen and pass it ino the dot manager
            currentGameStats.resetStats(); // reset the current game stats again 
            playing.drawMenu(); // draw the initial playing screen -- THIS IS JUST THE BLANK BACKGROUND
            PlaySound(0,0,0); // end all previous sounds
            
            // grab the start time
            timeManager.timeStart = time(NULL);
            long currentTime, timeEndMinutes, timeEndSeconds;
            char timeStr[10];

            // loop for playing game
            while (playing.menuState) {
                // frame rate --- COMMENT ON TO CHECK FRAME RATE
                    // long timeStartFrame = time(NULL);
                    // int counter; 
                    // counter++;  

                // Update the time managers time passed
                currentTime = time(NULL);
                if(currentTime >= timeManager.timeStart + timeManager.totalTime){ // if the time to play is over
                    // Erase all dots from vector after game is over
                    for(int i=0;i<dotManager.totalDots.size(); i++){
                        dotManager.totalDots.erase(dotManager.totalDots.begin() + i);
                    }
                    // once time has finished
                    performanceMenu.menuState = true; // go to the performance menu -- The only way to get to the performance menu is by finishing a game
                    playing.menuState = false; // exit out of the playing loop 
                    movingRing.radius = 2; // reset the moving ring radius for next game
                }

                // TIME LOGIC
                    // erase past time that was displayed
                    sprintf(timeStr, "%d:%d", timeEndMinutes, timeEndSeconds); // change the time ints to a displayable string
                    LCD.SetFontColor(BLACK); LCD.WriteAt(timeStr, 10, 200); // erase past time

                    // calculate timeLeft
                    timeEndMinutes = ((timeManager.timeStart + timeManager.totalTime) - currentTime) / 60;
                    timeEndSeconds = ((timeManager.timeStart + timeManager.totalTime) - currentTime) % 60;

                    // display the new time left
                    sprintf(timeStr, "%d:%d", timeEndMinutes, timeEndSeconds); // change the time ints to a displayable string
                    LCD.SetFontColor(WHITE); LCD.WriteAt(timeStr, 10, 200); // draw new time

                /*
                    Draw the new score, the update dots from dotmanager tracks the score and erases the last score once a score 
                    needs to be iterated, so the old score does not need to be deleted every loop, only when the score changes
                */
                LCD.SetFontColor(WHITE); LCD.WriteAt(currentGameStats.score, 250, 10);

                bool mousedown = LCD.Touch(&x_position, &y_position); // get mouse position 

                // stationary ring logic
                Ring1.drawRing();  Ring2.drawRing();  Ring3.drawRing();  Ring4.drawRing();  
                
                dotManager.createDot(); // create the dots
                dotManager.updateDots(); // update the dots, this function also draws the dots
                
                if(mousedown) { dotManager.checkDots(x_position, y_position); } // if touched, check all the dots to update their states
                
                movingRing.drawRing(); // update the ring position

                // FPS TRACKER --- COMMENT ON TO CHECK FRAME RATE
                    // if (time(NULL) > timeStartFrame) {
                    //     printf("%d fps\n", counter); 
                    //     counter = 0; 
                    // }  
            }
        }

        if(performanceMenu.menuState){
            performanceMenu.drawMenu(); // draw the initial performance screen

            // calculate all the scores after the game has finished
                currentGameStats.calcAccuracy(); 
                currentGameStats.calcStars(); 
                // Check game save state
                currentGameStats.checkSave();

            // these variables control the order and where the stats are displayed
                int starPositionX = 100; // where the star row stars being drawn from in the x
                bool printStars = true; // control so the stars are only drawn once in the loop 
                bool printAccuracy =  true; // control so the accuracy is only drawn once in the loop 
                int accuracyBarLength = 0; // the initial accuracy bar length
                int accuracyBarTotalLength = 100; // how long the accuracy bar could be
                char strAccuracy[6]; // the string to hold the int accuracy value
                    sprintf(strAccuracy, "%d%", int(currentGameStats.accuracy)); // converts the accuracy value to a string
            
            //  print blank stars
            for(int i = 0; i < 5; i++) {
                LCD.SetFontColor(WHITE); LCD.DrawCircle(starPositionX, 200, 11);  
                starPositionX += 30; // the space between the blank stars
            }
            starPositionX = 100; // after blank stars are drawn reset where the stars are drawn from in the x

            // blank accuracy bar
            LCD.DrawRectangle(80, 90, accuracyBarTotalLength, 10); 

            while (performanceMenu.menuState){
                // accuracy logic
                if(printAccuracy){ // if the accuracy has not been printed yet
                    for(int i = 0; i < currentGameStats.accuracy; i++){ // draw the rectangle up to the actual accuracy 
                        LCD.DrawRectangle(80, 90, accuracyBarLength, 10); 
                        accuracyBarLength+=1; // itterate the length
                        Sleep(0.01); // pause for a bit to increase smootheness
                    }
                    printAccuracy = false; // make sure the accuracy is only drawn once
                }
                LCD.SetFontColor(WHITE); LCD.WriteAt(strAccuracy, 190, 88); // write the actual accuracy score

                LCD.SetFontColor(WHITE); LCD.WriteAt(currentGameStats.score, CENTERX - 20, 128); // write the score

                // earned star logic
                if (printStars && !printAccuracy) { // once the accuracy has been drawn and the stars have not been 
                    for(int i = 0; i < currentGameStats.stars; i++) { // print the stars that were earned
                        LCD.SetFontColor(DARKSLATEBLUE); LCD.FillCircle(starPositionX, 200, 9); 
                        starPositionX += 30; // the space between the stars
                        Sleep(1.0); // pause a bit to increase smootheness
                    }
                    printStars = false; // make sure the stars are only drawn once
                }
                // back button logic -- ONLY APPEARS ONCE EVERYTHING IS DRAWN -- and takes you back to the main menu
                LCD.Touch(&x_position, &y_position); // grab hover position 
                backButton.updateButton(x_position, y_position); // continually draw and update button 
                if(LCD.Touch(&x_position, &y_position)) { backButton.checkState(x_position, y_position);} // start the main menu music when start menu loads } // if clicked check button state
            }
            PlaySound(TEXT("game_files\\mainMenu.wav"), NULL, SND_FILENAME | SND_ASYNC | SND_LOOP ); // see original reference
        }
    }
    return 0;
}

