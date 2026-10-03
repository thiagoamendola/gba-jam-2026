#include "melee_enemy.h"

#include "bn_math.h"
#include "bn_sound_items.h"

#include "player.h"
#include "walls.h"

#include "bn_sprite_items_robot.h"

melee_enemy::melee_enemy(
    base_gameplay_scene* scene, player* player, const bn::fixed_point& position,
        std::initializer_list<bn::fixed_point> idle_locations) :
    base_enemy(
        scene, player, position, idle_locations, bn::sprite_items::robot,
            COLLIDER_RADIUS, MELEE_WALK_SPEED, MELEE_RUN_SPEED, SPOT_DISTANCE, SPOT_HALF_ANGLE,
            TOUCH_DISTANCE, bn::span<const base_enemy::animation_frame>(WALK_ANIM_FRAMES, WALK_ANIM_COUNT))
{
}

melee_enemy::~melee_enemy()
{
}

void melee_enemy::destroy()
{
    _state = enemy_state::DEAD;
    _sprite.set_tiles(_sprite_item.tiles_item(), melee_frame_index::DEAD);
    _sprite.put_below();
    _sprite.set_rotation_angle_safe(_sprite.rotation_angle() + 90);
}

bn::fixed_point melee_enemy::_update_pursue(const walls& walls)
{
    // Move towards player.
    const bn::fixed_point direction = _player->position() - _position;
    const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    // Initiate attack if in range.
    if (distance < ATTACK_RANGE && !_is_attacking)
    {
        _is_attacking = true;
        bn::sound_items::bat_swing.play();
        _attack_anim_index = 0;
        _attack_anim_frame_end = 0;
        _sprite.set_tiles(_sprite_item.tiles_item(), ATTACK_ANIM_FRAMES[0].sprite_index);
    }

    // If attack animation finishing and player still in range of attack, kill player.
    if (_attack_anim_index >= ATTACK_ANIM_COUNT && distance < 10)
    {
        _is_attacking = false;
        _player->die();
        return bn::fixed_point();
    }

    // Not touching player yet, so move towards them.
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

void melee_enemy::_update_animation()
{
    if (!_is_attacking)
    {
        base_enemy::_update_animation();
        return;
    }

    ++_attack_anim_frame_end;

    if (_attack_anim_frame_end >= ATTACK_ANIM_FRAMES[_attack_anim_index].duration)
    {
        _attack_anim_frame_end = 0;
        ++_attack_anim_index;

        if (_attack_anim_index >= ATTACK_ANIM_COUNT)
        {
            _is_attacking = false;
            _player->die();
        }
        else
        {
            _sprite.set_tiles(_sprite_item.tiles_item(), ATTACK_ANIM_FRAMES[_attack_anim_index].sprite_index);
        }
    }
}