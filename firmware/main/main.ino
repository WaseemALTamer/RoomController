
#include "tick_system/transition_ease_in_out.h"

#include "render_system.h"
#include "ui_objects.h"
#include "constants.h"
#include "math.h"
#include "touch_screen.h"



Objects::Circle test_object(
    0, 0,
    25,
    0xffcc00
);



EaseInOut animation;

void on_animation_callback(float value, void* ctx){
    

    if(!animation.uniform.is_running){
        if (value == 0){
            animation.start_transition();
        }else{
            animation.reverse_transition();
        }
    }

    
    float r = 100;
    float angle = value * 2 * PI; // value is betwen 0 - 2pi


    float dx = sin(angle) * r;
    float dy = cos(angle) * r;




    if (test_object.isdragging){
        return;
    }

    test_object.move_to(((RenderSystem::tft.width() - test_object.w) / 2 + dx), 
                        (RenderSystem::tft.height() - test_object.h) / 2 - dy);
}



void setup() {

    Serial.begin(115200);

    RenderSystem::init();
    TouchScreen::init();

    RenderSystem::objects[0] = &test_object;




    pinMode(LCM_BL, OUTPUT);
    digitalWrite(LCM_BL, HIGH);



    animation.duration = 3;
    animation.starting_value = 0;
    animation.ending_value = 1;
    animation.add_callback(on_animation_callback);
    animation.start_transition();

}


void loop() {

    
    animation.update();


    TouchScreen::loop();
    RenderSystem::loop();
}