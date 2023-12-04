#ifndef GAMEBUTTON_H
#define GAMEBUTTON_H

// headers to include the libraries neccesary for this header
#include "simulatorLibraries.h"
#include "GameMenus.h"
#include "windows.h"
#include "GamePiano.h"

// ------------------- Button Class: S.P. -------------------
// Functions: S.P.
class Button { 
    public:
        int x_position; 
        int y_position; 
        int length; 
        int height; 
        char name[40];
        Menu *controlledMenu; // determines what menu the button switches to when clicked
        bool hoverstate = false; // determines whether the button is being hovered or not
    Button(int x_pos, int y_pos, int setLength, int setHeight, char *setName, Menu *controlled_Menu) {
        x_position = x_pos; 
        y_position = y_pos;  
        length = setLength;
        height = setHeight; 
        strcpy(name, setName); 
        controlledMenu = controlled_Menu; 
    }
    virtual void doAction() {
        RESETMENUS(); // resets all the menu states -- this exits all menu loops
        controlledMenu->menuState = true; // sets the game to go to the controlled menu loop 
        Beep(200, 20); // responsive beep
    }
    void updateButton(int hover_x, int hover_y){
        // this function checks for the hover state of the button per mouse position
        if(hover_x >= x_position && hover_x <= (x_position + length) && hover_y >= y_position && hover_y <= (y_position + height)) {
            drawButton(true); // if it is being hovered draw it as being hovered
        }
        else {
            drawButton(false); // if it's not being hovered draw it normal 
        }
    }
    virtual void checkState(int x_pos, int y_pos){ 
        // this function checks the click state of the button
        if(x_pos >= x_position && x_pos <= (x_position + length) && y_pos >= y_position && y_pos <= (y_position + height)) { 
            doAction(); // if it was clicked do it's action 
        }
        
    }
    void drawButton(bool hoverState){
        // this function draws the button depending on it's hover state
        if(hoverState){
            LCD.SetFontColor(DARKSLATEBLUE); LCD.WriteAt(name, x_position, y_position);  
        }
        else {
            LCD.SetFontColor(WHITE); LCD.WriteAt(name, x_position, y_position); 
        }
    } 
    void RESETMENUS () {
        // this function resets the states of all the menus in use
        // all menus in use have to be defined here
            startMenu.menuState = false; 
            creditsMenu.menuState = false; 
            statsMenu.menuState = false; 
            guideMenu.menuState = false; 
            playing.menuState = false; 
            difficultyMenu.menuState = false; 
            performanceMenu.menuState = false; 
    }
};

// ------------------- Difficulty Button Class: S.P. -------------------
// Functions: S.P.
class DifficultyButton : public Button {
    private: 
        int setDifficulty; // what the button sets the gloabl difficulty as
        int *difficultyPTR; // the address of the gloabl difficulty variable
    public: 
    DifficultyButton(int x_pos, int y_pos, int setLength, int setHeight, char *setName, Menu *controlled_Menu, int *difficultyPTR, int setDiff) :  Button(x_pos, y_pos, setLength, setHeight, setName, controlled_Menu), difficultyPTR(difficultyPTR), setDifficulty(setDiff) {}

    void doAction() override {
        RESETMENUS(); // reset all the menus
        Beep(200, 20); // responsive beep
        controlledMenu->menuState = true; // sets the game to go to the controlled menu loop 
        *difficultyPTR = setDifficulty; // sets the difficulty to it's assigned difficulty
    }
};

// ------------------- Increment Button Class: S.P. (Only used for time up and time down) -------------------
// Functions: S.P.
class IncrementButton {
    private: 
        int x_position; 
        int y_position; 
        int length; 
        int height; 
        int incrementValue; // how much the button increments by 
        int *valueBeingIncremented; // the pointer to the value that's being altered by the button, this is only the time value realistically 
    protected: 
        void doAction() {
            Beep(200, 20); // responsive beep 
            // erase past time 
            Sleep(0.1); // this sleep is very important to not gather too much touches from the checkstate!
            LCD.SetFontColor(0); LCD.WriteAt(timeManager.totalTime, 210, 130); // draw the new time that was altered
            *valueBeingIncremented += incrementValue; // incremenet current time, new time is drawn in main loop 
            
        }
    public: 
    IncrementButton(int x_pos, int y_pos, int setLength, int setHeight, int *valueBeingIncremented, int incrementValue) {
        x_position = x_pos; 
        y_position = y_pos;  
        length = setLength;
        height = setHeight; 
        this->valueBeingIncremented = valueBeingIncremented; 
        this->incrementValue = incrementValue;  
    }
    void checkState(int x_pos, int y_pos){ 
        // this function checks the click state of the button 
        if(x_pos >= x_position && x_pos <= (x_position + length) && y_pos >= y_position && y_pos <= (y_position + height)) { 
            doAction(); // if it was clicked do it's action 
        }
        
    }
};
    
// ------------------- Reset Button Class: S.P. -------------------
// Functions: S.P.
class ResetButton { 
    /* 
        This class performs exactly the same as the normal button, it just doesn't alter a menu state
        The only thing this button does is reset the scores in the stored scores file
        Refer to Button for explantions on varibales and checkstate, drawButton, and updateButton functions. 
    */
    public:
        int x_position; 
        int y_position; 
        int length; 
        int height; 
        char name[40]; 
        bool hoverstate = false; 
    ResetButton(int x_pos, int y_pos, int setLength, int setHeight, char *setName) {
        x_position = x_pos; 
        y_position = y_pos;  
        length = setLength;
        height = setHeight; 
        strcpy(name, setName); 
    }
    void doAction() {
        GameStats resetStats; // dummy class, to call the reset function 
        resetStats.clearSave(); // call the function to reset all the save data
        statsMenu.drawMenu(); // draw the new statsMenu with the updated stats
        Beep(200, 20); // responsive beep
    }
    void updateButton(int hover_x, int hover_y){
        if(hover_x >= x_position && hover_x <= (x_position + length) && hover_y >= y_position && hover_y <= (y_position + height)) {
            drawButton(true); 
        }
        else {
            drawButton(false); 
        }
    }
    void checkState(int x_pos, int y_pos){ 
        if(x_pos >= x_position && x_pos <= (x_position + length) && y_pos >= y_position && y_pos <= (y_position + height)) { 
            doAction(); 
        }
    }
    void drawButton(bool hoverState){
        if(hoverState){
            LCD.SetFontColor(RED); LCD.WriteAt(name, x_position, y_position);  
        }
        else {
            LCD.SetFontColor(WHITE); LCD.WriteAt(name, x_position, y_position); 
        }
    } 
};
    void RESETMENUS () {
        // a menus have to be defined here
        startMenu.menuState = false; 
        creditsMenu.menuState = false; 
        statsMenu.menuState = false; 
        guideMenu.menuState = false; 
        playing.menuState = false; 
        difficultyMenu.menuState = false; 
        performanceMenu.menuState = false; 
    }

#endif