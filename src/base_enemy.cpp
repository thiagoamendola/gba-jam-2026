#include "base_enemy.h"

#include "bn_math.h"
#include "bn_log.h"
#include "bn_random.h"
#include "bn_sound_items.h"

#include "constants.h"
#include "player.h"
#include "walls.h"

#include "bn_sprite_items_splatter.h"

base_enemy::base_enemy (
    player *player, const bn::fixed_point &position,
    std::initializer_list<bn::fixed_point> idle_locations,
    const bn::sprite_item &sprite_item, bn::fixed collider_radius,
    bn::fixed walk_speed, bn::fixed run_speed, bn::fixed spot_distance,
    bn::fixed spot_half_angle, bn::fixed touch_distance,
    bn::span<const animation_frame> walk_animation_frames) : 
        _player(player),
        _position(position),
        _world_position(position),
        _collider_radius(collider_radius),
        _run_speed(run_speed),
        _sprite_item(sprite_item),
        _sprite(sprite_item.create_sprite(_position)),
        _walk_speed(walk_speed),
        _spot_distance_squared(spot_distance * spot_distance),
        _touch_distance_squared(touch_distance * touch_distance),
        _spot_half_angle(spot_half_angle),
        _walk_animation_frames(walk_animation_frames),
        _state(enemy_state::IDLE),
        _idle_location_index(0),
        _walk_anim_index(0),
        _walk_anim_frame_end(0),
        _splatter_anim_index(0),
        _splatter_anim_frame_end(0),
        _splatter_position_offset(0, 0)
{

    for (const bn::fixed_point &idle_location : idle_locations)
    {
        _idle_locations.push_back(idle_location);
    }
}

base_enemy::~base_enemy()
{
}

bool base_enemy::is_dead() const
{
    return _state == enemy_state::DEAD;
}

const bn::fixed_point &base_enemy::position() const
{
    return _position;
}

bn::fixed base_enemy::collider_radius() const
{
    return _collider_radius;
}

void base_enemy::update(bn::fixed_point player_movement, const walls &walls)
{
    // Subtract player's movement.
    _position -= player_movement;

    // If dead, update position and splatter animation.
    if (_state == enemy_state::DEAD)
    {
        _sprite.set_position(_position);
        _splatter_sprite->set_position(_position + _splatter_position_offset);

        // Animate splatter.
        if (_splatter_anim_index < SPLATTER_ANIM_COUNT)
        {
            ++_splatter_anim_frame_end;

            if (_splatter_anim_frame_end >= SPLATTER_ANIM_FRAMES[_splatter_anim_index].duration)
            {
                _splatter_anim_frame_end = 0;
                ++_splatter_anim_index;

                if (_splatter_anim_index < SPLATTER_ANIM_COUNT)
                {
                    _splatter_sprite->set_tiles(
                        bn::sprite_items::splatter.tiles_item(),
                        SPLATTER_ANIM_FRAMES[_splatter_anim_index].sprite_index);
                }
            }
        }
        return;
    }

    bn::fixed_point movement;

    if (_state == enemy_state::IDLE)
    {
        const bn::fixed_point player_direction = _player->world_position() - _world_position;
        const bn::fixed player_distance_squared =
            player_direction.x() * player_direction.x() + player_direction.y() * player_direction.y();

        // Check if player is in range of detection.
        if (player_distance_squared < _spot_distance_squared)
        {
            // Now check if the player is within the enemy's field of view.
            const bn::fixed player_angle = bn::degrees_atan2(
                -player_direction.y().round_integer(), player_direction.x().round_integer());
            bn::fixed angle_difference = bn::safe_degrees_angle(player_angle - _sprite.rotation_angle());

            if (angle_difference > 180)
            {
                angle_difference -= 360;
            }

            // If so, final check is to verify if any walls between the enemy and the player block the line of sight.
            if (bn::abs(angle_difference) < _spot_half_angle &&
                !walls.has_wall_between(_world_position, _player->world_position()))
            {
#if !INVISIBLE
                _state = enemy_state::PURSUE;
#endif
            }
        }

        // Check if player is too close so it's touching the enemy, which should also trigger it.
        BN_LOG("Player distance squared: ", player_distance_squared, " Touch distance squared: ", _touch_distance_squared);
        if (player_distance_squared < _touch_distance_squared)
        {
#if !INVISIBLE
            _state = enemy_state::PURSUE;
#endif
        }
    }

    if (_state == enemy_state::IDLE && !_idle_locations.empty())
    {
        // Move towards the next idle location.
        const bn::fixed_point &target_location = _idle_locations[_idle_location_index];
        const bn::fixed_point direction = target_location - _world_position;
        const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        if (distance > 0)
        {
            // Move either the full distance for the frame or the remaining distance to the target.
            movement = distance <= _walk_speed ? direction : (direction / distance) * _walk_speed;
            // Ensure walls are handled before updating position.
            movement = walls.resolve_movement(_world_position, _collider_radius, movement);
            _world_position += movement;
            _position += movement;
        }

        // If current idle location reached, move to the next one.
        if (_world_position == target_location)
        {
            _idle_location_index = (_idle_location_index + 1) % _idle_locations.size();
        }
    }
    else if (_state == enemy_state::PURSUE)
    {
        // Enemies will have customized pursue behavior.
        movement = _update_pursue(walls);
    }

    // Check if player attacks hit and are not blocked by walls.
    if (_player->check_attack_collision(*this) &&
        !walls.has_wall_between(_player->world_position(), _world_position))
    {
        static bn::random random;

        if (random.get_int(2) == 0)
        {
            bn::sound_items::breaksound.play();
        }
        else
        {
            bn::sound_items::breaksound2.play();
        }

        destroy();
        return;
    }

    // Rotate towards movement direction.
    if (movement.x() != 0 || movement.y() != 0)
    {
        _sprite.set_rotation_angle_safe(bn::degrees_atan2(-movement.y().data(), movement.x().data()));
    }

    _sprite.set_position(_position);

    _update_animation();
}

void base_enemy::_update_animation()
{
    // Update sprite animation.
    ++_walk_anim_frame_end;

    if (_walk_animation_frames.size() &&
        _walk_anim_frame_end >= _walk_animation_frames[_walk_anim_index].duration)
    {
        _walk_anim_frame_end = 0;
        _walk_anim_index = (_walk_anim_index + 1) % _walk_animation_frames.size();
        _sprite.set_tiles(
            _sprite_item.tiles_item(), _walk_animation_frames[_walk_anim_index].sprite_index);
    }
}

void base_enemy::start_exit_transition()
{
    _transition_sprite_position = _sprite.position();
    _transition_sprite_horizontal_scale = _sprite.horizontal_scale();
    _transition_sprite_vertical_scale = _sprite.vertical_scale();
    _sprite.set_bg_priority(0);
}

void base_enemy::update_exit_transition(const bn::fixed_point &snapshot_position, bn::fixed scale)
{
    _sprite.set_position(_transition_sprite_position.safe_multiplication(scale) + snapshot_position);
    _sprite.set_scale(
        _transition_sprite_horizontal_scale.safe_multiplication(scale),
        _transition_sprite_vertical_scale.safe_multiplication(scale));
}

void base_enemy::_start_splatter_animation()
{
    const bn::fixed_point direction = _player->world_position() - _world_position;
    const bn::fixed direction_angle =
        bn::degrees_atan2(-direction.y().round_integer(), direction.x().round_integer()); // Y goes down.

    // Get the opposite distance direction.
    const bn::fixed offset_angle = direction_angle + 180; 
    // Rotaton offset for splatter sprite.
    const bn::fixed sprite_angle = direction_angle + 225; 

    // Calculate splatter offset position.
    const auto [sin, cos] = bn::degrees_sin_and_cos(offset_angle);
    _splatter_position_offset = bn::fixed_point(cos * SPLATTER_OFFSET, -sin * SPLATTER_OFFSET);

    // Create sprite at right position and rotation.
    _splatter_sprite.emplace(bn::sprite_items::splatter.create_sprite(
        _position + _splatter_position_offset, SPLATTER_ANIM_FRAMES[0].sprite_index));
    _splatter_sprite->set_rotation_angle_safe(sprite_angle);
    
    // Reset animation variables.
    _splatter_anim_index = 0;
    _splatter_anim_frame_end = 0;
}