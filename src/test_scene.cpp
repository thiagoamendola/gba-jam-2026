#include "test_scene.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_fixed_point.h"
#include "bn_backdrop.h"
#include "bn_color.h"

#include "scene_type.h"
#include "controller.h"

test_scene::test_scene()
    : _controller(), _scenario(), _player(), 
      _dog(&_player, bn::fixed_point(400, 400)),
      _walls()
{
    bn::backdrop::set_color(bn::color(16, 0, 0));

    _walls.create_horizontal_wall(bn::fixed_point(150, -150), 222);
    _walls.create_horizontal_wall(bn::fixed_point(150, -86), 222);
    _walls.create_vertical_wall(bn::fixed_point(150, -150), -86);
    _walls.create_vertical_wall(bn::fixed_point(222, -150), -86);

    _walls.create_vertical_wall(bn::fixed_point(222, -50), 86);
    _walls.create_horizontal_wall(bn::fixed_point(170, 0), 270);

}

test_scene::~test_scene()
{
}

bn::optional<scene_type> test_scene::update()
{
    bn::fixed_point movement = _controller.get_smooth_directional() * 3.0f; // <-- MAGIC NUMBER

    movement = _player.update(movement, _walls);
    _scenario.update(movement);
    _dog.update(movement, _walls);
    _walls.update(movement);

    return bn::nullopt;
}
