#include "bullet.h"

#include "bn_math.h"

#include "bn_sprite_items_bullet.h"
#include "player.h"
#include "walls.h"

bullet::bullet(
    const bn::fixed_point &position, const bn::fixed_point &world_position,
    bn::fixed rotation, player *player) : 
        _position(position),
        _world_position(world_position),
        _rotation(rotation),
        _player(player),
        _sprite(bn::sprite_items::bullet.create_sprite(position))
{
    _sprite.set_rotation_angle_safe(rotation);
}

bool bullet::update(const bn::fixed_point &player_movement, const walls &walls)
{
    // Subtract player's movement.
    _position -= player_movement;

    // Calculate movement from rotation.
    const auto [sin, cos] = bn::degrees_sin_and_cos(_rotation);
    const bn::fixed_point movement(cos * SPEED, -sin * SPEED);
    const bn::fixed_point next_position = _world_position + movement;

    // Check if collided with a wall.
    if (walls.has_wall_between(_world_position, next_position))
    {
        return false;
    }

    // Update position.
    _world_position = next_position;
    _position += movement;

    // Check if bullet hit player.
    const bn::fixed_point player_distance = _player->world_position() - _world_position;
    const bn::fixed player_distance_squared =
        player_distance.x() * player_distance.x() + player_distance.y() * player_distance.y();
    if (player_distance_squared < COLLISION_DISTANCE * COLLISION_DISTANCE)
    {
        _player->die();
        return false;
    }

    _sprite.set_position(_position);
    return true;
}
