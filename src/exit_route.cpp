#include "exit_route.h"

#include "bn_log.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_sound_items.h"

#include "base_enemy.h"
#include "end_transition_manager.h"
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
            bn::sound_items::gong.play();
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
            if (_associated_scenario->update_exit_transition())
            {
                if (! _transition_elements_hidden && _associated_scenario->exit_transition_fade_in_complete())
                {
                    _hide_transition_elements();
                    _transition_elements_hidden = true;
                }

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
    return end_transition_manager::FULL_ANIMATION_FRAMES;
}

void exit_route::_start_end_animation()
{
    _transition_elements_hidden = false;
}

void exit_route::_hide_transition_elements()
{
    _sprite.set_visible(false);
    _player->start_exit_transition();

    for (base_enemy *enemy : _enemies)
    {
        enemy->start_exit_transition();
    }
}
