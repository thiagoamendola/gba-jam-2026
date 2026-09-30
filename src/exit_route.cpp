#include "exit_route.h"

#include "bn_log.h"
#include "bn_music.h"
#include "bn_music_items.h"

#include "base_enemy.h"
#include "player.h"
#include "scenario.h"
#include "game_state.h"

#include "bn_sprite_items_exit.h"

exit_route::exit_route(
    player *player, game_state *game_state, scenario *associated_scenario, const bn::fixed_point &position,
    std::initializer_list<base_enemy *> enemies, scene_type next_scene)
    : _player(player), _game_state(game_state),
      _associated_scenario(associated_scenario),
      _position(position),
      _sprite(bn::sprite_items::exit.create_sprite(_position)),
      _sprite_animation(bn::create_sprite_animate_action_forever(
          _sprite, EXIT_ANIMATION_FRAME_DURATION, bn::sprite_items::exit.tiles_item(), 0, 1, 0, 2)),
      _next_scene(next_scene),
      _state(exit_state::DISABLED),
      _end_animation_frame(0)
{
    for (base_enemy *enemy : enemies)
    {
        _enemies.push_back(enemy);
    }

    _sprite.set_visible(false);
}

exit_route::~exit_route()
{
}

bn::optional<scene_type> exit_route::update(const bn::fixed_point &player_movement)
{
    if (_state == exit_state::READY || _state == exit_state::ANIMATING)
    {
        _sprite_animation.update();
    }

    switch (_state)
    {
    case exit_state::DISABLED:
        _position -= player_movement;
        _sprite.set_position(_position);

        if (_all_enemies_dead())
        {
            _state = exit_state::READY;
            _sprite.set_visible(true);
            bn::music_items::beyond_insidetherobot.play();
            _game_state->skip_music_start = false;
        }
        break;

    case exit_state::READY:
    {
        _position -= player_movement;
        _sprite.set_position(_position);

        const bn::fixed_point player_distance = _player->position() - _position;
        const bn::fixed player_distance_squared =
            player_distance.x() * player_distance.x() + player_distance.y() * player_distance.y();

        if (player_distance_squared < CLEAR_DISTANCE_SQUARED)
        {
            // Player touched exit. Start the exit transition.
            _associated_scenario->start_exit_transition();
            _start_end_animation();
            _state = exit_state::ANIMATING;
            _end_animation_frame = 0;
            bn::music::stop();
        }
        break;
    }

    case exit_state::ANIMATING:
        if (_end_animation_frame < _end_animation_duration())
        {
            const transition_transform transform = _end_animation_transform();

            if (_associated_scenario->update_exit_transition(transform.position, transform.scale))
            {
                _update_end_animation_sprites(transform);
                ++_end_animation_frame;
            }
        }
        else
        {
            _state = exit_state::DONE;
            BN_LOG("STAGE CLEARED");
            return _next_scene;
        }
        break;

    case exit_state::DONE:
        break;

    default:
        break;
    }

    return bn::nullopt;
}

bool exit_route::is_end_animation_playing() const
{
    return _state == exit_state::ANIMATING;
}

bool exit_route::is_available() const
{
    return _state == exit_state::READY;
}

const bn::fixed_point& exit_route::position() const
{
    return _position;
}

bool exit_route::_all_enemies_dead() const
{
    for (const base_enemy *enemy : _enemies)
    {
        if (!enemy->is_dead())
        {
            return false;
        }
    }

    return true;
}

int exit_route::_end_animation_duration()
{
    return ZOOM_OUT_PHASE.duration_frames + MOVE_SNAPSHOT_DOWN_PHASE.duration_frames;
}

exit_route::transition_transform exit_route::_end_animation_transform() const
{
    const transition_transform initial_transform = {
        bn::fixed_point(0, 0),
        bn::fixed(1)};

    if (_end_animation_frame < ZOOM_OUT_PHASE.duration_frames)
    {
        return _interpolate_end_animation_phase(initial_transform, ZOOM_OUT_PHASE, _end_animation_frame);
    }

    const transition_transform zoom_out_transform = {
        ZOOM_OUT_PHASE.end_position,
        ZOOM_OUT_PHASE.end_scale};
    const int move_down_frame = _end_animation_frame - ZOOM_OUT_PHASE.duration_frames;
    return _interpolate_end_animation_phase(
        zoom_out_transform, MOVE_SNAPSHOT_DOWN_PHASE, move_down_frame);
}

exit_route::transition_transform exit_route::_interpolate_end_animation_phase(
    const transition_transform &start_transform, const end_animation_phase &phase, int frame)
{
    if (phase.duration_frames <= 1)
    {
        return {phase.end_position, phase.end_scale};
    }

    const bn::fixed linear_progress = bn::fixed(frame).safe_division(phase.duration_frames - 1);
    const bn::fixed progress = apply_easing(linear_progress, phase.easing_method);
    return {
        start_transform.position +
            (phase.end_position - start_transform.position).safe_multiplication(progress),
        start_transform.scale +
            (phase.end_scale - start_transform.scale).safe_multiplication(progress)};
}

void exit_route::_start_end_animation()
{
    _transition_sprite = {
        _sprite.position(),
        _sprite.horizontal_scale(),
        _sprite.vertical_scale()};
    _sprite.set_bg_priority(0);
    _player->start_exit_transition();

    for (base_enemy *enemy : _enemies)
    {
        enemy->start_exit_transition();
    }
}

void exit_route::_update_end_animation_sprites(const transition_transform &transform)
{
    _sprite.set_position(_transition_sprite.position.safe_multiplication(transform.scale) + transform.position);
    _sprite.set_scale(
        _transition_sprite.horizontal_scale.safe_multiplication(transform.scale),
        _transition_sprite.vertical_scale.safe_multiplication(transform.scale));
    _player->update_exit_transition(transform.position, transform.scale);

    for (base_enemy *enemy : _enemies)
    {
        enemy->update_exit_transition(transform.position, transform.scale);
    }
}
