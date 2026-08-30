#include "dog_enemy.h"

#include "bn_math.h"
#include "bn_sprite_ptr.h"

#include "player.h"

#include "bn_sprite_items_dog.h"

dog_enemy::dog_enemy(const player* player, const bn::fixed_point& position)
    : _player(player),
      _position(position),
    _sprite(bn::sprite_items::dog.create_sprite(_position)),
      _state(enemy_state::PURSUE),
      _walk_anim_index(0),
      _walk_anim_frame_end(0)
{
}

dog_enemy::~dog_enemy()
{
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
    _sprite.set_rotation_angle_safe(90);
}

void dog_enemy::update(bn::fixed_point player_movement)
{
    // Subtract player's movement.
    _position -= player_movement;

    if (_state == enemy_state::DEAD)
    {
        _sprite.set_position(_position);
        return;
    }

    bn::fixed_point movement;

    if (_state == enemy_state::PURSUE)
    {
        // Move towards player.
        const bn::fixed_point direction = _player->position() - _position;
        const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

        if (distance > 0)
        {
            movement = (direction / distance) * DOG_SPEED;
        }

        _position += movement;
    }

    // Check for collision with player's attack hitbox.
    if (_player->check_attack_collision(*this)) // <-- MAKE HITBOX USAGE MORE ROBUST
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