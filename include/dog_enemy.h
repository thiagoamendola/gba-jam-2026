#ifndef DOG_ENEMY_H
#define DOG_ENEMY_H

#include <initializer_list>

#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

class player;
class walls;

class dog_enemy
{
public:
    static constexpr bn::fixed COLLIDER_RADIUS = 3;

    // Idle locations use world coordinates and are copied into this dog.
    dog_enemy(
            const player* player, const bn::fixed_point& position,
            std::initializer_list<bn::fixed_point> idle_locations);
    ~dog_enemy();

    void update(bn::fixed_point movement, const walls& walls);
    void destroy();

    [[nodiscard]] const bn::fixed_point& position() const;

private:
    enum class enemy_state
    {
        IDLE,
        PURSUE,
        DEAD,
    };

    enum dog_frame_index
    {
        IDLE = 0,
        WALK_1 = 1,
        WALK_2 = 2,
        DEAD = 3,
    };

    // <-- HOW CAN I GENERALIZE THIS?
    struct animation_frame
    {
        dog_frame_index sprite_index;
        int duration;
    };

    static constexpr bn::fixed DOG_SPEED = 1;
    static constexpr bn::fixed SPOT_DISTANCE = 200;
    static constexpr bn::fixed SPOT_DISTANCE_SQUARED = SPOT_DISTANCE * SPOT_DISTANCE;
    static constexpr bn::fixed SPOT_HALF_ANGLE = 45;
    static constexpr int MAX_IDLE_LOCATIONS = 8;

    static constexpr animation_frame WALK_ANIM_FRAMES[] = {
        { dog_frame_index::IDLE, 10 },
        { dog_frame_index::WALK_1, 12 },
        { dog_frame_index::IDLE, 10 },
        { dog_frame_index::WALK_2, 12 },
    };
    static constexpr int WALK_ANIM_COUNT = sizeof(WALK_ANIM_FRAMES) / sizeof(WALK_ANIM_FRAMES[0]);

    const player* _player;
    bn::fixed_point _position;
    bn::fixed_point _world_position;
    bn::sprite_ptr _sprite;
    enemy_state _state;
    bn::vector<bn::fixed_point, MAX_IDLE_LOCATIONS> _idle_locations;
    int _idle_location_index;

    int _walk_anim_index;
    int _walk_anim_frame_end;
};

#endif
