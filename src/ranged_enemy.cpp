#include "ranged_enemy.h"

#include "bn_math.h"
#include "bn_sound_items.h"

#include "base_gameplay_scene.h"
#include "player.h"
#include "walls.h"

#include "bn_sprite_items_robot.h"

ranged_enemy::ranged_enemy(
        base_gameplay_scene* scene, player* player, const bn::fixed_point& position,
        std::initializer_list<bn::fixed_point> idle_locations) :
    base_enemy(
            player, position, idle_locations, bn::sprite_items::robot,
            COLLIDER_RADIUS, RANGED_WALK_SPEED, RANGED_RUN_SPEED, SPOT_DISTANCE, SPOT_HALF_ANGLE,
            bn::span<const base_enemy::animation_frame>(WALK_ANIM_FRAMES, WALK_ANIM_COUNT)),
    _scene(scene)
{
}

ranged_enemy::~ranged_enemy()
{
}

void ranged_enemy::destroy()
{
    _state = enemy_state::DEAD;
    _sprite.set_tiles(_sprite_item.tiles_item(), ranged_frame_index::DEAD);
    _sprite.put_below();
    _sprite.set_rotation_angle_safe(_sprite.rotation_angle() + 90);
    _start_splatter_animation();
}

bn::fixed_point ranged_enemy::_update_pursue(const walls& walls)
{
    const bn::fixed_point direction = _player->world_position() - _world_position;
    const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    // If  player is too far away, enemy should get closer.
    if (distance > SHOOT_DISTANCE)
    {
        bn::fixed_point movement;
        if (distance > 0)
        {
            movement = (direction / distance) * _run_speed;
        }

        movement = walls.resolve_movement(_world_position, _collider_radius, movement);
        _world_position += movement;
        _position += movement;
        return movement;
    }

    // Rotate enemy towards player.
    const bn::fixed rotation = bn::degrees_atan2(-direction.y().round_integer(), direction.x().round_integer());
    _sprite.set_rotation_angle_safe(rotation);

    // Start shooting process if player just got into shooting range.
    if (!_in_shoot_range)
    {
        _in_shoot_range = true;
        _shoot_delay = SHOOT_DELAY;
    }

    // If shoot delay is not over, keep moving.
    if (_shoot_delay > 0)
    {
        --_shoot_delay;

        const bn::fixed movement_distance = distance > 0 ? distance : 1;
        bn::fixed_point movement = (direction / movement_distance) * _walk_speed;
        movement = walls.resolve_movement(_world_position, _collider_radius, movement);
        _world_position += movement;
        _position += movement;
        return movement;
    }

    if (_shoot_cooldown > 0)
    {
        --_shoot_cooldown;
    }

    // If cooldown is over, shoot a bullet!
    if (_shoot_cooldown <= 0)
    {
        bn::sound_items::silencer.play();
        _scene->create_bullet(_position, _world_position, rotation);
        _shoot_cooldown = SHOOT_COOLDOWN;
    }

    return bn::fixed_point();
}