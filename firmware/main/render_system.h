#ifndef RENDER_SYSTEM_H
#define RENDER_SYSTEM_H

#include <TFT_eSPI.h>
#include "constants.h"
#include "structure.h"




namespace RenderSystem {

    extern TFT_eSPI tft;

    extern uint32_t backgruond_color;

    extern constexpr int TILE_SIZE = 32;

    extern TFT_eSprite* render_sprite_tile;

    extern constexpr const int num_width_tiles = 15;
    extern constexpr const int num_height_tiles = 10;


    struct Tile {
        int x_pos;
        int y_pos;
        bool is_dirty;
    };

    extern Tile screen_tiles[num_height_tiles][num_width_tiles];


    extern Container* objects[15];
    extern int objectCount;

    uint16_t rgb888_to_rgb565(uint32_t c);

    void render();
    void setup();
    void update();

}



#endif