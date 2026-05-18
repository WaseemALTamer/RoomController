#ifndef STRUCTURE_H
#define STRUCTURE_H


#include <TFT_eSPI.h>


class Container{
public:
    int x, y, w, h;

    Container(int x, int y, int w, int h)
        : x(x), y(y), w(w), h(h) {}

    virtual void draw(TFT_eSprite* display, int new_x, int new_y) = 0;
};


#endif