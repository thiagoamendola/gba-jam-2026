#include "exit_route.h"

#include "bn_log.h"

#include "dog_enemy.h"
#include "player.h"
#include "scenario.h"

#include "bn_sprite_items_exit.h"

exit_route::exit_route(
    player* player, scenario* associated_scenario, const bn::fixed_point& position,
    std::initializer_list<dog_enemy*> dogs, scene_type next_scene)
    : _player(player),
      _associated_scenario(associated_scenario),
      _position(position),
      _sprite(bn::sprite_items::exit.create_sprite(_position)),
      _next_scene(next_scene),
      _state(exit_state::DISABLED),
      _end_animation_frame(0)
{
    for(dog_enemy* dog : dogs)
    {
        _dogs.push_back(dog);
    }

    _sprite.set_visible(false);
}

exit_route::~exit_route()
{
}

bn::optional<scene_type> exit_route::update(const bn::fixed_point& player_movement)
{
    switch(_state)
    {
    case exit_state::DISABLED:
        _position -= player_movement;
        _sprite.set_position(_position);

        if (_all_enemies_dead())
        {
            _state = exit_state::READY;
            _sprite.set_visible(true);
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
        }
        break;
    }

    case exit_state::ANIMATING:
        if (_end_animation_frame < END_ANIMATION_FRAMES)
        {
            const bn::fixed scale = _end_animation_scale();

            if (_associated_scenario->update_exit_transition(scale))
            {
                _update_end_animation_sprites(scale);
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

bool exit_route::_all_enemies_dead() const
{
    for(const dog_enemy* dog : _dogs)
    {
        if(!dog->is_dead())
        {
            return false;
        }
    }

    return true;
}

// <-- Let's improve this
bn::fixed exit_route::_end_animation_scale() const
{
    return 1 - (bn::fixed(_end_animation_frame) / ((END_ANIMATION_FRAMES - 1) * 2));
}

void exit_route::_start_end_animation()
{
    _transition_sprite = {
        _sprite.position(),
        _sprite.horizontal_scale(),
        _sprite.vertical_scale()
    };
    _sprite.set_bg_priority(0);
    _player->start_exit_transition();

    for(dog_enemy* dog : _dogs)
    {
        dog->start_exit_transition();
    }
}

void exit_route::_update_end_animation_sprites(bn::fixed scale)
{
    _sprite.set_position(_transition_sprite.position.safe_multiplication(scale));
    _sprite.set_scale(
            _transition_sprite.horizontal_scale.safe_multiplication(scale),
            _transition_sprite.vertical_scale.safe_multiplication(scale));
    _player->update_exit_transition(scale);

    for(dog_enemy* dog : _dogs)
    {
        dog->update_exit_transition(scale);
    }
}