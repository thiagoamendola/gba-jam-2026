#include "player.h"

#include "bn_math.h"
#include "bn_log.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"
#include "bn_sprite_ptr.h"
#include "bn_sound_items.h"

#include "base_enemy.h"
#include "exit_route.h"
#include "walls.h"
#include "constants.h"

#include "bn_sprite_items_hitbox.h"
#include "bn_sprite_items_pointer.h"
#include "bn_sprite_items_player.h"


player::player()
    : _hold_state(hold_state::MELEE),
      _sprite(bn::sprite_items::player.create_sprite(0, 0, MELEE_ANIM_FRAMES[0].sprite_index)),
      _exit_pointer_sprite(bn::sprite_items::pointer.create_sprite(0, 0)),
      _rotation_center_position(0, 0),
      _world_position(0, 0),
      _is_attacking(false),
      _is_dead(false),
      _attack_anim_index(0),
      _attack_anim_frame_end(0)
{
    _exit_pointer_sprite.set_scale(1.5);
}

player::~player()
{
}

void player::_update_exit_pointer(const exit_route& exit_route)
{
    const bn::fixed_point direction = exit_route.position() - _rotation_center_position;
    _exit_pointer_sprite.set_position(_rotation_center_position);
    _exit_pointer_sprite.set_visible(exit_route.is_available());

    if (direction.x() != 0 || direction.y() != 0)
    {
        _exit_pointer_sprite.set_rotation_angle_safe(
            bn::degrees_atan2(-direction.y().round_integer(), direction.x().round_integer()));
    }
}

const bn::fixed_point& player::position() const
{
    return _rotation_center_position;
}

const bn::fixed_point& player::world_position() const
{
    return _world_position;
}

void player::start_exit_transition()
{
    _transition_sprite_position = _sprite.position();
    _transition_sprite_horizontal_scale = _sprite.horizontal_scale();
    _transition_sprite_vertical_scale = _sprite.vertical_scale();
    _sprite.set_bg_priority(0);
    _exit_pointer_sprite.set_visible(false);

    if(_attack_hitbox_sprite)
    {
        _attack_hitbox_sprite->set_visible(false);
    }
}

void player::update_exit_transition(const bn::fixed_point& snapshot_position, bn::fixed scale)
{
    _sprite.set_position(_transition_sprite_position.safe_multiplication(scale) + snapshot_position);
    _sprite.set_scale(
            _transition_sprite_horizontal_scale.safe_multiplication(scale),
            _transition_sprite_vertical_scale.safe_multiplication(scale));
}

bn::fixed_point player::get_attack_hitbox_position() const
{
    const auto [sin, cos] = bn::degrees_sin_and_cos(_sprite.rotation_angle());
    const bn::fixed_point rotated_player_offset(
        PLAYER_SPRITE_OFFSET.x() * cos + PLAYER_SPRITE_OFFSET.y() * sin,
        -PLAYER_SPRITE_OFFSET.x() * sin + PLAYER_SPRITE_OFFSET.y() * cos);
    const bn::fixed_point rotated_attack_offset(
        ATTACK_COLLIDER_OFFSET.x() * cos + ATTACK_COLLIDER_OFFSET.y() * sin,
        -ATTACK_COLLIDER_OFFSET.x() * sin + ATTACK_COLLIDER_OFFSET.y() * cos);
    return _rotation_center_position - rotated_player_offset + rotated_attack_offset;
}

// <-- mAKE THIS MORE GENERIC
bool player::check_attack_collision(const base_enemy& enemy) const
{
    if (! _is_attacking)
    {
        return false;
    }

    const bn::fixed_point distance = enemy.position() - get_attack_hitbox_position();
    const bn::fixed collision_radius = ATTACK_COLLIDER_RADIUS + enemy.collider_radius();

    if (distance.x() * distance.x() + distance.y() * distance.y() <= collision_radius * collision_radius)
    {
        return true;
    }

    return false;
}

bn::fixed_point player::update(
    bn::fixed_point movement, const walls& walls, const exit_route& exit_route)
{
    // <-- TODO: Slightly pan camera towards the looking direction?

    // Check if attack input pressed.
    if (bn::keypad::a_pressed() && !_is_attacking)
    {
        _is_attacking = true;
        bn::sound_items::bat_swing.play();
        _attack_anim_index = 0;
        _attack_anim_frame_end = 0;
        _sprite.set_tiles(bn::sprite_items::player.tiles_item(), MELEE_ANIM_FRAMES[_attack_anim_index].sprite_index); // <-- IF MELEE ONLY
        if constexpr(SHOW_HITBOX_ATTACK)
        {
            _attack_hitbox_sprite.emplace(bn::sprite_items::hitbox.create_sprite(get_attack_hitbox_position()));
            _attack_hitbox_sprite->set_scale(ATTACK_COLLIDER_RADIUS / HITBOX_SPRITE_RADIUS);
        }
    }

    if (_is_attacking)
    {
        // Update attack animation frame.
        ++_attack_anim_frame_end;

        if (_attack_anim_frame_end >= MELEE_ANIM_FRAMES[_attack_anim_index].duration)
        {
            _attack_anim_frame_end = 0;
            _attack_anim_index++;

            if (_attack_anim_index >= MELEE_ANIM_COUNT)
            {
                // We finished the last animation frame. Go back to idle state.
                _is_attacking = false;
                _attack_anim_index = 0;
                _sprite.set_tiles(bn::sprite_items::player.tiles_item(), MELEE_ANIM_FRAMES[0].sprite_index);
                _attack_hitbox_sprite.reset();
            }
            else
            {
                _sprite.set_tiles(bn::sprite_items::player.tiles_item(), MELEE_ANIM_FRAMES[_attack_anim_index].sprite_index);
            }
        }
    }

    // Update player position with collision resolution.
    const bn::fixed_point requested_movement = movement;
    movement = walls.resolve_movement(_world_position, PLAYER_COLLIDER_RADIUS, requested_movement);
    _world_position += movement;

    // Rotate sprite based on movement direction.
    if (requested_movement.x() != 0 || requested_movement.y() != 0)
    {
        _sprite.set_rotation_angle_safe(
                bn::degrees_atan2(-requested_movement.y().data(), requested_movement.x().data()));
    }

    // Reposition sprite so rotation anchors into the sprite offset.
    const auto [sin, cos] = bn::degrees_sin_and_cos(_sprite.rotation_angle());
    const bn::fixed_point rotated_offset(
        PLAYER_SPRITE_OFFSET.x() * cos + PLAYER_SPRITE_OFFSET.y() * sin,
        - PLAYER_SPRITE_OFFSET.x() * sin + PLAYER_SPRITE_OFFSET.y() * cos);

    _sprite.set_position(_rotation_center_position - rotated_offset);

    if (_attack_hitbox_sprite)
    {
        _attack_hitbox_sprite->set_position(get_attack_hitbox_position());
    }

    _update_exit_pointer(exit_route);

    return movement;
}

void player::die()
{
#if !INVINCIBLE
    _is_dead = true;
    _is_attacking = false;
    _sprite.set_tiles(bn::sprite_items::player.tiles_item(), player_frame_index::IDLE);
#endif
}

bool player::is_dead() const
{
    return _is_dead;
}
