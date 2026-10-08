#ifndef EXIT_ROUTE_H
#define EXIT_ROUTE_H

#include <initializer_list>

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "game_state.h"
#include "scene_type.h"

class base_enemy;
class player;
class scenario;

class exit_route
{
public:
    exit_route(
        player* player, game_state* game_state, scenario* associated_scenario, const bn::fixed_point& position,
        std::initializer_list<base_enemy*> enemies, scene_type next_scene);
    ~exit_route();

    bn::optional<scene_type> update(const bn::fixed_point& player_movement);
    [[nodiscard]] bool is_end_animation_playing() const;
    [[nodiscard]] bool is_available() const;
    [[nodiscard]] const bn::fixed_point& position() const;

private:
    enum class exit_state
    {
        DISABLED,
        READY,
        ANIMATING,
        DONE,
    };

    static constexpr int MAX_ENEMIES = 32;
    static constexpr bn::fixed CLEAR_DISTANCE_SQUARED = 10 * 10;
    static constexpr int EXIT_ANIMATION_FRAME_COUNT = 4;
    static constexpr int EXIT_ANIMATION_FRAME_DURATION = 5;

    player* _player;
    game_state* _game_state;
    scenario* _associated_scenario;
    bn::vector<base_enemy*, MAX_ENEMIES> _enemies;

    bn::fixed_point _position;
    bn::sprite_ptr _sprite;
    bn::sprite_animate_action<EXIT_ANIMATION_FRAME_COUNT> _sprite_animation;
    scene_type _next_scene;
    exit_state _state;
    int _end_animation_frame;
    bool _transition_elements_hidden = false;

    [[nodiscard]] bool _all_enemies_dead() const;
    [[nodiscard]] static int _end_animation_duration();
    void _start_end_animation();
    void _hide_transition_elements();
};

#endif // EXIT_ROUTE_H