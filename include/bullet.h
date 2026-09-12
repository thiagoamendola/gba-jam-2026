#ifndef BULLET_H
#define BULLET_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"

class player;
class walls;

class bullet
{
public:
    static constexpr bn::fixed SPEED = 8;
    static constexpr bn::fixed COLLISION_DISTANCE = 5;

    bullet(const bn::fixed_point &position, const bn::fixed_point &world_position,
        bn::fixed rotation, player *player);

    bool update(const bn::fixed_point &player_movement, const walls &walls);

private:
    bn::fixed_point _position;
    bn::fixed_point _world_position;
    bn::fixed _rotation;
    
    player *_player;
    bn::sprite_ptr _sprite;
};

#endif