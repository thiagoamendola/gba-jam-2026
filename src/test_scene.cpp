#include "test_scene.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_fixed_point.h"
#include "bn_backdrop.h"
#include "bn_color.h"

#include "scene_type.h"
#include "controller.h"

test_scene::test_scene()
    : _scenario(), _controller(), _player()
{
    bn::backdrop::set_color(bn::color(16, 0, 0));

}

test_scene::~test_scene()
{
}

bn::optional<scene_type> test_scene::update()
{
    bn::fixed_point movement = _controller.get_smooth_directional() * 3.0f; // <-- MAGIC NUMBER

    _player.update(movement);
    _scenario.update(movement);

    return bn::nullopt;
}
