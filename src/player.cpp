#include "player.h"

#include "bn_math.h"
#include "bn_sprite_items_player.h"

player::player()
    : _sprite(bn::sprite_items::player.create_sprite(0, 0))
{

}

player::~player()
{
}

void player::update(bn::fixed_point movement)
{
    // <-- TODO: Slightly pan camera towards the looking direction?
    // _sprite.set_position(_sprite.position() + movement);

    if (movement.x() != 0 || movement.y() != 0)
    {
        _sprite.set_rotation_angle_safe(bn::degrees_atan2(-movement.y().data(), movement.x().data()));
    }
}

