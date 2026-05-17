#include <TFT_eSPI.h>
#include "tick_system/transition_ease_in_out.h"
#include "constants.h"
#include <TAMC_GT911.h>

#define TOUCH_SDA 33
#define TOUCH_SCL 32
#define TOUCH_INT 21
#define TOUCH_RST 25

#define TOUCH_WIDTH 320
#define TOUCH_HEIGHT 480

TAMC_GT911 ts = TAMC_GT911(
  TOUCH_SDA,
  TOUCH_SCL,
  TOUCH_INT,
  TOUCH_RST,
  TOUCH_WIDTH,
  TOUCH_HEIGHT
);






TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

EaseInOut animation;

uint16_t lineBuffer[LCM_HEIGHT]; // global line buffer



template <typename T>
T myAbs(T x) {
    return (x < 0) ? -x : x;
}


template <typename T>
T myMax(T a, T b) {
    return (a > b) ? a : b;
}

template <typename T>
T myMin(T a, T b) {
    return (a < b) ? a : b;
}


uint16_t rgb888_to_rgb565(uint32_t c)
{
    uint8_t r = (c >> 16) & 0xFF;
    uint8_t g = (c >> 8)  & 0xFF;
    uint8_t b = c & 0xFF;

    return ((r & 0xF8) << 8) |
           ((g & 0xFC) << 3) |
           (b >> 3);
}



class Rect {
public:
    Rect(TFT_eSPI* display,
         int x, int y,
         int w, int h,
         uint32_t color,
         uint32_t bg)
    : tft(display),
      x(x), y(y),
      w(w), h(h),
      color(color),
      bg(bg),
      oldX(x), oldY(y) {}

    void draw() {
        tft->fillRect(oldX, oldY, w, h, rgb888_to_rgb565(color));
    }

    void eraseOld() {
        tft->fillRect(oldX, oldY, w, h, rgb888_to_rgb565(bg));
    }

    void moveTo(int newX, int newY)
    {
        // Save OLD position first (critical)
        int oldX = x;
        int oldY = y;

        // Compute union bounding box (dirty region)
        int minX = myMin(oldX, newX);
        int minY = myMin(oldY, newY);
        int maxX = myMax(oldX + w, newX + w);
        int maxY = myMax(oldY + h, newY + h);

        int width  = maxX - minX;
        int height = maxY - minY;

        // Loop over screen space region
        for (int py = minY; py < maxY; py++)
        {
            for (int px = minX; px < maxX; px++)
            {
                bool inOld = pointInRect(px, py, oldX, oldY, w, h);
                bool inNew = pointInRect(px, py, newX, newY, w, h);

                int idx = px - minX;

                if (inNew)
                {
                    lineBuffer[idx] = rgb888_to_rgb565(color);
                }
                else if (inOld)
                {
                    lineBuffer[idx] = rgb888_to_rgb565(bg);
                }
                else
                {
                    lineBuffer[idx] = rgb888_to_rgb565(bg);
                }
            }

            tft->pushImage(minX, py, width, 1, lineBuffer);
        }

        // Update state LAST (critical)
        x = newX;
        y = newY;
    }

    void setColor(uint16_t c) {
        color = c;
    }

    void setSize(int nw, int nh) {
        w = nw;
        h = nh;
    }


    bool rectOverlap(int x1, int y1, int w1, int h1,
                    int x2, int y2, int w2, int h2)
    {
        return !(x1 + w1 <= x2 ||   // left
                x2 + w2 <= x1 ||   // right
                y1 + h1 <= y2 ||   // above
                y2 + h2 <= y1);    // below
    }

    bool pointInRect(int px, int py,
                    int rx, int ry,
                    int rw, int rh)
    {
        return (px >= rx &&
                px <  rx + rw &&
                py >= ry &&
                py <  ry + rh);
    }

private:
    TFT_eSPI* tft;

    int x, y;
    int oldX, oldY;
    int w, h;

    uint32_t color;
    uint32_t bg;
};





Rect test_object(
    &tft, 
    0, 0,
    50, 50,
    0xffcc00,
    0x000000
);

Rect test_object2(
    &tft, 
    0, 0,
    50, 50,
    0xffcc00,
    0x000000
);



Rect test_object3(
    &tft, 
    0, 0,
    50, 50,
    0xffcc00,
    0x000000
);

Rect test_object4(
    &tft, 
    0, 0,
    50, 50,
    0xffcc00,
    0x000000
);

Rect test_object5(
    &tft, 
    0, 0,
    50, 50,
    0xffcc00,
    0x000000
);













int old_pos = 0;


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



void setup() {

    pinMode(27, OUTPUT);
    digitalWrite(27, HIGH);

    tft.init();
    tft.setRotation(3);

    tft.fillScreen(rgb888_to_rgb565(0x000000));
    tft.setSwapBytes(true);


    animation.duration = 3;
    animation.starting_value = 0;
    animation.ending_value = LCM_HEIGHT;
    animation.add_callback(on_animation_callback);
    animation.start_transition();


    

    Serial.begin(115200);
    Wire.begin(TOUCH_SDA, TOUCH_SCL);



    ts.begin();
    ts.setRotation(1); // adjust if orientation is wrong

    
}


void loop() {
    animation.update();


    ts.read();

    if (ts.isTouched) {
        for (int i = 0; i < ts.touches; i++) {
        Serial.print("X: ");
        Serial.print(ts.points[i].x);
        Serial.print(" Y: ");
        Serial.println(ts.points[i].y);
        }
    }
}