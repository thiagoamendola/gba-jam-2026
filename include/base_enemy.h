#ifndef BASE_ENEMY_H
#define BASE_ENEMY_H

#include <initializer_list>

#include "bn_fixed_point.h"
#include "bn_span.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

class player;
class walls;

class base_enemy
{
public:
    struct animation_frame
    {
        int sprite_index;
        int duration;
    };

    base_enemy(
        player *player, const bn::fixed_point &position,
        std::initializer_list<bn::fixed_point> idle_locations,
        const bn::sprite_item &sprite_item, bn::fixed collider_radius,
        bn::fixed walk_speed, bn::fixed run_speed, bn::fixed spot_distance,
        bn::fixed spot_half_angle, bn::span<const animation_frame> walk_animation_frames);
    virtual ~base_enemy() = 0;

    void update(bn::fixed_point movement, const walls &walls);
    virtual void destroy() = 0;

    void start_exit_transition();
    void update_exit_transition(const bn::fixed_point &snapshot_position, bn::fixed scale);

    [[nodiscard]] bool is_dead() const;
    [[nodiscard]] const bn::fixed_point &position() const;
    [[nodiscard]] bn::fixed collider_radius() const;

protected:
    virtual bn::fixed_point _update_pursue(const walls &walls) = 0;
    virtual void _update_animation();

    enum class enemy_state
    {
        IDLE,
        PURSUE,
        DEAD,
    };

    static constexpr int MAX_IDLE_LOCATIONS = 8;

    player *_player;
    bn::fixed_point _position;
    bn::fixed_point _world_position;
    bn::fixed _collider_radius;
    bn::fixed _run_speed;
    const bn::sprite_item &_sprite_item;
    bn::sprite_ptr _sprite;
    bn::fixed _walk_speed;
    bn::fixed _spot_distance_squared;
    bn::fixed _spot_half_angle;
    bn::span<const animation_frame> _walk_animation_frames;
    enemy_state _state;
    bn::vector<bn::fixed_point, MAX_IDLE_LOCATIONS> _idle_locations;
    int _idle_location_index;
    int _walk_anim_index;
    int _walk_anim_frame_end;

    bn::fixed_point _transition_sprite_position;
    bn::fixed _transition_sprite_horizontal_scale;
    bn::fixed _transition_sprite_vertical_scale;
};

#endif // BASE_ENEMY_H
