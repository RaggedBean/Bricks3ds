#ifndef PLAYER_H
#define PLAYER_H


struct Player
{
    float x, y;
    float w, h;
    float speed;
    int lifes;

    void draw() {
        C2D_DrawRectangle(x, y, 0, w, h,
                          C2D_Color32(0xC0, 0xC0, 0xC0, 0xFF),
                          C2D_Color32(0xC0, 0xC0, 0xC0, 0xFF),
                          C2D_Color32(0xC0, 0xC0, 0xC0, 0xFF),
                          C2D_Color32(0xC0, 0xC0, 0xC0, 0xFF));
    }

};

#endif

