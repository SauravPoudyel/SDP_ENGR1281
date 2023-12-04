#ifndef RHYTHMOBJECTS_H
#define RHYTHMOBJECTS_H

// Headers to include the libraries neccesary for this header
#include "SimulatorLibraries.h"
#include "vector"
#include "math.h"
#include "windows.h"
#include "fstream"
#include "GameTrackers.h"

// Constants
#define BGCOLOR BLACK
#define CENTERX 160
#define CENTERY 120
#define FLOATS_EQUAL(a, b, epsilon) (std::abs((a) - (b)) < (epsilon))

// Piano frequencies
#include "GamePiano.h" 

// Forward declarations
class DotManager; 
class MovingRing; 
class GameStats; 

// ------------------- Rhythm Ring class (the 4 stationary rings) S.P. -------------------
// Functions: S.P.
class RhythmRing {
    protected:  
    public:
        // Class vars
        int x_position; 
        int y_position;
        int ringColor = WHITE; 
        float radius;
    // Constructor - Accepts x and y positions and a radius
    RhythmRing(int x_pos, int y_pos, float r) {
        radius = r;
        x_position = x_pos; 
        y_position = y_pos; 
    }
    // Method for rendering the ring on screen
    void drawRing(){
        LCD.SetFontColor(ringColor); 
        LCD.DrawCircle(x_position, y_position, radius); 
    }
}; 

// ------------------- Moving Ring class (inherits from stationary rhythm rings) S.P. ------------------- 
// Functions: S.P.
class MovingRing : public RhythmRing {
    public: 
        // Class vars
        float velocity; // how fast the radius increases 
        float forgiveness; // how much leeway there is with the correct clicks
        int (*rhythmRingsRadiiPTR)[4]; // pointer to the array of 4 rhythmRingsRadii
        bool matching; // if the moving ring radius mathces a rhythm ring
        bool nearToRing; // if the moving ring is within 10 px of the stationary ring
    public:
        // Constructor - Accepts x and y positions (x_pos and y_pos), radius (r), velocity (v), forgiveness (forgiveness), rhythm ring radii pointers (RRingsRadiiPTR)
        MovingRing(int x_pos, int y_pos, float r, float v, float forgiveness, int (*RRingsRadiiPTR)[4]) : RhythmRing(x_pos, y_pos, r), velocity(v), forgiveness(forgiveness)  {
            rhythmRingsRadiiPTR = RRingsRadiiPTR;
            ringColor = PURPLE; 
        }

    // Method for updating the color and radius of the moving ring
    void updateRing() { 
        // cover the last ring 
        LCD.SetFontColor(BGCOLOR); 
        LCD.DrawCircle(x_position, y_position, radius); 
        
        // Radius increases linearly based on velocity
        radius += velocity; 

        // Change color part-way through expansion
        if (radius >= 70){
            updateColor(); 
        }
        // Erase the ring once it reaches max size and reset radius and color
        if(radius >= 120 ) {
            // cover the last ring 
            LCD.SetFontColor(BGCOLOR); 
            LCD.DrawCircle(x_position, y_position, radius);
            radius = 1.0; 
            ringColor = PURPLE; 
        }
    }

    // Method to change ring color (used with LCD.SetFontColor)
    void updateColor(){
        ringColor+= 400; 
    }

    // Method for checking moving ring radius against the radii of the stationary rings
    void checkRadius() {
        // Iterate through each stationary ring
        for(int i = 0; i < 4; i++) {
            // Set matching to true if: stationary ring radius - forgivenss <= moving ring radius <= stationary ring radius + forgiveness
            if (radius == ((*rhythmRingsRadiiPTR)[i] + forgiveness) || radius == ((*rhythmRingsRadiiPTR)[i] - forgiveness) ) {
                matching == true; 
            }
            if (radius <= ((*rhythmRingsRadiiPTR)[i] - 10)) {
                nearToRing == true; 
                
            }

        }
    }

    // Method for rendering the moving ring to the screen
    void drawRing() { 
        // Update moving ring's size
        updateRing(); 

        // Iterate through to check if the moving ring radius matches the non moving rings
        for(int i = 0; i < 4; i++){
            checkRadius();
        }

        // Draw new ring
        LCD.SetFontColor(ringColor); LCD.DrawCircle(x_position, y_position, radius);
    }
};

// ------------------- Rhythm dot class S.P. J.C. -------------------
// Functions: S.P. J.C. 
class RhythmDot {
    public: 
        // CONSTANTS
        int dotColor = WHITE; 
        float angle_increment = 0.1; // how much angle increases per update function call
        
        // OTHER
        int originalRadius; // Radius before expansion effect
        float displayRadius; // Actual radius of dot used for erasing 
        float timesBeenPassed = 0; // How many times the dot has been passed by the moving ring
        double angle; // angle (used to determine next position of dot in its orbit)
        bool clockwise; // determines if the dot is moving clockwise or counterclockwise
        bool dotState; // determines if the dot should be rendered 
        bool isCritical; // used to indicate when the dot should be clicked
        RhythmRing *assignedRingPTR; // reference to the rhythm ring the dot is orbiting around
        int x_position; // x-coordinate of the dot
        int y_position; // y-coordinate of the dot

        // Effect dependencies
        bool renderFade; // used in dot fade VFX

    // Constructor - Accepts radius (r), clockwise or counterclockwise (direction), rhythm ring reference pointer (rhythmRingPTR), starting angle (startingAngle)
    RhythmDot(int r, bool direction, RhythmRing *rhythmRingPTR, double startingAngle) {
        // defined constructor variables
        originalRadius = r; 
        displayRadius = originalRadius; 
        assignedRingPTR = rhythmRingPTR;
        x_position = assignedRingPTR->x_position - assignedRingPTR->radius;
        y_position = assignedRingPTR->y_position;
        clockwise = direction; 
        dotState = true; 
        angle = startingAngle; 

        renderFade = false; 
        isCritical = false; 
    }
    // Method for rendering the rhythm dot
    void drawDot() {
        // Draw the ring that it's on as it orbits
        assignedRingPTR->drawRing(); 

        // Erase past dot
        LCD.SetFontColor(BGCOLOR); LCD.FillCircle(x_position, y_position, displayRadius); 
        
        // Increment to the new radius
        updateDot(); 

        // Draw new dot - change color depending on if the dot is critical or not
        if(isCritical){
            LCD.SetFontColor(PURPLE); 
        }
        else {
            LCD.SetFontColor(dotColor); 
        } 
        LCD.FillCircle(x_position, y_position, displayRadius);
    }

    // Method for dot expansion effect
    void movingRingClose() {
        // Expand dot if it hasn't already been passed before
        if(timesBeenPassed < 1) {
            displayRadius += 0.08; 
        }
    }

    // Method for updating the dot's position in orbit
    void updateDot(){
        // X coordinate = xCoordinateOfOrigin + radius * cos(angle)
        x_position = CENTERX + assignedRingPTR->radius * cos(angle);
        // Y coordinate = xCoordinateOfOrigin + radius * sin(angle)
        y_position = CENTERY + assignedRingPTR->radius * sin(angle); 

        // Increment or decrement the angle depending on whether dot is moving clockwise or counterclockwise
        switch (clockwise){
        case true:
            // adjust the angle increment based on the radius of the ring dot is orbiting
            angle += angle_increment / assignedRingPTR->radius; 
            break;
        case false:
            angle -= angle_increment / assignedRingPTR->radius; 
            break;
        }
        
        // Adjust dot color 
        if (renderFade == true) {
            dotColor += 1000; 
        }
    }

    // Method for checking if the dot has been clicked
    void checkState(int x_touch_position, int y_touch_position) { 
        // If it's pressed within the right radius
        if(x_touch_position >= (x_position - displayRadius) && x_touch_position <= (x_position + displayRadius) && y_touch_position <= (y_position + displayRadius) && y_touch_position >= (y_position - displayRadius)){
            // Toggle the state so it doesn't keep getting rendered
            dotState = false;

            // Erase dot
            LCD.SetFontColor(BGCOLOR); LCD.FillCircle(x_position, y_position, displayRadius); 
        }
    }
};

// ------------------- Particle Effect Class J.C. -------------------
// Functions: J.C.
class ParticleEffect{
    public:
        // Position vars
        int xPos, yPos, velocity = 1;
        int radius = 1;
        // Timing vars
        int maxEndFrame = 30; // Max number of frames the effect should be rendered for
        int timer = 0; // Frame counter
        // Status vars
        bool isComplete = false; // Used to determine if the effect should be rendered
        bool isPositive; // Used to indicate which effect to play
    // Constructor - Accepts x and y positions (x and y) and positive indicator (positive)
    ParticleEffect(int x, int y, bool positive){
        // Offset position of negative feedback indicator slightly
        if(positive){
            xPos = x;
            yPos = y;
        }
        else {
            xPos = x-5;
            yPos = y-5;
        }
        isPositive = positive;
    }

    // toggle complete status of effect
    void toggleParticleEffect(){
        isComplete = !isComplete;
    }

    // Render effect conditionally
    void drawEffect(){
        if(isPositive){
            // Ring ripple effect
            LCD.SetFontColor(BGCOLOR);
            LCD.DrawCircle(xPos, yPos, radius);

            // Continue to animate effect until complete
            if(checkTimer()){
                // Radius of ripple-effect
                updateParticle();

                LCD.SetFontColor(WHITE);
                LCD.DrawCircle(xPos, yPos, radius);
            }
        }
        else {
            // Red "X"
            LCD.SetFontColor(RED);
            LCD.DrawLine((xPos-3), (yPos-3), (xPos+4), (yPos+4));
            LCD.DrawLine((xPos+3), (yPos-3), (xPos-4), (yPos+4));

            updateParticle();

            // Erase effect once it is completed
            if(checkTimer() == false){
                LCD.SetFontColor(BGCOLOR);

                LCD.DrawLine((xPos-3), (yPos-3), (xPos+4), (yPos+4));
                LCD.DrawLine((xPos+3), (yPos-3), (xPos-4), (yPos+4));
            }
        }
    }

    // Update radius of ring effect and increment timer for both
    void updateParticle(){
        // Make sure ripple doesn't go off screen
        if(isPositive && (yPos - (radius + velocity)) > 0 && (yPos + (radius + velocity)) < 240){
            radius += velocity;
        }
        timer++;
    }

    // Compare current frame to the end frame and return true if effect should still be rendered
    bool checkTimer(){
        if(timer < maxEndFrame){
            // Continue effect
            return(true);
        }
        else {
            // End effect
            toggleParticleEffect();
            return(false);
        }
    }
};

// ------------------- Dot Manager Class S.P. J.C. -------------------
// Functions: S.P. J.C.
class DotManager {
    protected: 
        // Object references
        RhythmRing *Ring1PTR; // Pointer to innermost ring
        RhythmRing *Ring2PTR; // Pointer to second innermost ring
        RhythmRing *Ring3PTR; // Pointer to third innermost ring
        RhythmRing *Ring4PTR; // Pointer to outermost ring
        MovingRing *movingRingPTR; // Pointer to moving ring 
        GameStats *currentGameStats; // Pointer to stat tracking object
        // Timer var
        int timeStart; // var used for timing purposes
    public:
        float pause = 3; // delay between dot generations
        std::vector<RhythmDot> totalDots; // Vector to store all dots rendered on screen
        std::vector<ParticleEffect> effectsToRender; // "Queue-like" system for effects that need to be rendered
    
    // Constructor - Accepts dot vector (dotVector), pointers to each ring, pointer to moving ring (MovingRingPTR), pointer to stats (gStats)
    DotManager(std::vector<RhythmDot> dotVector, RhythmRing *ring1PTR, RhythmRing *ring2PTR, RhythmRing *ring3PTR, RhythmRing *ring4PTR, MovingRing *MovingRingPTR, GameStats *gStats){
        totalDots = dotVector;
        Ring1PTR = ring1PTR;
        Ring2PTR = ring2PTR;
        Ring3PTR = ring3PTR;
        Ring4PTR = ring4PTR;
        movingRingPTR = MovingRingPTR;
        currentGameStats = gStats; 
        timeStart = TimeNow();
    } 
    // change moving ring and angle increments based on difficulty
    void setDifficulty(int difficulty) {
        // 1 easy, 3 hard
        // Adjust moving ring velocity based on difficulty (easy is just standard speed)
        switch(difficulty){
            case(2):
                movingRingPTR->velocity += 0.1;
                pause = 2; 
                break;
            case(3):
                pause = 1.5; 
                movingRingPTR->velocity += 0.2;
                break;
        }
        // Adjust angle increment of each dot based on difficulty (easy 1x speed, medium 2x speed, hard 3x speed)
        for(int i=0; i<totalDots.size(); i++){
            totalDots.at(i).angle_increment *= difficulty;
        } 
    }

    // Method for creating rhythm dots
    void createDot() {
        // Generate dots after a delay dictated by "pause"
        if(TimeNow() >= timeStart + pause){
            // Choose random number
            int randomizer = Random.RandInt();

            // String to store a reference to audio file
            char randomPianoNoteTemp[30]; 
            // Random angle around orbit to generate dot
            double randomAngle = randomizer%360; 

            // Determine random properties of dot and append dot to vector
            switch((randomizer % 4)+1){
                // Each case corresponds to a stationary ring
                case(1):
                    totalDots.push_back(RhythmDot(8, (randomizer % 2 == 0 ? true : false), Ring1PTR, randomAngle ));
                    break;
                case(2):
                    totalDots.push_back(RhythmDot(8, (randomizer % 2 == 0 ? true : false), Ring2PTR, randomAngle));
                    break;
                case(3):
                    totalDots.push_back(RhythmDot(8, (randomizer % 2 == 0 ? true : false), Ring3PTR, randomAngle));
                    break;
                case(4):
                    totalDots.push_back(RhythmDot(8, (randomizer % 2 == 0 ? true : false), Ring4PTR,  randomAngle));
            }
            timeStart = TimeNow();
        }
            
    }

    // Update the positions of all dots on screen
    void updateDots(){
        // Iterate through all the dots in the vector
        for(int i=0; i<totalDots.size(); i++){
            // If the dots should still be rendered
            if(totalDots.at(i).dotState == true){
                // If the dot has been passed by the moving ring once already, erase it
                if(int(totalDots.at(i).timesBeenPassed) > 1){
                    // Add negative particle effect to effect queue
                    effectsToRender.push_back(ParticleEffect(totalDots.at(i).x_position, totalDots.at(i).y_position, false));
                    // Decrement score
                    currentGameStats->incrementClickedIncorrect();
                    currentGameStats->incrementScore(-50);
                    // Toggle dot state
                    totalDots.at(i).dotState = false; 

                    // Draw dot
                    totalDots.at(i).drawDot(); 
                    
                    // Erase past dot
                    LCD.SetFontColor(BGCOLOR); 
                    LCD.FillCircle(totalDots.at(i).x_position, totalDots.at(i).y_position, totalDots.at(i).displayRadius); 

                    // Erase previous frame's score counter
                    LCD.SetFontColor(BLACK); 
                    LCD.FillRectangle(245, 10, 70, 20);
                }
                // Toggle critical state of the dot if the moving ring is within proper range
                else if(movingRingPTR->radius >= (totalDots.at(i).assignedRingPTR->radius - totalDots.at(i).displayRadius) && movingRingPTR->radius <= (totalDots.at(i).assignedRingPTR->radius + totalDots.at(i).displayRadius) ){
                    totalDots.at(i).isCritical = true; 
                    totalDots.at(i).drawDot();  
                }
                // Render dot normally if not critical
                else {
                    totalDots.at(i).isCritical = false; 
                    totalDots.at(i).drawDot();
                }
                // Render dot expansion effect when ring gets close
                if (movingRingPTR->radius >= (totalDots.at(i).assignedRingPTR->radius - 25) && movingRingPTR->radius <= (totalDots.at(i).assignedRingPTR->radius + 5)) {
                    totalDots.at(i).movingRingClose(); 

                    if(int(movingRingPTR->radius) == int(totalDots.at(i).assignedRingPTR->radius)) {
                        /* 
                            once the ring fully passes the radius the rhythm increment the times that dot has been passed
                            Keep in mind that the moving ring is 2 radius long and so it passes twice per one interval, hence +0.5
                        */
                        totalDots.at(i).timesBeenPassed += 0.5;  

                    }
                }
            }
        }
        // Iterate through all the effects in the queue
        for(int k=0; k<effectsToRender.size(); k++){
            // Render effect if its not done yet
            if(effectsToRender.at(k).isComplete == false){
                effectsToRender.at(k).drawEffect();
            }
            // Remove effect from queue if it is done
            else{
                effectsToRender.erase(effectsToRender.begin()+k);
            }
        }
    }

    // Method to check if a dot was clicked and if the click was correct or not 
    void checkDots(int x_pos, int y_pos){
        // Iterate through all the dots in the vector
        for(int i=0; i<totalDots.size(); i++){
            if(totalDots.at(i).dotState == true){
                // If it is being renderd and not is not being pressed, check if it's being pressed
                totalDots.at(i).checkState(x_pos, y_pos); 

                // Erase previous frame's score counter 
                LCD.SetFontColor(BLACK); 
                LCD.FillRectangle(245, 10, 70, 20);
            }
            else {
                // Correct click 
                if(movingRingPTR->radius >= (totalDots.at(i).assignedRingPTR->radius - 10) && movingRingPTR->radius <= (totalDots.at(i).assignedRingPTR->radius + 10) ) {
                    // Add a positive feedback effect to the queue at current (x, y) position 
                    effectsToRender.push_back(ParticleEffect(x_pos, y_pos, true));
                    // Incrememnt stats; 
                    currentGameStats->incrementClickedCorrect();
                    currentGameStats->incrementScore(100); 

                    // Erase previous frame's score counter
                    LCD.SetFontColor(BLACK); 
                    LCD.FillRectangle(245, 10, 70, 20);
                    
                    // Get random integer
                    int randomizer = Random.RandInt(); 

                    // Initialize with a default note
                    char randomPianoNoteTemp[100] = C4; 

                    // Determine random note frequency of the Rhythm dot
                    switch ((randomizer % 7) + 1) {
                        case 1:
                            PlaySound(TEXT(C4), NULL, SND_FILENAME | SND_ASYNC);
                            break;
                        case 2:
                            PlaySound(TEXT(D4), NULL, SND_FILENAME | SND_ASYNC );
                            break;
                        case 3:
                            PlaySound(TEXT(E4), NULL, SND_FILENAME | SND_ASYNC);
                            break;
                        case 4:
                            PlaySound(TEXT(F4), NULL, SND_FILENAME | SND_ASYNC);
                            break;
                        case 5:
                            PlaySound(TEXT(G4), NULL, SND_FILENAME | SND_ASYNC);
                            break;
                        case 6:
                            PlaySound(TEXT(A4), NULL, SND_FILENAME | SND_ASYNC);
                            break;
                        case 7:
                            PlaySound(TEXT(B4), NULL, SND_FILENAME | SND_ASYNC);
                            break;
                    }
                }
                // Incorrect click
                else {
                    // Add negative feedback effect to queue at current (x, y) position
                    effectsToRender.push_back(ParticleEffect(x_pos, y_pos, false));
                    // Decrement stats
                    currentGameStats->incrementClickedIncorrect();
                    currentGameStats->incrementScore(-50);

                    // Erase previous frame's score counter
                    LCD.SetFontColor(BLACK); 
                    LCD.FillRectangle(245, 10, 70, 20);
                    
                    // Play error sound effect
                    Beep(500,100); 
                
                }
                // Remove dot from dot vector
                totalDots.erase(totalDots.begin()+i);                  
            }
        }
        
    }
};

#endif