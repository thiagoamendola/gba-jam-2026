#include "test_scene.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_fixed_point.h"

#include "scene_type.h"
#include "controller.h"

test_scene::test_scene()
    : _scenario(), _controller()
{
    BN_LOG("HELLOOO");
}

test_scene::~test_scene()
{
}

bn::optional<scene_type> test_scene::update()
{
    bn::fixed_point movement = _controller.get_smooth_directional() * 3.0f; // <-- MAGIC NUMBER

    _scenario.update(movement);

    return bn::nullopt;
}
