#ifndef DOG_ENEMY_H
#define DOG_ENEMY_H

#include "base_enemy.h"

class player;
class walls;

class dog_enemy : public base_enemy
{
public:
    static constexpr bn::fixed COLLIDER_RADIUS = 3;
    static constexpr bn::fixed DOG_WALK_SPEED = 1;
    static constexpr bn::fixed DOG_RUN_SPEED = 2;
    static constexpr bn::fixed SPOT_DISTANCE = 120;
    static constexpr bn::fixed SPOT_DISTANCE_SQUARED = SPOT_DISTANCE * SPOT_DISTANCE;
    static constexpr bn::fixed SPOT_HALF_ANGLE = 45;
    static constexpr int MAX_IDLE_LOCATIONS = 8;

    enum dog_frame_index
    {
        IDLE = 0,
        WALK_1 = 1,
        WALK_2 = 2,
        DEAD = 3,
    };

    static constexpr base_enemy::animation_frame WALK_ANIM_FRAMES[] = {
        { dog_frame_index::IDLE, 10 },
        { dog_frame_index::WALK_1, 12 },
        { dog_frame_index::IDLE, 10 },
        { dog_frame_index::WALK_2, 12 },
    };
    static constexpr int WALK_ANIM_COUNT = sizeof(WALK_ANIM_FRAMES) / sizeof(WALK_ANIM_FRAMES[0]);

    dog_enemy(
            player* player, const bn::fixed_point& position,
            std::initializer_list<bn::fixed_point> idle_locations);
    ~dog_enemy() override;

protected:
    void destroy() override;
    bn::fixed_point _update_pursue(const walls& walls) override;
};

#endif
