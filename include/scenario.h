#ifndef SCENARIO_H
#define SCENARIO_H

#include "bn_fixed_point.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_ptr.h"

class scenario
{
public:
    scenario(const bn::regular_bg_item& bg_item, const bn::fixed_point& initial_position);
    ~scenario();

    void update(bn::fixed_point movement);

    const bn::fixed_point& initial_position() const { return _initial_position; }

private:
    bn::regular_bg_ptr _bg;

    bn::fixed_point _initial_position;
    bn::fixed_point _current_position;

    void _update_bg_window();
};

#endif // SCENARIO_H