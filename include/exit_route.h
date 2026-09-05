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

private:
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
    static constexpr int END_ANIMATION_FRAMES = 300;

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
    [[nodiscard]] bn::fixed _end_animation_scale() const;
    void _start_end_animation();
    void _update_end_animation_sprites(bn::fixed scale);
};

#endif // EXIT_ROUTE_H