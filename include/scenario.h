#ifndef SCENARIO_H
#define SCENARIO_H

#include "bn_regular_bg_ptr.h"
#include "bn_point.h"

class scenario
{
public:
    scenario();
    ~scenario();

    void update(bn::point movement);

private:
    bn::regular_bg_ptr _bg;

    bn::point _current_position;
};

#endif // SCENARIO_H