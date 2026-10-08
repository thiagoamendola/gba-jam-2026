#include "test_scene.h"

#include "bn_backdrop.h"

#include "constants.h"
#include "stage_1_defs.h"

#include "common_variable_8x8_sprite_font.h"
#include "bn_regular_bg_items_stage_1.h"
#include "bn_regular_bg_items_stage_1_walls.h"

test_scene::test_scene(game_state* game_state) :
    base_gameplay_scene(
        game_state,
        bn::regular_bg_items::stage_1, bn::regular_bg_items::stage_1_walls,
        bn::fixed_point(370, -370), bn::fixed_point(432, -822),
        std::initializer_list<base_enemy*>({&_melee_enemy1, &_melee_enemy2, &_melee_enemy3}),
        scene_type::STORY_1, scene_type::TEST,
        stage_1_defs::horizontal_walls.data(), stage_1_defs::horizontal_walls.size(),
        stage_1_defs::vertical_walls.data(), stage_1_defs::vertical_walls.size()),
        _melee_enemy1(this, &_player, bn::fixed_point(272, -513),
            {bn::fixed_point(272, -513), bn::fixed_point(423, -513)}),
        _melee_enemy2(this, &_player, bn::fixed_point(423, -315),
            {bn::fixed_point(423, -510), bn::fixed_point(423, -315)}),
        _melee_enemy3(this, &_player, bn::fixed_point(195, -380),
            {bn::fixed_point(200, -380)})
{
    _backdrop_manager.set_color_fade(bn::color(16, 1, 12), bn::color(18, 14, 1));
}

test_scene::~test_scene()
{
}

void test_scene::_update_enemies(const bn::fixed_point &movement)
{
    _melee_enemy1.update(movement, _walls);
    _melee_enemy2.update(movement, _walls);
    _melee_enemy3.update(movement, _walls);
}
