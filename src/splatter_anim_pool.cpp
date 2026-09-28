#include "splatter_anim_pool.h"

#include "bn_sprite_items_splatter.h"

namespace
{
    enum splatter_frame_index
    {
        SPLAT_0 = 0,
        SPLAT_1 = 1,
        SPLAT_2 = 2,
        SPLAT_3 = 3,
    };

    struct animation_frame
    {
        int sprite_index;
        int duration;
    };

    constexpr animation_frame animation_frames[] = {
        { splatter_frame_index::SPLAT_0, 2 },
        { splatter_frame_index::SPLAT_1, 2 },
        { splatter_frame_index::SPLAT_2, 3 },
        { splatter_frame_index::SPLAT_3, 3 },
    };
    constexpr int animation_frames_count = sizeof(animation_frames) / sizeof(animation_frames[0]);
}

splatter_anim_pool::splatter_anim_pool()
{
    // Instantiate splatter sprites in the pool.
    for (bn::optional<bn::sprite_ptr>& sprite : _sprites)
    {
        sprite.emplace(bn::sprite_items::splatter.create_sprite(0, 0, animation_frames[0].sprite_index));
        sprite->set_visible(false);
    }
}

void splatter_anim_pool::update(const bn::fixed_point& movement)
{
    for (int index = 0; index < _active_splatters_count; index++)
    {
        bn::sprite_ptr& sprite = *_sprites[index];

        // Update position of active splatter.
        sprite.set_position(sprite.position() - movement);

        // Update animation of active splatter.
        int& animation_index = _animation_indexes[index];
        if (animation_index < animation_frames_count)
        {
            int& animation_frame_end = _animation_frame_ends[index];
            animation_frame_end++;

            if (animation_frame_end >= animation_frames[animation_index].duration)
            {
                animation_frame_end = 0;
                animation_index++;

                if (animation_index < animation_frames_count)
                {
                    sprite.set_tiles(bn::sprite_items::splatter.tiles_item(),
                        animation_frames[animation_index].sprite_index);
                }
            }
        }
    }
}

void splatter_anim_pool::spawn(const bn::fixed_point& position, bn::fixed rotation_angle)
{
    const int index = _next_splatter_index;
    bn::sprite_ptr& sprite = *_sprites[index];

    // Set up the splatter sprite for spawning.
    sprite.set_position(position);
    sprite.set_tiles(bn::sprite_items::splatter.tiles_item(), animation_frames[0].sprite_index);
    sprite.set_rotation_angle_safe(rotation_angle);
    sprite.set_visible(true);
    _animation_indexes[index] = 0;
    _animation_frame_ends[index] = 0;

    // Bump active splatter count if not maxed yet.
    if (_active_splatters_count < MAX_SPLATTERS)
    {
        _active_splatters_count++;
    }
    _next_splatter_index = (_next_splatter_index + 1) % MAX_SPLATTERS;
}