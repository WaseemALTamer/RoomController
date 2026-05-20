#include <Wire.h>
#include <TAMC_GT911.h>
#include "constants.h"
#include "structure.h"
#include "render_system.h"



namespace TouchScreen{


    
    int read_cooldown_duration_ms = 10;
    
    TAMC_GT911 ts = TAMC_GT911(
                        TOUCH_SDA,
                        TOUCH_SCL,
                        TOUCH_INT,
                        TOUCH_RST,
                        TOUCH_WIDTH,
                        TOUCH_HEIGHT
                    );


    bool read_cooldown_passed() {
        // this will be used to keep the system working and so we dont
        // overload the I2C bus and for us to wait for it each read

        static unsigned long last_read_time = 0;
        unsigned long now = millis();

        if (now - last_read_time < read_cooldown_duration_ms) {
            return false;
        }

        last_read_time = now;
        return true;
    }

    
    inline bool hitTest(int px, int py, Container* obj) {
        // AABB hit box check

        return (obj != nullptr) &&
            (px >= obj->x) &&
            (px <= obj->x + obj->w) &&
            (py >= obj->y) &&
            (py <= obj->y + obj->h);
    }


    constexpr int maxReleaseCountdown = 5;

    struct TouchBinding {
        Container* object = nullptr;
        TouchEvent event;
        int releaseCountdown  = maxReleaseCountdown;
    };


    constexpr int maxTouches = 5;
    TouchBinding binded_touches[maxTouches];    // the 5 reflect the library bing used max allowed
                                                // touch on screen, it is not a var.
                                                // this array index will reflect the id  since the
                                                // id of the touch is always between 0-4 depending
                                                // on the touch.
    

    void init() {
        Wire.begin(TOUCH_SDA, TOUCH_SCL);

        ts.begin();
    }

    void loop() {
        
        if (!read_cooldown_passed()) return;

        ts.read();

        

        for (int i = 0; i < ts.touches; i++) {
            
            // am correcting the touch event corrdenets right here
            // please change that to be more proffecianal

            
            TouchEvent e;
            e.x = ts.points[i].y;
            e.y = TOUCH_WIDTH - ts.points[i].x;
            e.size = ts.points[i].size;
            e.id = ts.points[i].id;

            //Serial.print(e.x);
            //Serial.print("   ");
            //Serial.println(e.y);


            
            Container* object = nullptr;

            bool no_change = true;

            // check existing bindings
            for (int j = 0; j < maxTouches; j++) {

                TouchBinding* touch_bind = &binded_touches[j];
                if (touch_bind == nullptr) continue;
                
                

                if (touch_bind->event.id == e.id) {
                    object = touch_bind->object;

                    // we only let the drag event propegate if there is a change
                    if (touch_bind->event.x != e.x ||
                        touch_bind->event.y != e.y){
                            touch_bind->event.x = e.x;
                            touch_bind->event.y = e.y;
                            touch_bind->event.size = e.size;
                            touch_bind->event.id = e.id;

                            no_change = false;
                        }

                    touch_bind->releaseCountdown = maxReleaseCountdown;
                    
                    break;
                }
            }


            // drag update
            if (object != nullptr) {
                if (no_change){
                    break;
                }

                object->on_drag(e);
                break;
            }


            // if not found, create binding
            if (object == nullptr) {

                for (int j = 0; j < RenderSystem::objectCount; j++) {

                    Container* candidate = RenderSystem::objects[j];
                    if (candidate == nullptr) continue;
                    if (hitTest(e.x, e.y, candidate)) {

                        object = candidate;
                        
                        // find free slot
                        for (int k = 0; k < maxTouches; k++) {

                            if (binded_touches[k].object == nullptr) {

  
                                binded_touches[k].event.x = e.x;
                                binded_touches[k].event.y = e.y;
                                binded_touches[k].event.size = e.size;
                                binded_touches[k].event.id = e.id;

                                binded_touches[k].object = object;

                                binded_touches[k].releaseCountdown = maxReleaseCountdown;

                                object->on_press(e);
                                break;
                            }
                        }

                    }
                }
            }
            
        }



        // detect the touch release
        for (int i = 0; i < maxTouches; i++) {

            TouchBinding& bind = binded_touches[i];

            // skip empty slots
            if (bind.object == nullptr) continue;

            bool found = false;
                
            for (int j = 0; j < ts.touches; j++) {
                if (ts.points[j].id == bind.event.id) {
                    found = true;
                    break;
                }
            }

            if (!found) bind.releaseCountdown -= 1;

            if (bind.releaseCountdown <= 0) {

                if (bind.object != nullptr) {
                    bind.object->on_rlease(bind.event);
                }

                bind.object = nullptr;
                bind.event.id = -1;
            }
        }



    }


}
