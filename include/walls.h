#ifndef WALLS_H
#define WALLS_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

class walls
{
public:
    walls(const bn::fixed_point& top_left_position);

    void update(const bn::fixed_point& player_movement);

private:
    static constexpr int WALL_CELL_SIZE = 8; // <-- WHAT'S THE DIFFERENCE HERE?
    static constexpr int WALL_SPRITE_SIZE = 8; // <-- WHAT'S THE DIFFERENCE HERE?
    static constexpr int WALL_SPRITES_COUNT = 34; // <-- WHAT IS THIS?

    static constexpr int HORIZONTAL_WALL_INDEX = 0;
    static constexpr int VERTICAL_WALL_INDEX = 1;
    static constexpr int CONNECTION_WALL_INDEX = 2;
    static constexpr int WALL_COLUMNS = 10; // <-- REMOVE SOON
    static constexpr int WALL_ROWS = 9; // <-- REMOVE SOON

    bn::fixed_point _top_left_position;
    bn::vector<bn::sprite_ptr, WALL_SPRITES_COUNT> _sprites;

    void _add_wall(int column, int row, int graphics_index);
};

#endif