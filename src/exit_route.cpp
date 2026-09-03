#include "exit_route.h"

#include "bn_log.h"

#include "dog_enemy.h"
#include "player.h"

#include "bn_sprite_items_exit.h"

exit_route::exit_route(
        const player* player, const bn::fixed_point& position,
        std::initializer_list<const dog_enemy*> dogs)
    : _player(player),
      _position(position),
      _sprite(bn::sprite_items::exit.create_sprite(_position)),
      _stage_cleared(false)
{
    for(const dog_enemy* dog : dogs)
    {
        _dogs.push_back(dog);
    }

    _sprite.set_visible(_all_dogs_dead());
}

exit_route::~exit_route()
{
}

void exit_route::update(const bn::fixed_point& player_movement)
{
    _position -= player_movement;
    _sprite.set_position(_position);

    const bool visible = _all_dogs_dead();
    _sprite.set_visible(visible);

    if(!visible || _stage_cleared)
    {
        return;
    }

    const bn::fixed_point player_distance = _player->position() - _position;
    const bn::fixed player_distance_squared =
            player_distance.x() * player_distance.x() + player_distance.y() * player_distance.y();

    if(player_distance_squared < CLEAR_DISTANCE_SQUARED)
    {
        BN_LOG("STAGE CLEARED");
        _stage_cleared = true;
    }
}

bool exit_route::_all_dogs_dead() const
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