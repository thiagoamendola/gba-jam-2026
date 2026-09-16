#ifndef MELEE_ENEMY_H
#define MELEE_ENEMY_H

#include "base_enemy.h"

class player;
class walls;

class melee_enemy : public base_enemy
{
public:
    static constexpr bn::fixed COLLIDER_RADIUS = 3;
    static constexpr bn::fixed MELEE_WALK_SPEED = 1;
    static constexpr bn::fixed MELEE_RUN_SPEED = 2;
    static constexpr bn::fixed SPOT_DISTANCE = 95;
    static constexpr bn::fixed SPOT_HALF_ANGLE = 40;
    static constexpr bn::fixed TOUCH_DISTANCE = 18;
    static constexpr int MAX_IDLE_LOCATIONS = 8;

    enum melee_frame_index
    {
        IDLE = 0,
        ATTACK_1 = 1,
        ATTACK_2 = 2,
        DEAD = 3,
    };

    static constexpr base_enemy::animation_frame WALK_ANIM_FRAMES[] = {
        { melee_frame_index::IDLE, 10 },
    };
    static constexpr base_enemy::animation_frame ATTACK_ANIM_FRAMES[] = {
        { melee_frame_index::IDLE, 3 },
        { melee_frame_index::ATTACK_1, 4 },
        { melee_frame_index::ATTACK_2, 5 },
    };
    static constexpr int WALK_ANIM_COUNT = sizeof(WALK_ANIM_FRAMES) / sizeof(WALK_ANIM_FRAMES[0]);
    static constexpr int ATTACK_ANIM_COUNT = sizeof(ATTACK_ANIM_FRAMES) / sizeof(ATTACK_ANIM_FRAMES[0]);

    melee_enemy(
            player* player, const bn::fixed_point& position,
            std::initializer_list<bn::fixed_point> idle_locations);
    ~melee_enemy() override;

protected:
    void destroy() override;
    bn::fixed_point _update_pursue(const walls& walls) override;
    void _update_animation() override;

    bool _is_attacking = false;
    int _attack_anim_index = 0;
    int _attack_anim_frame_end = 0;
};

#endif