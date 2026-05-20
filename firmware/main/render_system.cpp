#include <TFT_eSPI.h>
#include "constants.h"
#include "structure.h"


namespace RenderSystem{


    uint16_t rgb888_to_rgb565(uint32_t c)
    {
        uint8_t r = (c >> 16) & 0xFF;
        uint8_t g = (c >> 8)  & 0xFF;
        uint8_t b = c & 0xFF;

        return ((r & 0xF8) << 8) |
            ((g & 0xFC) << 3) |
            (b >> 3);
    }

    TFT_eSPI tft = TFT_eSPI();
    uint32_t backgruond_color = 0x000000;

    constexpr int TILE_SIZE = 32;


    TFT_eSprite render_sprite_tile = TFT_eSprite(&tft);

    // we flip the width with the height since we rotate the lcd
    const int num_width_tiles = 15;
    const int num_height_tiles = 10;



    struct Tile{
        int x_pos;
        int y_pos;

        bool is_dirty;
    };


    Tile screen_tiles[num_height_tiles][num_width_tiles];

    constexpr int objectCount = 50;
    Container* objects[objectCount] = {};



    void render()
    {
        for (int ty = 0; ty < num_height_tiles; ty++) {
            for (int tx = 0; tx < num_width_tiles; tx++) {

                Tile& tile = screen_tiles[ty][tx];

                if (!tile.is_dirty)
                    continue;

                // 1. clear tile buffer
                render_sprite_tile.fillSprite(rgb888_to_rgb565(backgruond_color)); // background color

                int tileX = tile.x_pos;
                int tileY = tile.y_pos;

                int tileW = TILE_SIZE;
                int tileH = TILE_SIZE;

                
                
                
                // 2. draw all objects that intersect this tile
                for (int i = 0; i < objectCount; i++) {

                    Container* obj = objects[i];

                    if (obj == nullptr) continue;

                    // AABB intersection test
                    bool intersect =
                        !(obj->x + obj->w <= tileX ||
                        obj->x >= tileX + tileW ||
                        obj->y + obj->h <= tileY ||
                        obj->y >= tileY + tileH);

                    if (!intersect)
                        continue;
                    
                    int relative_pos_x = obj->x - tileX;
                    int relative_pos_y = obj->y - tileY;


                    obj->draw(&render_sprite_tile, relative_pos_x, relative_pos_y);
                }

   
                // 4. push tile to screen
                render_sprite_tile.pushSprite(tileX, tileY);

                // 5. mark clean
                tile.is_dirty = false;
            }
        }
    }


    void init(){
        tft.init();
        tft.setRotation(3);

        tft.fillScreen(rgb888_to_rgb565(backgruond_color));
        tft.setSwapBytes(true);

        render_sprite_tile.setColorDepth(16);
        render_sprite_tile.createSprite(TILE_SIZE, TILE_SIZE);
        render_sprite_tile.fillSprite(rgb888_to_rgb565(backgruond_color));

        

        
        for (int y = 0; y < num_height_tiles; y++) {
            for (int x = 0; x < num_width_tiles; x++) {

                screen_tiles[y][x].x_pos = x * TILE_SIZE;
                screen_tiles[y][x].y_pos = y * TILE_SIZE;

                screen_tiles[y][x].is_dirty = true;  // optional initial state
            }
        }

    }


    void loop(){
        render();
    }

}