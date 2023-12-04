#ifndef GAMEMENUS_H
#define GAMEMENUS_H

// headers to include the libraries neccesary for this library
#include "SimulatorLibraries.h"

// ------------------- Menu Class: S.P. -------------------
// Functions: S.P.
class Menu{ 
    public: 
        bool menuState = false; // determines if the game is allowed to enter the menu loop 
    virtual void drawMenu(){ 
        /*
            Each respective Menu is a child of this menu and calls it's own draw menu which then 
            creates a FEHImage class and opens the base image of the meny that is to be drawn for that child
            each FEHImage is stored respectively in the Menu that it is to be called from
        */
        CLEAR(); // draw blank
    }
    void CLEAR() {
        // draws blank background
        FEHImage EmptyBackground; 
        EmptyBackground.Open("Images.pic\\EmptyBackgroundFEH.pic"); 
        EmptyBackground.Draw(0,0); 
        EmptyBackground.Close(); 
    }
};

// ------------------- Start Menu Class: S.P. -------------------
// Functions: S.P.
class StartMenu : public Menu {
    public:
    void drawMenu() override {
        FEHImage StartBlank; 
        StartBlank.Open("Images.pic\\StartMenuFEH.pic"); 
        StartBlank.Draw(0,0); 
        StartBlank.Close(); 
        
    }
};

// ------------------- Stats Menu Class: S.P. -------------------
// Functions: S.P.
class StatsMenu : public Menu {
    private:
        FILE *statsfileptr; // grabs the file ptr to read from saved data
    public: 
        // all the displayable stats variables
        int accuracy; 
        int score; 
        int stars;
        int correctClicked; 
        int incorrectClicked; 
        int totalClicked;
    void drawMenu() override {
        FEHImage Stats; 
        Stats.Open("Images.pic\\StatsFEH.pic"); 
        Stats.Draw(0,0); 
        Stats.Close(); 

        // stats specific call
        displayStats(); 
    }

    void readStats() {
        // this function reads from the stats file and stores them in the respective variables
        statsfileptr = fopen("game_files/songStats.txt", "r");
        if(statsfileptr != NULL){
            fscanf(statsfileptr, "%d%d%d%d%d%d", &score, &stars, &totalClicked, &correctClicked, &incorrectClicked, &accuracy);
        }
        else {
            score = stars = totalClicked = correctClicked = incorrectClicked = accuracy = 0; 
        }
        fclose(statsfileptr);        
    }

    void displayStats(){ 
        // this function displays all the stats to the stats menu screen 
        readStats(); // the read stats function needs to be called in order to display any stats
        
        // converts all the stats variables into displayable strings
        char scoreStr[50], clickedStr[50], correctStr[50], incorrectStr[50], accStr[20];
        sprintf(scoreStr, "High Score: %d", score);
        sprintf(clickedStr, "DotsClicked: %d", totalClicked);
        sprintf(correctStr, "Correct: %d", correctClicked);
        sprintf(incorrectStr, "Incorrect: %d", incorrectClicked);
        sprintf(accStr, "Best Accuracy: %d", accuracy);
        
        // displays the stats 
        LCD.SetFontColor(WHITE);
        LCD.WriteAt(scoreStr, 50, 80+10);
        LCD.WriteAt(clickedStr, 50, 105+10);
        LCD.WriteAt(correctStr, 50, 130+10);
        LCD.WriteAt(incorrectStr, 50, 155+10);
        LCD.WriteAt(accStr, 50, 180+10);
    }

};

// ------------------- Guide Menu Class: S.P. -------------------
// Functions: S.P.
class GuideMenu : public Menu {
    public:
    void drawMenu() override {
        FEHImage Guide; 
        Guide.Open("Images.pic\\GuideFEH.pic"); 
        Guide.Draw(0,0); 
        Guide.Close(); 
    }
};

// ------------------- Credits Menu Class: S.P. -------------------
// Functions: S.P.
class CreditsMenu : public Menu {
    public:  
    void drawMenu() override {
        FEHImage Credits; 
        Credits.Open("Images.pic\\CreditsFEH.pic"); 
        Credits.Draw(0,0); 
        Credits.Close(); 

    }
};

// ------------------- Performance Menu Class: S.P. -------------------
// Functions: S.P.
class PerformanceMenu : public Menu {
    public: 
    void drawMenu() override {
        FEHImage Performance; 
        Performance.Open("Images.pic\\PerformanceFEH.pic"); 
        Performance.Draw(0,0); 
        Performance.Close(); 
    }
};

// ------------------- Difficulty Menu Class: S.P. -------------------
// Functions: S.P.
class DifficultyMenu : public Menu {
    public: 
    void drawMenu() override {
        FEHImage Difficulty; 
        Difficulty.Open("Images.pic\\DifficultyFEH.pic"); 
        Difficulty.Draw(0,0); 
        Difficulty.Close(); 
    }
}; 

#endif