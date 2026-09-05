#ifndef EXIT_ROUTE_H
#define EXIT_ROUTE_H

#include <initializer_list>

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "scene_type.h"

class dog_enemy;
class player;
class scenario;

class exit_route
{
public:
    exit_route(
        player* player, scenario* associated_scenario, const bn::fixed_point& position,
        std::initializer_list<dog_enemy*> dogs, scene_type next_scene);
    ~exit_route();

    bn::optional<scene_type> update(const bn::fixed_point& player_movement);
    [[nodiscard]] bool is_end_animation_playing() const;

    // A phase interpolates from the previous phase target to this position and scale.
    // Adjust the target and duration to tune movement speed and zoom speed together.
    struct end_animation_phase
    {
        int duration_frames;
        bn::fixed_point end_position;
        bn::fixed end_scale;
    };

    // The snapshot begins at screen position { 0, 0 } with scale 1.
    static constexpr end_animation_phase ZOOM_OUT_PHASE = {
        180,
        bn::fixed_point(0, 30),
        bn::fixed(0.5)
    };

    // This phase begins at ZOOM_OUT_PHASE's target. Change end_scale to zoom while moving.
    static constexpr end_animation_phase MOVE_SNAPSHOT_DOWN_PHASE = {
        60,
        bn::fixed_point(0, 300),
        bn::fixed(0.4)
    };

private:
    struct transition_transform
    {
        bn::fixed_point position;
        bn::fixed scale;
    };

    struct transition_sprite_data
    {
        bn::fixed_point position;
        bn::fixed horizontal_scale;
        bn::fixed vertical_scale;
    };

    enum class exit_state
    {
        DISABLED,
        READY,
        ANIMATING,
        DONE,
    };

    static constexpr int MAX_DOGS = 8;
    static constexpr bn::fixed CLEAR_DISTANCE_SQUARED = 10 * 10;

    player* _player;
    scenario* _associated_scenario;
    bn::vector<dog_enemy*, MAX_DOGS> _dogs;

    bn::fixed_point _position;
    bn::sprite_ptr _sprite;
    transition_sprite_data _transition_sprite;
    scene_type _next_scene;
    exit_state _state;
    int _end_animation_frame;

    [[nodiscard]] bool _all_enemies_dead() const;
    [[nodiscard]] static constexpr int _end_animation_duration()
    {
        return ZOOM_OUT_PHASE.duration_frames + MOVE_SNAPSHOT_DOWN_PHASE.duration_frames;
    }
    [[nodiscard]] transition_transform _end_animation_transform() const;
    [[nodiscard]] static transition_transform _interpolate_end_animation_phase(
            const transition_transform& start_transform, const end_animation_phase& phase, int frame);
    void _start_end_animation();
    void _update_end_animation_sprites(const transition_transform& transform);
};

#endif // EXIT_ROUTE_H