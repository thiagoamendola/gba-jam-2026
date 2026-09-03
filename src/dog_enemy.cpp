#include "dog_enemy.h"

#include "bn_math.h"
#include "bn_sprite_ptr.h"

#include "player.h"
#include "walls.h"

#include "bn_sprite_items_dog.h"

dog_enemy::dog_enemy(
                const player* player, const bn::fixed_point& position,
                std::initializer_list<bn::fixed_point> idle_locations)
    : _player(player),
      _position(position),
      _world_position(position),
      _sprite(bn::sprite_items::dog.create_sprite(_position)),
      _state(enemy_state::IDLE),
      _idle_location_index(0),
      _walk_anim_index(0),
      _walk_anim_frame_end(0)
{
    for(const bn::fixed_point& idle_location : idle_locations)
    {
        _idle_locations.push_back(idle_location);
    }
}

dog_enemy::~dog_enemy()
{
}

bool dog_enemy::is_dead() const
{
    return _state == enemy_state::DEAD;
}

const bn::fixed_point& dog_enemy::position() const
{
    return _position;
}

void dog_enemy::destroy()
{
    _state = enemy_state::DEAD;
    _sprite.set_tiles(bn::sprite_items::dog.tiles_item(), dog_frame_index::DEAD);
    _sprite.put_below();
    _sprite.set_rotation_angle_safe(_sprite.rotation_angle() + 90);
}

void dog_enemy::update(bn::fixed_point player_movement, const walls& walls)
{
    // Subtract player's movement.
    _position -= player_movement;

    if (_state == enemy_state::DEAD)
    {
        _sprite.set_position(_position);
        return;
    }

    bn::fixed_point movement;

    if(_state == enemy_state::IDLE)
    {
        const bn::fixed_point player_direction = _player->world_position() - _world_position;
        const bn::fixed player_distance_squared =
                player_direction.x() * player_direction.x() + player_direction.y() * player_direction.y();

        // Check if player is in range of detection.
        if(player_distance_squared < SPOT_DISTANCE_SQUARED)
        {
            // Now check if the player is within the dog's field of view.
            const bn::fixed player_angle = bn::degrees_atan2(
                    -player_direction.y().round_integer(), player_direction.x().round_integer());
            bn::fixed angle_difference = bn::safe_degrees_angle(player_angle - _sprite.rotation_angle());

            if(angle_difference > 180)
            {
                angle_difference -= 360;
            }

            // If so, final check is to verify if any walls between the dog and the player block the line of sight.
            if(bn::abs(angle_difference) < SPOT_HALF_ANGLE &&
               !walls.has_wall_between(_world_position, _player->world_position()))
            {
                _state = enemy_state::PURSUE;
            }
        }
    }

    if (_state == enemy_state::IDLE && !_idle_locations.empty())
    {
        const bn::fixed_point& target_location = _idle_locations[_idle_location_index];
        const bn::fixed_point direction = target_location - _world_position;
        const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        if(distance > 0)
        {
            // Move exactly to a nearby target so a wall cannot be skipped by waypoint cycling.
            movement = distance <= DOG_SPEED ? direction : (direction / distance) * DOG_SPEED;
            movement = walls.resolve_movement(_world_position, COLLIDER_RADIUS, movement);
            _world_position += movement;
            _position += movement;
        }

        if(_world_position == target_location)
        {
            _idle_location_index = (_idle_location_index + 1) % _idle_locations.size();
        }
    }
    else if(_state == enemy_state::PURSUE)
    {
        // Move towards player.
        const bn::fixed_point direction = _player->position() - _position;
        const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        if (distance > 0)
        {
            movement = (direction / distance) * DOG_SPEED;
        }

        movement = walls.resolve_movement(_world_position, COLLIDER_RADIUS, movement);
        _world_position += movement;
        _position += movement;
    }

    // A wall blocks attacks between the player and the dog.
    if (_player->check_attack_collision(*this) &&
        !walls.has_wall_between(_player->world_position(), _world_position))
    {
        destroy();
        return;
    }

    // Rotate towards movement direction.
    if (movement.x() != 0 || movement.y() != 0)
    {
        _sprite.set_rotation_angle_safe(bn::degrees_atan2(-movement.y().data(), movement.x().data()));
    }

    _sprite.set_position(_position);

    // Update sprite animation.
    ++_walk_anim_frame_end;

    if (_walk_anim_frame_end >= WALK_ANIM_FRAMES[_walk_anim_index].duration)
    {
        _walk_anim_frame_end = 0;
        _walk_anim_index = (_walk_anim_index + 1) % WALK_ANIM_COUNT;
        _sprite.set_tiles(bn::sprite_items::dog.tiles_item(), WALK_ANIM_FRAMES[_walk_anim_index].sprite_index);
    }
}