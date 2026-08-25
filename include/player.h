#ifndef PLAYER_H
#define PLAYER_H

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"


class player
{
public:
    player();
    ~player();

    void update(bn::fixed_point movement);

private:
    bn::sprite_ptr _sprite;
};


#endif