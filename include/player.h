#ifndef PLAYER_H
#define PLAYER_H

#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"

class base_enemy;
class exit_route;
class walls;

class player
{
public:
    player();
    ~player();

    [[nodiscard]] bn::fixed_point update(
        bn::fixed_point movement, const walls& walls, const exit_route& exit_route);
        
    void start_exit_transition();
    void update_exit_transition(const bn::fixed_point& snapshot_position, bn::fixed scale);

    [[nodiscard]] const bn::fixed_point& position() const;
    [[nodiscard]] const bn::fixed_point& world_position() const;
    [[nodiscard]] bool check_attack_collision(const base_enemy& enemy) const;

    void die();
    [[nodiscard]] bool is_dead() const;

private:
    enum hold_state
    {
        BAREHANDS,
        MELEE,
    };

    enum player_frame_index
    {
        IDLE = 0,
        MELEE_1 = 1,
        MELEE_2 = 2,
        MELEE_3 = 3,
    };

    struct animation_frame
    {
        player_frame_index sprite_index;
        int duration;
    };

    static constexpr bn::fixed_point PLAYER_SPRITE_OFFSET = bn::fixed_point(-8, 0);
    static constexpr bn::fixed PLAYER_COLLIDER_RADIUS = 8;
    static constexpr bn::fixed_point ATTACK_COLLIDER_OFFSET = bn::fixed_point(5, -1);
    static constexpr bn::fixed ATTACK_COLLIDER_RADIUS = 14;
    static constexpr bn::fixed HITBOX_SPRITE_RADIUS = 5;
    static constexpr animation_frame MELEE_ANIM_FRAMES[] = {
        { player_frame_index::MELEE_1, 3 },
        { player_frame_index::MELEE_2, 4 },
        { player_frame_index::MELEE_3, 5 }
    };
    static constexpr int MELEE_ANIM_COUNT = sizeof(MELEE_ANIM_FRAMES) / sizeof(MELEE_ANIM_FRAMES[0]);

    hold_state _hold_state;
    bn::sprite_ptr _sprite;
    bn::sprite_ptr _exit_pointer_sprite;
    bn::optional<bn::sprite_ptr> _attack_hitbox_sprite;
    bn::fixed_point _rotation_center_position;
    bn::fixed_point _world_position;
    bool _is_attacking;
    bool _is_dead;
    int _attack_anim_index;
    int _attack_anim_frame_end;
    bn::fixed_point _transition_sprite_position;
    bn::fixed _transition_sprite_horizontal_scale;
    bn::fixed _transition_sprite_vertical_scale;

    [[nodiscard]] bn::fixed_point get_attack_hitbox_position() const;
    void _update_exit_pointer(const exit_route& exit_route);
};


#endif