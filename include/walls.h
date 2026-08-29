#ifndef WALLS_H
#define WALLS_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

class walls
{
public:
    walls() = default;

    void create_horizontal_wall(const bn::fixed_point& start_position, bn::fixed end_x);
    void create_vertical_wall(const bn::fixed_point& start_position, bn::fixed end_y);

    void update(const bn::fixed_point& player_movement);

private:
    struct wall_cell
    {
        bn::fixed_point position;
        bn::sprite_ptr sprite;
    };

    enum wall_graphics_index
    {
        HORIZONTAL_WALL_INDEX,
        VERTICAL_WALL_INDEX,
        CONNECTION_WALL_INDEX,
    };

    static constexpr int WALL_CELL_SIZE = 8;
    static constexpr int WALL_SPRITE_HALF_SIZE = 4;
    static constexpr int MAX_WALL_CELLS = 128;

    bn::vector<wall_cell, MAX_WALL_CELLS> _cells;

    void _add_or_upgrade_wall(const bn::fixed_point& position, int graphics_index);
};

#endif