#include "dog_enemy.h"

#include "bn_math.h"

#include "player.h"

#include "bn_sprite_items_dog.h"

dog_enemy::dog_enemy(const player* player, const bn::fixed_point& position)
    : _player(player),
      _position(position),
      _sprite(bn::sprite_items::dog.create_sprite(_position))
{
}

dog_enemy::~dog_enemy()
{
}

void dog_enemy::update(bn::fixed_point player_movement)
{
    // Subtract player's movement.
    _position -= player_movement;

    // Move towards player.
    const bn::fixed_point direction = _player->position() - _position;
    const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());
    bn::fixed_point movement;
    if (distance > 0)
    {
        movement = (direction / distance) * DOG_SPEED;
    }

    _position += movement;

    // Rotate towards movement direction.
    if (movement.x() != 0 || movement.y() != 0)
    {
        _sprite.set_rotation_angle_safe(bn::degrees_atan2(-movement.y().data(), movement.x().data()));
    }

    _sprite.set_position(_position);
}