#ifndef GAMETRACKERS_H
#define GAMETRACKERS_H

// Headers to include the libraries neccesary for this header
#include "SimulatorLibraries.h"
#include "math.h"
#include "stdlib.h"
#include "vector"
#include "fstream"

// Forward declarations
class MovingRing;
class DotManager;

// ------------------- Time Manager Class -------------------
// Functions: J.C
class TimeManager {
    public:
        int timeStart; // Game start
        int timePassed; // How much time has passed since the game started
        int totalTime; // Total duration of the game
    // Constructor
    TimeManager() : timeStart(0), timePassed(0), totalTime(60) {}

    // Method for setting the total duration of the game
    void setTotalTime(int totalSeconds) {
        totalTime = totalSeconds;
    }

    // Method that sets the start time to the current time
    void startTimer() {
        timeStart = TimeNow();
    }

    // Method that determines how much time has passed
    void updateTimer() {
        timePassed = TimeNow() - timeStart;
    }
};

// ------------------- Game Stats Class -------------------
// Functions: J.C
class GameStats {
    /*
        SAVE FILE STRUCTURE:
        SCORE   STARS   DOTSCLICKED   CLICKEDCORRECT   CLICKEDINCORRECT   ACCURACY

        GAME STATS: SCORE, STARS, ACCURACY
        GLOBAL STATS: DOTSCLICKED, CLICKEDCORRECT, CLICKEDINCORRECT
    */
    private:
        FILE *savefileptr; // pointer to save file
        std::vector<int> saveVect; // vector used for file writing
        float saveAcc;
    public:
        // Tracked stats
        int score;
        int stars;
        int dotsClicked;
        int dotsClickedCorrect;
        int dotsClickedIncorrect;
        int accuracy;
        
        // Initialize all stats to zero (object instantiated on game start)
        GameStats(){
            score = stars = dotsClicked = dotsClickedCorrect = dotsClickedIncorrect = accuracy = 0;
        }

        // Method to reset stats upon starting a new game
        void resetStats(){
            score = stars = dotsClicked = dotsClickedCorrect = dotsClickedIncorrect = accuracy = 0;
        }

        // Increase score by given amount
        void incrementScore(int increase){
            if (score == 0 && increase <= 51){
                score = score; 
            }
            else {
                score += increase;
            }
        }

        // Increment the amount of dots clicked correctly and the total number of dots clicked
        void incrementClickedCorrect(){
            dotsClickedCorrect++;
            dotsClicked++;
        }

        // Increment the amount of dots clicked incorrectly and the total number of dots clicked
        void incrementClickedIncorrect(){
            dotsClickedIncorrect++;
            dotsClicked++;
        }
        
        // Calculate the accuracy given dots clicked and dots clicked correctly
        void calcAccuracy(){
            if(dotsClicked > 0){
                accuracy = ((float)dotsClickedCorrect/(float)dotsClicked) * 100.0;
            }
            else {
                accuracy = 0;
            }
        }
        
        // Determine stars based on accuracy
        void calcStars(){
            if(accuracy > -9999999 && accuracy <= 20){
                stars = 0;
            }
            else if(accuracy > 20 && accuracy <= 40){
                stars = 1;
            }
            else if(accuracy > 40 && accuracy <= 60){
                stars = 2;
            }
            else if(accuracy > 60 && accuracy <= 80){
                stars = 3;
            }
            else if(accuracy > 80 && accuracy <= 95){
                stars = 4;
            }
            else {
                stars = 5;
            }
        }

        // Check to see if any stats exceed current best and increment global stats
        void checkSave(){
            // Initialize temps
            int tempScores, tempStars, tempDotsClicked, tempDotsCorrect, tempDotsIncorrect;
            int tempAcc;
            // Read save data to compare
            savefileptr = fopen("game_files/songStats.txt", "r");
            
            if(savefileptr != NULL){
                // Read save data into temporary variables
                fscanf(savefileptr, "%d%d%d%d%d%d", &tempScores, &tempStars, &tempDotsClicked, &tempDotsCorrect, &tempDotsIncorrect, &tempAcc);

                // Save best results either from game or save into save vect or saveAcc
                saveVect.push_back(tempScores > score ? tempScores : score);
                saveVect.push_back(tempStars > stars ? tempStars : stars);
                
                // Increment the global stats
                saveVect.push_back(tempDotsClicked + dotsClicked);
                saveVect.push_back(tempDotsCorrect + dotsClickedCorrect);
                saveVect.push_back(tempDotsIncorrect + dotsClickedIncorrect);

                // Save best accuracy result
                saveVect.push_back(tempAcc > accuracy ? tempAcc : accuracy);
            }
            // Close save
            fclose(savefileptr);
            // Update the save
            if(savefileptr != NULL)
                updateSave();
        }

        // Method for clearing the save file
        void clearSave(){
            // Open save file for writing
            savefileptr = fopen("game_files/songStats.txt", "w");
            // Clear save
            if(savefileptr != NULL){
                fprintf(savefileptr, "%d\t%d\t%d\t%d\t%d\t%d", 0, 0, 0, 0, 0, 0);
                // Close save
                fclose(savefileptr);
            }
        }

        // Method for writing to the save file based on the results of checkSave()
        void updateSave(){
            // Open save file for writing
            savefileptr = fopen("game_files/songStats.txt", "w");
            // Write best data to save
            if(savefileptr != NULL){
                fprintf(savefileptr, "%d\t%d\t%d\t%d\t%d\t%d", saveVect.at(0), saveVect.at(1), saveVect.at(2), saveVect.at(3), saveVect.at(4), saveVect.at(5));
                // Close save
                fclose(savefileptr);
            }
        }  
}; 

#endif