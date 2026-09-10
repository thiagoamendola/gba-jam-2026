#include "test_scene.h"

#include "constants.h"

#include "bn_regular_bg_items_stage_1.h"
#include "bn_regular_bg_items_stage_1_walls.h"
#include "stage_1_defs.h"

#include "common_variable_8x8_sprite_font.h"

test_scene::test_scene() :
    base_gameplay_scene(
        bn::regular_bg_items::stage_1, bn::regular_bg_items::stage_1_walls,
        bn::fixed_point(370, -370), bn::fixed_point(485, -800),
        {&_dog1, &_dog2}, 
        scene_type::STORY_1, scene_type::TEST,
        stage_1_defs::horizontal_walls.data(), stage_1_defs::horizontal_walls.size(),
        stage_1_defs::vertical_walls.data(), stage_1_defs::vertical_walls.size()),
        _dog1(&_player, bn::fixed_point(290, -545),
            {bn::fixed_point(290, -545), bn::fixed_point(470, -545)}),
        _dog2(&_player, bn::fixed_point(480, -315),
            {bn::fixed_point(480, -530), bn::fixed_point(480, -315)})
{
}

test_scene::~test_scene()
{
}

void test_scene::_update_enemies(const bn::fixed_point &movement)
{
    _dog1.update(movement, _walls);
    _dog2.update(movement, _walls);
}
