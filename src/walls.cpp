#include "walls.h"

#include "bn_sprite_items_walls.h"

walls::walls(const bn::fixed_point& top_left_position) :
    _top_left_position(top_left_position)
{
    for(int column = 1; column < WALL_COLUMNS - 1; ++column)
    {
        _add_wall(column, 0, HORIZONTAL_WALL_INDEX);
        _add_wall(column, WALL_ROWS - 1, HORIZONTAL_WALL_INDEX);
    }

    for(int row = 1; row < WALL_ROWS - 1; ++row)
    {
        _add_wall(0, row, VERTICAL_WALL_INDEX);
        _add_wall(WALL_COLUMNS - 1, row, VERTICAL_WALL_INDEX);
    }

    _add_wall(0, 0, CONNECTION_WALL_INDEX);
    _add_wall(WALL_COLUMNS - 1, 0, CONNECTION_WALL_INDEX);
    _add_wall(0, WALL_ROWS - 1, CONNECTION_WALL_INDEX);
    _add_wall(WALL_COLUMNS - 1, WALL_ROWS - 1, CONNECTION_WALL_INDEX);
}

void walls::update(const bn::fixed_point& player_movement)
{
    _top_left_position -= player_movement;

    for(bn::sprite_ptr& sprite : _sprites)
    {
        sprite.set_position(sprite.position() - player_movement);
    }
}

void walls::_add_wall(int column, int row, int graphics_index)
{
    const bn::fixed x = _top_left_position.x() + (column * WALL_CELL_SIZE) + (WALL_SPRITE_SIZE / 2);
    const bn::fixed y = _top_left_position.y() + (row * WALL_CELL_SIZE) + (WALL_SPRITE_SIZE / 2);
    _sprites.push_back(bn::sprite_items::walls.create_sprite(x, y, graphics_index));
}