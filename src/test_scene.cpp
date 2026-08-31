#include "test_scene.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_fixed_point.h"
#include "bn_backdrop.h"
#include "bn_color.h"

#include "scene_type.h"
#include "controller.h"

// #include "bn_regular_bg_items_land.h"
#include "bn_regular_bg_items_stage_1.h"
#include "bn_regular_bg_items_stage_1_walls.h"
#include "stage_1_defs.h"

test_scene::test_scene()
    : _controller(), 
      _scenario(bn::regular_bg_items::stage_1, bn::regular_bg_items::stage_1_walls, 
        bn::fixed_point(370, -370)), 
      _player(), _dog(&_player, bn::fixed_point(400, 400)),
      _walls(&_scenario)
{
    bn::backdrop::set_color(bn::color(16, 0, 0));

    // <-- REPLACE std::array WITH bn::span or bn::vector
    _walls.create_walls(
        stage_1_defs::horizontal_walls.data(), stage_1_defs::horizontal_walls.size(),
        stage_1_defs::vertical_walls.data(), stage_1_defs::vertical_walls.size());

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
