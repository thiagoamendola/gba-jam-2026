#include "stage_2_scene.h"

#include "constants.h"

#include "bn_regular_bg_items_stage_2.h"
#include "bn_regular_bg_items_stage_2_walls.h"
#include "stage_2_defs.h"

#include "common_variable_8x8_sprite_font.h"

stage_2_scene::stage_2_scene(game_state* game_state) :
    base_gameplay_scene(
        game_state,
        bn::regular_bg_items::stage_2, bn::regular_bg_items::stage_2_walls,
        bn::fixed_point(390, 420), bn::fixed_point(369, 166),
        std::initializer_list<base_enemy*>({
            &_melee_enemy1, &_melee_enemy2, &_melee_enemy3, &_melee_enemy4, &_melee_enemy5, &_melee_enemy6,
        }),
        scene_type::STORY_2, scene_type::STAGE_2,
        stage_2_defs::horizontal_walls.data(), stage_2_defs::horizontal_walls.size(),
        stage_2_defs::vertical_walls.data(), stage_2_defs::vertical_walls.size()),
        _melee_enemy1(&_player, bn::fixed_point(97, 230),
            {bn::fixed_point(97, 230), bn::fixed_point(-2, 230)}),
        _melee_enemy2(&_player, bn::fixed_point(83, 304),
            {}),
        _melee_enemy3(&_player, bn::fixed_point(160, 225),
            {bn::fixed_point(160, 225), bn::fixed_point(260, 225)}),
        _melee_enemy4(&_player, bn::fixed_point(200, 260),
            {bn::fixed_point(200, 260), bn::fixed_point(259, 301), bn::fixed_point(163, 274),}),
        _melee_enemy5(&_player, bn::fixed_point(67, 111),
            {bn::fixed_point(67, 111), bn::fixed_point(67, 73), bn::fixed_point(267, 73), bn::fixed_point(267, 111)}),
        _melee_enemy6(&_player, bn::fixed_point(267, 73),
            {bn::fixed_point(267, 73), bn::fixed_point(267, 111), bn::fixed_point(67, 111), bn::fixed_point(67, 73)})
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