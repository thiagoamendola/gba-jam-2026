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

    [[nodiscard]] const bn::fixed_point& position() const;

private:
    enum class hold_state
    {
        BAREHANDS,
        MELEE,
    };

    struct animation_frame
    {
        int sprite_index;
        int duration;
    };

    static constexpr bn::fixed_point PLAYER_SPRITE_OFFSET = bn::fixed_point(-8, 0);
    static constexpr animation_frame MELEE_ANIM_FRAMES[] = { { 1, 3 }, { 2, 4 }, { 3, 5 } };
    static constexpr int MELEE_ANIM_COUNT = sizeof(MELEE_ANIM_FRAMES) / sizeof(MELEE_ANIM_FRAMES[0]);

    hold_state _hold_state;
    bn::sprite_ptr _sprite;
    bn::fixed_point _rotation_center_position;
    bool _is_attacking;
    int _attack_anim_index;
    int _attack_anim_frame_end;
};


#endif