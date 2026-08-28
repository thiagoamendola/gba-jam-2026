#include "player.h"

#include "bn_math.h"
#include "bn_log.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"

#include "dog_enemy.h"

#include "bn_sprite_items_hitbox.h"
#include "bn_sprite_items_player.h"


player::player()
    : _hold_state(hold_state::MELEE),
            _sprite(bn::sprite_items::player.create_sprite(0, 0, MELEE_ANIM_FRAMES[0].sprite_index)),
      _rotation_center_position(0, 0),
      _is_attacking(false),
            _attack_anim_index(0),
            _attack_anim_frame_end(0)
{

}

player::~player()
{
}

const bn::fixed_point& player::position() const
{
    return _rotation_center_position;
}

bn::fixed_point player::_attack_hitbox_position() const
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
bool player::check_attack_collision(const dog_enemy& dog) const
{
    if (! _is_attacking)
    {
        return false;
    }

    const bn::fixed_point distance = dog.position() - _attack_hitbox_position();
    const bn::fixed collision_radius = ATTACK_COLLIDER_RADIUS + dog_enemy::COLLIDER_RADIUS;

    if (distance.x() * distance.x() + distance.y() * distance.y() <= collision_radius * collision_radius)
    {
        return true;
    }

    return false;
}

void player::update(bn::fixed_point movement)
{
    // <-- TODO: Slightly pan camera towards the looking direction?
    
    
    // Check if attack input pressed.
    if (bn::keypad::a_pressed() && !_is_attacking)
    {
        _is_attacking = true;
        _attack_anim_index = 0;
        _attack_anim_frame_end = 0;
        _sprite.set_tiles(bn::sprite_items::player.tiles_item(), MELEE_ANIM_FRAMES[_attack_anim_index].sprite_index); // <-- IF MELEE ONLY
#if SHOW_HITBOX_ATTACK
        _attack_hitbox_sprite.emplace(bn::sprite_items::hitbox.create_sprite(_attack_hitbox_position()));
        _attack_hitbox_sprite->set_scale(ATTACK_COLLIDER_RADIUS / HITBOX_SPRITE_RADIUS);
#endif
    }

    if (_is_attacking)
    {
        // Update attack animation frame.
        ++_attack_anim_frame_end;

        if (_attack_anim_frame_end >= MELEE_ANIM_FRAMES[_attack_anim_index].duration)
        {
            _attack_anim_frame_end = 0;
            ++_attack_anim_index;

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

    // Rotate sprite based on movement direction.
    if (movement.x() != 0 || movement.y() != 0)
    {
        _sprite.set_rotation_angle_safe(bn::degrees_atan2(-movement.y().data(), movement.x().data()));
    }

    // Reposition sprite so rotation anchors into the sprite offset.
    const auto [sin, cos] = bn::degrees_sin_and_cos(_sprite.rotation_angle());
    const bn::fixed_point rotated_offset(
        PLAYER_SPRITE_OFFSET.x() * cos + PLAYER_SPRITE_OFFSET.y() * sin,
        - PLAYER_SPRITE_OFFSET.x() * sin + PLAYER_SPRITE_OFFSET.y() * cos);

    _sprite.set_position(_rotation_center_position - rotated_offset);

    if (_attack_hitbox_sprite)
    {
        _attack_hitbox_sprite->set_position(_attack_hitbox_position());
    }
}

