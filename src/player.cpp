#include "player.h"

#include "bn_math.h"
#include "bn_sprite_items_player.h"

namespace
{
    constexpr bn::fixed_point PLAYER_SPRITE_OFFSET(8, 0);
}

player::player()
    : _sprite(bn::sprite_items::player.create_sprite(0, 0)),
      _rotation_center_position(0, 0)
{

}

player::~player()
{
}

void player::update(bn::fixed_point movement)
{
    // <-- TODO: Slightly pan camera towards the looking direction?
    // _rotation_center_position += movement;

    // Rotate sprite based on movement direction.
    if (movement.x() != 0 || movement.y() != 0)
    {
        _sprite.set_rotation_angle_safe(bn::degrees_atan2(-movement.y().data(), movement.x().data()));
    }

    // Reposition sprite so rotation anchors into the sprite offset.
    const auto [sin, cos] = bn::degrees_sin_and_cos(_sprite.rotation_angle());
    const bn::fixed_point rotated_offset(
        PLAYER_SPRITE_OFFSET.x() * cos + PLAYER_SPRITE_OFFSET.y() * sin,
        - PLAYER_SPRITE_OFFSET.x() * sin + PLAYER_SPRITE_OFFSET.y() * cos);

    _sprite.set_position(_rotation_center_position + rotated_offset);
}

