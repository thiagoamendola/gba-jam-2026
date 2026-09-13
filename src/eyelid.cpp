#include "eyelid.h"

#include "bn_sprites.h"

#include "easing.h"

eyelid::eyelid(const bn::sprite_item& tile_item, int duration_frames) :
    _sprites(),
    _duration_frames(duration_frames),
    _current_frame(0),
    _travel_distance(144)
{
    // Upper lid
    for (int row = 0; row < 3; row++)
    {
        for (int column = 0; column < TILE_COLUMNS; ++column)
        {
            const bn::fixed x = -96 + column * 64;
            const bn::fixed y = -32 - row * 64;
            bn::sprite_ptr sprite = tile_item.create_sprite(x, y);
            sprite.set_z_order(bn::sprites::min_z_order());
            _sprites.push_back(bn::move(sprite));
        }
    }

    // Lower lid
    for (int row = 0; row < 3; row++)
    {
        for (int column = 0; column < TILE_COLUMNS; ++column)
        {
            const bn::fixed x = -96 + column * 64;
            const bn::fixed y = 32 + row * 64;
            bn::sprite_ptr sprite = tile_item.create_sprite(x, y);
            sprite.set_z_order(bn::sprites::min_z_order());
            _sprites.push_back(bn::move(sprite));
        }
    }
}

void eyelid::update()
{
    if (_sprites.empty())
    {
        return;
    }

    if (_current_frame < _duration_frames)
    {
        ++_current_frame;
    }

    const bn::fixed progress = bn::fixed(_current_frame) / _duration_frames;
    const bn::fixed movement = _travel_distance * apply_easing(progress, easing::EASE_IN);

    for (int index = 0; index < TILES_PER_LID; ++index)
    {
        // Upper lid
        _sprites[index].set_y(-32 - (index / TILE_COLUMNS) * 64 - movement);
        // Lower lid
        _sprites[index + TILES_PER_LID].set_y(
            32 + (index / TILE_COLUMNS) * 64 + movement);
    }

    if (_current_frame >= _duration_frames)
    {
        _sprites.clear();
    }
}