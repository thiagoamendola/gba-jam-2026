#ifndef SCENARIO_H
#define SCENARIO_H

#include "bn_regular_bg_ptr.h"
#include "bn_fixed_point.h"

class scenario
{
public:
    scenario();
    ~scenario();

    void update(bn::fixed_point movement);

private:
    bn::regular_bg_ptr _bg;

    bn::fixed_point _current_position;

    void _update_bg_window();
};

#endif // SCENARIO_H