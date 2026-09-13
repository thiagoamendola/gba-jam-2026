#ifndef EYELID_H
#define EYELID_H

#include "bn_fixed.h"
#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

class eyelid
{
public:
    eyelid(const bn::sprite_item& tile_item, int duration_frames);

    void update();

private:
    static constexpr int TILE_COLUMNS = 4;
    static constexpr int TILES_PER_LID = TILE_COLUMNS * 3;
    static constexpr int MAX_SPRITES = TILES_PER_LID * 2;

    bn::vector<bn::sprite_ptr, MAX_SPRITES> _sprites;
    int _duration_frames;
    int _current_frame;
    bn::fixed _travel_distance;
};

#endif // EYELID_H