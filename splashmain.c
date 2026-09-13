#include"types.h"
#include"x86.h"

const int WINDOW_WIDTH = 320;
const int WINDOW_HEIGHT = 200;

const int image_buf__start = 0x000A0000;
const int image_buf__end = 0x00A0000 + (WINDOW_WIDTH * WINDOW_HEIGHT);

void draw_pixel(int x, int y, int color) {
    int pos = y * WINDOW_WIDTH + x;
    int image_buf__ptr = image_buf__start + pos;

    *((char*) image_buf__ptr) = color;
}

void splashmain() {

    // REPLACE THIS INFINITE LOOP WITH YOUR CODE

    int res = 2;



    int image_buf__ptr = 0x000A0000;

    int color = 0x03;

    while (image_buf__ptr < image_buf__end) {
        *((char*) image_buf__ptr) = color;

        image_buf__ptr++;
    }

    draw_pixel(320 / 2, 200 / 2, 0x0f);
}
