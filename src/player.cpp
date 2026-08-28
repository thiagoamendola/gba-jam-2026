#include "player.h"

#include "bn_math.h"
#include "bn_sprite_items_player.h"
#include "bn_log.h"
#include "bn_fixed_point.h"
#include "bn_keypad.h"

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
}

