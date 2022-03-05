//
// Created by selexin on 26/3/20.
//

#ifndef STEPPER_CONTROL2_EASYDRIVER_H
#define STEPPER_CONTROL2_EASYDRIVER_H

#define EASYDRIVER_MODE_FULL_STEP    1
#define EASYDRIVER_MODE_HALF_STEP    2
#define EASYDRIVER_MODE_QUARTER_STEP 4
#define EASYDRIVER_MODE_EIGHTH_STEP  8

#define EASYDRIVER_DIRECTION_FORWARDS  1
#define EASYDRIVER_DIRECTION_REVERSE  -1

class EasyDriver {
private:
    int PIN_STEP;
    int PIN_DIR;
    int PIN_MS1;
    int PIN_MS2;
    int PIN_ENABLE;

    bool enabled;
    int currentMode;
    int direction;
    int delayMillis;

public:
    /**
     * Constructor, args:
     * PIN_STEP
     * PIN_DIR
     * PIN_MS1
     * PIN_MS2
     * PIN_ENABLE
     */
    EasyDriver(int pinStep, int pinDir, int pinMs1, int pinMs2, int pinEnable);

    // Reset all pins to defaults
    void reset();

    // Enable/disable the stepper
    void enable(bool enabled);

    // Set the resolution to one of the MODE_X_STEP constants
    void setMode(int mode);

    void setDirection(int direction);

    void setDelay(int millis);

    // Take one step in the current set direction
    void step();

    // Take one step forward
    void stepForward();

    // Take one step backwards
    void stepReverse();
};


#endif //STEPPER_CONTROL2_EASYDRIVER_H
