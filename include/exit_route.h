#ifndef EXIT_ROUTE_H
#define EXIT_ROUTE_H

#include <initializer_list>

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "easing.h"
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

    // A chunk of key animation for controlling position/scale transition over predefined duration.
    struct end_animation_phase
    {
        int duration_frames;
        bn::fixed_point end_position;
        bn::fixed end_scale;
        easing easing_method = easing::LINEAR;
    };

    // Definitions follow the class so omitted easing values keep the LINEAR default.
    static const end_animation_phase ZOOM_OUT_PHASE;
    static const end_animation_phase MOVE_SNAPSHOT_DOWN_PHASE;

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
    [[nodiscard]] static int _end_animation_duration();
    [[nodiscard]] transition_transform _end_animation_transform() const;
    [[nodiscard]] static transition_transform _interpolate_end_animation_phase(
            const transition_transform& start_transform, const end_animation_phase& phase, int frame);
    void _start_end_animation();
    void _update_end_animation_sprites(const transition_transform& transform);
};

// The snapshot begins at screen position { 0, 0 } with scale 1.
inline constexpr exit_route::end_animation_phase exit_route::ZOOM_OUT_PHASE = {
    90,
    bn::fixed_point(0, 30),
    bn::fixed(0.5),
    easing::EASE_IN_OUT
};

// This phase begins at ZOOM_OUT_PHASE's target. It defaults to LINEAR easing.
inline constexpr exit_route::end_animation_phase exit_route::MOVE_SNAPSHOT_DOWN_PHASE = {
    60,
    bn::fixed_point(0, 300),
    bn::fixed(0.4),
    easing::EASE_IN
};

#endif // EXIT_ROUTE_H