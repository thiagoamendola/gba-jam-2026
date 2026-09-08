#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"

class controller
{
public:

    bn::fixed_point get_normalized_directional();
    bn::fixed_point get_smooth_directional();

    bool is_any_button_pressed() const;

private:
    const bn::fixed INTERP_STEP = 0.1;

    bn::fixed_point _previous_raw_dir_input;
    bn::fixed_point _smooth_dir_input;
    bn::fixed _interp;

    bn::fixed_point unit_vector(bn::fixed_point original_vector);
};

#endif
