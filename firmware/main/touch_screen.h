#ifndef TOUCHSCREEN_H
#define TOUCHSCREEN_H

#include <Wire.h>
#include <TAMC_GT911.h>
#include "constants.h"

namespace TouchScreen {

    // Variables
    extern int read_cooldown_duration_ms;
    extern TAMC_GT911 ts;

    // Functions
    bool read_cooldown_passed();

    void init();

    void loop();

}

#endif // TOUCHSCREEN_H