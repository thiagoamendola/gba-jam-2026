#include "test_scene.h"

#include "constants.h"
#include "stage_1_defs.h"

#include "common_variable_8x8_sprite_font.h"
#include "bn_regular_bg_items_stage_1.h"
#include "bn_regular_bg_items_stage_1_walls.h"

test_scene::test_scene(game_state* game_state) :
    base_gameplay_scene(
        game_state,
        bn::regular_bg_items::stage_1, bn::regular_bg_items::stage_1_walls,
        bn::fixed_point(370, -370), bn::fixed_point(485, -800),
        std::initializer_list<base_enemy*>({&_melee_enemy1, &_melee_enemy2}),
        scene_type::STORY_1, scene_type::TEST,
        stage_1_defs::horizontal_walls.data(), stage_1_defs::horizontal_walls.size(),
        stage_1_defs::vertical_walls.data(), stage_1_defs::vertical_walls.size()),
        _melee_enemy1(&_player, bn::fixed_point(290, -545),
            {bn::fixed_point(290, -545), bn::fixed_point(470, -545)}),
        _melee_enemy2(&_player, bn::fixed_point(480, -315),
            {bn::fixed_point(480, -530), bn::fixed_point(480, -315)}),
        _melee_enemy3(&_player, bn::fixed_point(195, -400), // <-- REMOVE
            {bn::fixed_point(200, -400)})
{
    // bn::music_items::supernovaexplosion.play();
    // bn::music_items::supernovaexplosion_1.play();

    // bn::music_items::beyond_throughthefire.play();
    // bn::music_items::gameplay_p1.play();
    // bn::music_items::beyond_insidetherobot.play(); // WORKS
    // bn::music_items::onekb.play();
    // bn::music_items::ekorren_fortressrock.play();
    
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
