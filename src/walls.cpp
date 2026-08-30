#include "walls.h"

#include "utils.h"

#include "bn_math.h"
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

bn::fixed_point walls::resolve_movement(
    const bn::fixed_point& collider_position, bn::fixed collider_radius,
    const bn::fixed_point& movement) const
{
    const bn::fixed movement_x = _resolve_horizontal_movement(
            collider_position, collider_radius, movement.x());
    const bn::fixed_point position_after_x(collider_position.x() + movement_x, collider_position.y());
    const bn::fixed movement_y = _resolve_vertical_movement(
            position_after_x, collider_radius, movement.y());
    return bn::fixed_point(movement_x, movement_y);
}

bn::fixed walls::_resolve_horizontal_movement(
    const bn::fixed_point& collider_position, bn::fixed collider_radius, bn::fixed movement_x) const
{
    // Ignore the rest if no horizontal movement.
    if (movement_x == 0)
    {
        return 0;
    }

    const bn::fixed destination_x = collider_position.x() + movement_x;
    const bn::fixed radius_squared = collider_radius * collider_radius;
    bn::fixed resolved_movement_x = movement_x;

    // <-- OPTIMIZE THIS CHECKS???
    // Iterate through all wall cells to check for collisions.
    for(const wall_cell& cell : _cells)
    {
        const bn::fixed wall_left = cell.position.x();
        const bn::fixed wall_top = cell.position.y();
        const bn::fixed wall_right = wall_left + WALL_CELL_SIZE;
        const bn::fixed wall_bottom = wall_top + WALL_CELL_SIZE;

        // Skip this wall if y_distance is greater than collider radius.
        const bn::fixed closest_y = utils::clamp(collider_position.y(), wall_top, wall_bottom);
        const bn::fixed y_distance = collider_position.y() - closest_y;
        const bn::fixed y_distance_squared = y_distance * y_distance;
        if(y_distance_squared >= radius_squared)
        {
            continue;
        }

        // Considering a right triangle with the collider radius as the hypotenuse and vertical distance to wall as one leg,
        // the other leg is the horizontal distance between the collider center and the wall. The vertical leg is zero if the collider 
        // is in "the same line" as the wall, making the horizontal limit equal to the radius. Otherwise, the horizontal limit is reduced
        // according to the vertical distance.
        const bn::fixed horizontal_limit = bn::sqrt(radius_squared - y_distance_squared);

        // Handle either positive or negative horizontal movement.
        if(movement_x > 0)
        {
            // This is the limit position where collider center would contact the wall.
            const bn::fixed center_contact_x = wall_left - horizontal_limit;

            // If we're not touching the wall but will cross the contact point after this movement, we need to avoid that.
            if(collider_position.x() <= center_contact_x && destination_x > center_contact_x)
            {
                // If so, we'll limit the movement to the contact point. We use this min() so two adjacent walls, considering one has
                // |y_distance| > 0, doesn't cause us to move further when another wall with y_distance == 0 is encountered.
                resolved_movement_x = bn::min(resolved_movement_x, center_contact_x - collider_position.x());
            }
        }
        else
        {
            // This is the limit position where collider center would contact the wall.
            const bn::fixed center_contact_x = wall_right + horizontal_limit;

            // If we're not touching the wall but will cross the contact point after this movement, we need to avoid that.
            if(collider_position.x() >= center_contact_x && destination_x < center_contact_x)
            {
                // If so, we'll limit the movement to the contact point. We use this max() so two adjacent walls, considering one has
                // |y_distance| > 0, doesn't cause us to move further when another wall with y_distance == 0 is encountered.
                resolved_movement_x = bn::max(resolved_movement_x, center_contact_x - collider_position.x());
            }
        }
    }

    return resolved_movement_x;
}

bn::fixed walls::_resolve_vertical_movement(
        const bn::fixed_point& collider_position, bn::fixed collider_radius, bn::fixed movement_y) const
{
    // Ignore the rest if no vertical movement.
    if(movement_y == 0)
    {
        return 0;
    }

    const bn::fixed destination_y = collider_position.y() + movement_y;
    const bn::fixed radius_squared = collider_radius * collider_radius;
    bn::fixed resolved_movement_y = movement_y;

    // <-- OPTIMIZE THIS CHECKS???
    // Iterate through all wall cells to check for collisions.
    for(const wall_cell& cell : _cells)
    {
        const bn::fixed wall_left = cell.position.x();
        const bn::fixed wall_top = cell.position.y();
        const bn::fixed wall_right = wall_left + WALL_CELL_SIZE;
        const bn::fixed wall_bottom = wall_top + WALL_CELL_SIZE;

        // Skip this wall if x_distance is greater than collider radius.
        const bn::fixed closest_x = utils::clamp(collider_position.x(), wall_left, wall_right);
        const bn::fixed x_distance = collider_position.x() - closest_x;
        const bn::fixed x_distance_squared = x_distance * x_distance;
        if(x_distance_squared >= radius_squared)
        {
            continue;
        }

        // Considering a right triangle with the collider radius as the hypotenuse and horizontal distance to wall as one leg,
        // the other leg is the vertical distance between the collider center and the wall. The horizontal leg is zero if the collider 
        // is in "the same line" as the wall, making the vertical limit equal to the radius. Otherwise, the vertical limit is reduced
        // according to the horizontal distance.
        const bn::fixed vertical_limit = bn::sqrt(radius_squared - x_distance_squared);

        // Handle either positive or negative vertical movement.
        if(movement_y > 0)
        {
            // This is the limit position where collider center would contact the wall.
            const bn::fixed center_contact_y = wall_top - vertical_limit;

            // If we're not touching the wall but will cross the contact point after this movement, we need to avoid that.
            if(collider_position.y() <= center_contact_y && destination_y > center_contact_y)
            {
                // If so, we'll limit the movement to the contact point. We use this min() so two adjacent walls, considering one has
                // |x_distance| > 0, doesn't cause us to move further when another wall with x_distance == 0 is encountered.
                resolved_movement_y = bn::min(resolved_movement_y, center_contact_y - collider_position.y());
            }
        }
        else
        {
            // This is the limit position where collider center would contact the wall.
            const bn::fixed center_contact_y = wall_bottom + vertical_limit;

            // If we're not touching the wall but will cross the contact point after this movement, we need to avoid that.
            if(collider_position.y() >= center_contact_y && destination_y < center_contact_y)
            {
                // If so, we'll limit the movement to the contact point. We use this max() so two adjacent walls, considering one has
                // |x_distance| > 0, doesn't cause us to move further when another wall with x_distance == 0 is encountered.
                resolved_movement_y = bn::max(resolved_movement_y, center_contact_y - collider_position.y());
            }
        }
    }

    return resolved_movement_y;
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