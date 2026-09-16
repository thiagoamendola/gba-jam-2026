#ifndef RANGED_ENEMY_H
#define RANGED_ENEMY_H

#include "base_enemy.h"

class base_gameplay_scene;
class player;
class walls;

class ranged_enemy : public base_enemy
{
public:
    static constexpr bn::fixed COLLIDER_RADIUS = 3;
    static constexpr bn::fixed RANGED_WALK_SPEED = 1;
    static constexpr bn::fixed RANGED_RUN_SPEED = 2;
    static constexpr bn::fixed SPOT_DISTANCE = 95;
    static constexpr bn::fixed SPOT_HALF_ANGLE = 40;
    static constexpr bn::fixed TOUCH_DISTANCE = 18;
    static constexpr bn::fixed SHOOT_DISTANCE = 70;
    static constexpr int SHOOT_DELAY = 5;
    static constexpr int SHOOT_COOLDOWN = 20;
    static constexpr int MAX_IDLE_LOCATIONS = 8;

    enum ranged_frame_index
    {
        IDLE = 4,
        DEAD = 3,
    };

    static constexpr base_enemy::animation_frame WALK_ANIM_FRAMES[] = {
        { ranged_frame_index::IDLE, 10 },
    };
    static constexpr int WALK_ANIM_COUNT = sizeof(WALK_ANIM_FRAMES) / sizeof(WALK_ANIM_FRAMES[0]);

    ranged_enemy(
        base_gameplay_scene* scene, player* player, const bn::fixed_point& position,
        std::initializer_list<bn::fixed_point> idle_locations);
    ~ranged_enemy() override;

protected:
    void destroy() override;
    bn::fixed_point _update_pursue(const walls& walls) override;

private:
    base_gameplay_scene* _scene;
    bool _in_shoot_range = false;
    int _shoot_delay = 0;
    int _shoot_cooldown = 0;
};

#endif