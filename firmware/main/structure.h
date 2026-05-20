#ifndef STRUCTURE_H
#define STRUCTURE_H


#include <TFT_eSPI.h>


struct TouchEvent{
    uint16_t x;
    uint16_t y;
    uint8_t size;
    uint8_t id = -1;
    
};


class Container{
public:
    int x, y, w, h;

    Container(int x, int y, int w, int h)
        : x(x), y(y), w(w), h(h) {}


    //bool is_visible = true;

    virtual void draw(TFT_eSprite* display, int new_x, int new_y) = 0;


    
    virtual void on_press(TouchEvent e) = 0;
    virtual void on_drag(TouchEvent e) = 0;
    virtual void on_rlease(TouchEvent e) = 0;

};






#endif