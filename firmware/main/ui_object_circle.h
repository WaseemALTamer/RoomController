#include <TFT_eSPI.h>
#include "structure.h"
#include "render_system.h"



namespace Objects{


    class Circle : public Container {
    public:
        Circle(int x, int y,
            int r,
            uint32_t color)
            : Container(x, y, r * 2, r * 2),
            radius(r),
            color(color)
        {
            markDirtyArea(x, y, w, h);
        }

        uint32_t color;
        int radius;



        void draw(TFT_eSprite* display, int lx, int ly) {
            uint16_t c = RenderSystem::rgb888_to_rgb565(color);

            // draw circle centered inside its bounding box
            display->fillCircle(
                lx + radius,
                ly + radius,
                radius,
                RenderSystem::rgb888_to_rgb565(color)
            );

            //display->setTextDatum(MC_DATUM);
            //display->setTextColor(TFT_WHITE, TFT_BLACK);
            //display->drawString("WASEEM", lx + radius, ly + radius, 1);
            
        }


        bool isdragging = false;

        void on_press(TouchEvent e){
            Serial.println("pressed");
            isdragging = true;
        }

        void on_drag(TouchEvent e){
            move_to(e.x - radius, e.y - radius);
        }

        void on_rlease(TouchEvent e){
            Serial.println("rleased");
            isdragging = false;
        }


        void move_to(int new_x, int new_y) {

            markDirtyArea(x, y, w, h);
            markDirtyArea(new_x, new_y, w, h);

            x = new_x;
            y = new_y;
        }

        void markDirtyArea(int x, int y, int w, int h)
        {
            int xStart = x / RenderSystem::TILE_SIZE;
            int yStart = y / RenderSystem::TILE_SIZE;
            int xEnd   = (x + w) / RenderSystem::TILE_SIZE;
            int yEnd   = (y + h) / RenderSystem::TILE_SIZE;

            for (int ty = yStart; ty <= yEnd; ty++) {
                for (int tx = xStart; tx <= xEnd; tx++) {

                    if (tx >= 0 && tx < RenderSystem::num_width_tiles &&
                        ty >= 0 && ty < RenderSystem::num_height_tiles) {

                        RenderSystem::screen_tiles[ty][tx].is_dirty = true;
                    }
                }
            }
        }

    };



    
}