#ifndef BRICK_H
#define BRICK_H


struct Brick
{
    float x, y;
    float w, h;
    float pv;
    u32 color;

    void draw() {
        C2D_DrawRectSolid(x, y, 0, w, h, color);
    }

};

#endif