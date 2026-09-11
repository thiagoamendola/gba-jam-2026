#include "dog_enemy.h"

#include "bn_math.h"

#include "player.h"
#include "walls.h"

#include "bn_sprite_items_dog.h"

dog_enemy::dog_enemy(
        player* player, const bn::fixed_point& position,
        std::initializer_list<bn::fixed_point> idle_locations) :
    base_enemy(
            player, position, idle_locations, bn::sprite_items::dog,
            COLLIDER_RADIUS, DOG_WALK_SPEED, DOG_RUN_SPEED, SPOT_DISTANCE, SPOT_HALF_ANGLE,
            bn::span<const base_enemy::animation_frame>(WALK_ANIM_FRAMES, WALK_ANIM_COUNT))
{
}

dog_enemy::~dog_enemy()
{
}

void dog_enemy::destroy()
{
    _state = enemy_state::DEAD;
    _sprite.set_tiles(_sprite_item.tiles_item(), dog_frame_index::DEAD);
    _sprite.put_below();
    _sprite.set_rotation_angle_safe(_sprite.rotation_angle() + 90);
    _start_splatter_animation();
}

bn::fixed_point dog_enemy::_update_pursue(const walls& walls)
{
    // Move towards player.
    const bn::fixed_point direction = _player->position() - _position;
    const bn::fixed distance = bn::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (distance < 5)
    {
        // Dog touched player.
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
