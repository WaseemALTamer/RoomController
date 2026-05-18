#include <TFT_eSPI.h>
#include "structure.h"
#include "render_system.h"




namespace Objects{


    class Rect : public Container {
    public:
        Rect(int x, int y,
            int w, int h,
            uint32_t color)
            : Container(x, y, w, h),
            color(color)
        {
            markDirtyArea(x, y, w, h);
        }

        

        uint32_t color;

        void moveTo(int new_x, int new_y){
            
            markDirtyArea(x, y, w, h);

            markDirtyArea(new_x, new_y, w, h);
                
            x = new_x; 
            y = new_y;
        }

        void draw(TFT_eSprite* display, int new_x, int new_y){
            display->fillRect(new_x, new_y, w, h, RenderSystem::rgb888_to_rgb565(color));
        }


        void markDirtyArea(int x, int y, int w, int h)
        {
            int xStart = x / RenderSystem::TILE_SIZE;
            int yStart = y / RenderSystem::TILE_SIZE;
            int xEnd = (x + w) / RenderSystem::TILE_SIZE;
            int yEnd = (y + h) / RenderSystem::TILE_SIZE;

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