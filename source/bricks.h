#ifndef BRICKS_H
#define BRICKS_H

#include <vector>
#include "brick.h"


struct Bricks
{
    int row;
    int col;
    float top_left_pos_x;
    float top_left_pos_y;
    float w;
    float h;
    int padding_w;
    int padding_h;

    std::vector<Brick> current_level_bricks;
};




#endif

