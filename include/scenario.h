#ifndef SCENARIO_H
#define SCENARIO_H

#include "bn_fixed_point.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_ptr.h"

class scenario
{
public:
    scenario(const bn::regular_bg_item& bg_item, const bn::regular_bg_item& walls_item,
        const bn::fixed_point& initial_position);
    ~scenario();

    void update(bn::fixed_point movement);

    const bn::fixed_point& initial_position() const { return _initial_position; }
    [[nodiscard]] bn::fixed_point walls_image_to_world_position(
            const bn::fixed_point& image_position) const;

private:
    bn::regular_bg_ptr _bg;
    bn::regular_bg_ptr _walls_bg;

    bn::fixed_point _initial_position;
    bn::fixed_point _current_position;

    void _update_bg_window();
};

#endif // SCENARIO_H