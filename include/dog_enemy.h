#ifndef DOG_ENEMY_H
#define DOG_ENEMY_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"

class player;

class dog_enemy
{
public:
    dog_enemy(const player* player, const bn::fixed_point& position);
    ~dog_enemy();

    void update(bn::fixed_point movement);

private:
    static constexpr bn::fixed DOG_SPEED = 1;

    const player* _player;
    bn::fixed_point _position;
    bn::sprite_ptr _sprite;
};

#endif
