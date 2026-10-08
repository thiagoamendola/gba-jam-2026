#ifndef SPLATTER_ANIM_POOL_H
#define SPLATTER_ANIM_POOL_H

#include "bn_array.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

class splatter_anim_pool
{
public:
    static constexpr int MAX_SPLATTERS = 12;

    splatter_anim_pool();

    void update(const bn::fixed_point& movement);
    void spawn(const bn::fixed_point& position, bn::fixed rotation_angle);
    void hide_all();

private:
    bn::array<bn::optional<bn::sprite_ptr>, MAX_SPLATTERS> _sprites;
    bn::array<int, MAX_SPLATTERS> _animation_indexes = {};
    bn::array<int, MAX_SPLATTERS> _animation_frame_ends = {};
    int _active_splatters_count = 0;
    int _next_splatter_index = 0;
};

#endif