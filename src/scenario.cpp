#include "scenario.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_fixed_point.h"

#include "scene_type.h"

#include "bn_regular_bg_items_floor_old.h"

scenario::scenario()
    : _bg(bn::regular_bg_items::floor_old.create_bg(0, 0))
{
    _current_position = bn::fixed_point(0, 0);
}

scenario::~scenario()
{
}

void scenario::update(bn::fixed_point movement)
{
    // Update scenario position
    _current_position -= movement;
    _bg.set_position(_current_position);
}
