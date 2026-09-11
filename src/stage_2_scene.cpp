#include "stage_2_scene.h"

#include "constants.h"

#include "bn_regular_bg_items_stage_2.h"
#include "bn_regular_bg_items_stage_2_walls.h"
#include "stage_2_defs.h"

#include "common_variable_8x8_sprite_font.h"

stage_2_scene::stage_2_scene() :
    base_gameplay_scene(
        bn::regular_bg_items::stage_2, bn::regular_bg_items::stage_2_walls,
        bn::fixed_point(370, 370), bn::fixed_point(349, 116),
        std::initializer_list<base_enemy*>({
            &_melee_enemy1, &_melee_enemy2, &_melee_enemy3, &_melee_enemy4, &_melee_enemy5, &_melee_enemy6
        }),
        scene_type::STORY_1, scene_type::STAGE_2,
        stage_2_defs::horizontal_walls.data(), stage_2_defs::horizontal_walls.size(),
        stage_2_defs::vertical_walls.data(), stage_2_defs::vertical_walls.size()),
        _melee_enemy1(&_player, bn::fixed_point(77, 180),
            {bn::fixed_point(77, 180), bn::fixed_point(-22, 180)}),
        _melee_enemy2(&_player, bn::fixed_point(63, 254),
            {}),
        _melee_enemy3(&_player, bn::fixed_point(140, 175),
            {bn::fixed_point(140, 175), bn::fixed_point(240, 175)}),
        _melee_enemy4(&_player, bn::fixed_point(180, 210),
            {bn::fixed_point(180, 210), bn::fixed_point(239, 251), bn::fixed_point(143, 224),}),
        _melee_enemy5(&_player, bn::fixed_point(50, 50),
            {bn::fixed_point(50, 50), bn::fixed_point(50, -10), bn::fixed_point(230, -10), bn::fixed_point(230, 50)}),
        _melee_enemy6(&_player, bn::fixed_point(50, -10),
            {bn::fixed_point(50, -10), bn::fixed_point(230, -10), bn::fixed_point(230, 50), bn::fixed_point(50, 50)})
{
}

stage_2_scene::~stage_2_scene()
{
}

void stage_2_scene::_update_enemies(const bn::fixed_point &movement)
{
    _melee_enemy1.update(movement, _walls);
    _melee_enemy2.update(movement, _walls);
    _melee_enemy3.update(movement, _walls);
    _melee_enemy4.update(movement, _walls);
    _melee_enemy5.update(movement, _walls); 
    _melee_enemy6.update(movement, _walls);
}