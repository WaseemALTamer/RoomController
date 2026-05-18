#include <TFT_eSPI.h>
#include "tick_system/transition_ease_in_out.h"
#include "constants.h"
#include <TAMC_GT911.h>






TFT_eSPI tft = TFT_eSPI();
uint32_t backgruond_color = 0x000000;


int TILE_SIZE = 32;


TFT_eSprite* render_sprite_tile;

const int num_width_tiles = 15;
const int num_height_tiles = 10;




EaseInOut animation;


struct Tile{
    int x_pos;
    int y_pos;

    bool is_dirty;
};

Tile screen_tiles[num_height_tiles][num_width_tiles];




uint16_t rgb888_to_rgb565(uint32_t c)
{
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8)  & 0xFF;
    uint8_t b = c & 0xFF;

    return ((r & 0xF8) << 8) |
           ((g & 0xFC) << 3) |
           (b >> 3);
}



class Container{
public:
    int x, y, w, h;

    Container(int x, int y, int w, int h)
        : x(x), y(y), w(w), h(h) {}

    virtual void draw(TFT_eSprite* display, int new_x, int new_y) = 0;
};




class Rect : public Container {
public:
    Rect(int x, int y,
        int w, int h,
        uint32_t color)
        : Container(x, y, w, h),
        color(color)
    {}
    uint32_t color;

    void moveTo(int new_x, int new_y){
        
        markDirtyArea(x, y, w, h);

        markDirtyArea(new_x, new_y, w, h);
            
        x = new_x; 
        y = new_y;
    }

    void draw(TFT_eSprite* display, int new_x, int new_y){
        display->fillRect(new_x, new_y, w, h, rgb888_to_rgb565(color));
    }


    void markDirtyArea(int x, int y, int w, int h)
    {
        int xStart = x / TILE_SIZE;
        int yStart = y / TILE_SIZE;
        int xEnd   = (x + w) / TILE_SIZE;
        int yEnd   = (y + h) / TILE_SIZE;

        for (int ty = yStart; ty <= yEnd; ty++) {
            for (int tx = xStart; tx <= xEnd; tx++) {

                if (tx >= 0 && tx < num_width_tiles &&
                    ty >= 0 && ty < num_height_tiles) {

                    screen_tiles[ty][tx].is_dirty = true;
                }
            }
        }
    }
    
    
};




class Circle : public Container {
public:
    Circle(int x, int y,
           int r,
           uint32_t color)
        : Container(x, y, r * 2, r * 2),
          radius(r),
          color(color)
    {}

    uint32_t color;
    int radius;

    void moveTo(int new_x, int new_y) {

        markDirtyArea(x, y, w, h);
        markDirtyArea(new_x, new_y, w, h);

        x = new_x;
        y = new_y;
    }

    void draw(TFT_eSprite* display, int lx, int ly) {
        uint16_t c = rgb888_to_rgb565(color);

        // draw circle centered inside its bounding box
        display->fillCircle(
            lx + radius,
            ly + radius,
            radius,
            c
        );
    }

    void markDirtyArea(int x, int y, int w, int h)
    {
        int xStart = x / TILE_SIZE;
        int yStart = y / TILE_SIZE;
        int xEnd   = (x + w) / TILE_SIZE;
        int yEnd   = (y + h) / TILE_SIZE;

        for (int ty = yStart; ty <= yEnd; ty++) {
            for (int tx = xStart; tx <= xEnd; tx++) {

                if (tx >= 0 && tx < num_width_tiles &&
                    ty >= 0 && ty < num_height_tiles) {

                    screen_tiles[ty][tx].is_dirty = true;
                }
            }
        }
    }
};



Rect test_object(
    0, 0,
    50, 50,
    0xffcc00
);

Rect test_object2(
    0, 0,
    50, 50,
    0xffcc00
);

Rect test_object3(
    0, 0,
    50, 50,
    0xffcc00
);

Rect test_object4(
    0, 0,
    50, 50,
    0xffcc00
);


Circle test_object5(
    0, 0,
    25,
    0xff00cc
);


Container* objects[] = {
    &test_object,
    &test_object2,
    &test_object3,
    &test_object4,
    &test_object5
};

int objectCount = 5;



void on_animation_callback(float value, void* ctx){

    test_object.moveTo(value, 0);
    test_object2.moveTo(0, value/LCM_HEIGHT * LCM_WIDTH );

    test_object3.moveTo(value, value/LCM_HEIGHT * LCM_WIDTH );


    test_object4.moveTo(100, value/LCM_HEIGHT * LCM_WIDTH);
    test_object5.moveTo(value, 250);


    if(!animation.uniform.is_running){
        if (value == 0){
            animation.start_transition();
        }else{
            animation.reverse_transition();
        }
    }
}



void render()
{
    for (int ty = 0; ty < num_height_tiles; ty++) {
        for (int tx = 0; tx < num_width_tiles; tx++) {

            Tile& tile = screen_tiles[ty][tx];

            if (!tile.is_dirty)
                continue;

            // 1. clear tile buffer
            render_sprite_tile->fillSprite(rgb888_to_rgb565(backgruond_color)); // background color

            int tileX = tile.x_pos;
            int tileY = tile.y_pos;

            int tileW = TILE_SIZE;
            int tileH = TILE_SIZE;

            
            
            
            // 2. draw all objects that intersect this tile
            for (int i = 0; i < objectCount; i++) {

                Container* obj = objects[i];

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


                obj->draw(render_sprite_tile, relative_pos_x, relative_pos_y);
            }


            

            
            // 4. push tile to screen
            render_sprite_tile->pushSprite(tileX, tileY);

            // 5. mark clean
            tile.is_dirty = false;
        }
    }
}


void setup() {



    Serial.begin(115200);

    tft.init();
    tft.setRotation(3);

    tft.fillScreen(rgb888_to_rgb565(backgruond_color));
    tft.setSwapBytes(true);

    render_sprite_tile = new TFT_eSprite(&tft);
    render_sprite_tile->createSprite(TILE_SIZE, TILE_SIZE);


    animation.duration = 3;
    animation.starting_value = 0;
    animation.ending_value = LCM_HEIGHT;
    animation.add_callback(on_animation_callback);
    animation.start_transition();


    // initate the tiles
    for (int y = 0; y < num_height_tiles; y++) {
        for (int x = 0; x < num_width_tiles; x++) {

            screen_tiles[y][x].x_pos = x * TILE_SIZE;
            screen_tiles[y][x].y_pos = y * TILE_SIZE;

            screen_tiles[y][x].is_dirty = true;  // optional initial state
        }
    }

    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH);
}


void loop() {
    animation.update();
    render();
}