#include "walls.h"

#include "bn_sprite_items_walls.h"

void walls::create_horizontal_wall(const bn::fixed_point& start_position, bn::fixed end_x)
{
    // Snap start position and end to nearest wall cell.
    const bn::fixed start_x = start_position.x() - (start_position.x() % WALL_CELL_SIZE);
    const bn::fixed start_y = start_position.y() - (start_position.y() % WALL_CELL_SIZE);
    end_x -= end_x % WALL_CELL_SIZE;
    const bn::fixed_point snapped_start_position(start_x, start_y);

    const int direction = end_x >= start_x ? WALL_CELL_SIZE : -WALL_CELL_SIZE;

    // Create first wall cell as a connection.
    _add_or_upgrade_wall(snapped_start_position, CONNECTION_WALL_INDEX);

    // Create intermediate horizontal wall cells.
    for(bn::fixed x = start_x + direction;
        direction > 0 ? x < end_x : x > end_x;
        x += direction)
    {
        _add_or_upgrade_wall(bn::fixed_point(x, start_y), HORIZONTAL_WALL_INDEX);
    }

    // Create last wall cell as a connection.
    _add_or_upgrade_wall(bn::fixed_point(end_x, start_y), CONNECTION_WALL_INDEX);
}

void walls::create_vertical_wall(const bn::fixed_point& start_position, bn::fixed end_y)
{
    // Snap start position and end to nearest wall cell.
    const bn::fixed start_x = start_position.x() - (start_position.x() % WALL_CELL_SIZE);
    const bn::fixed start_y = start_position.y() - (start_position.y() % WALL_CELL_SIZE);
    end_y -= end_y % WALL_CELL_SIZE;
    const bn::fixed_point snapped_start_position(start_x, start_y);

    const int direction = end_y >= start_y ? WALL_CELL_SIZE : -WALL_CELL_SIZE;

    // Create first wall cell as a connection.
    _add_or_upgrade_wall(snapped_start_position, CONNECTION_WALL_INDEX);

    // Create intermediate vertical wall cells.
    for(bn::fixed y = start_y + direction;
        direction > 0 ? y < end_y : y > end_y;
        y += direction)
    {
        _add_or_upgrade_wall(bn::fixed_point(start_x, y), VERTICAL_WALL_INDEX);
    }

    // Create last wall cell as a connection.
    _add_or_upgrade_wall(bn::fixed_point(start_x, end_y), CONNECTION_WALL_INDEX);
}

void walls::update(const bn::fixed_point& player_movement)
{
    // Move according to player.
    for(wall_cell& cell : _cells)
    {
        cell.sprite.set_position(cell.sprite.position() - player_movement);
    }
}

void walls::_add_or_upgrade_wall(const bn::fixed_point& position, int graphics_index)
{
    // <-- We check all cells so O(n). We can make this O(1) if needed, likely with a hashmap.
    for(wall_cell& cell : _cells)
    {
        if(cell.position == position)
        {
            cell.sprite.set_tiles(bn::sprite_items::walls.tiles_item(), CONNECTION_WALL_INDEX);
            return;
        }
    }

    bn::sprite_ptr sprite = bn::sprite_items::walls.create_sprite(
            position.x() + WALL_SPRITE_HALF_SIZE, position.y() + WALL_SPRITE_HALF_SIZE, graphics_index);
    _cells.push_back(wall_cell{ position, bn::move(sprite) });
}