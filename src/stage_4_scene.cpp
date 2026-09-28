#include "stage_4_scene.h"

#include "bn_regular_bg_items_stage_4.h"
#include "bn_regular_bg_items_stage_4_walls.h"
#include "stage_4_defs.h"

stage_4_scene::stage_4_scene(game_state* game_state) :
    base_gameplay_scene(
        game_state,
        bn::regular_bg_items::stage_4, bn::regular_bg_items::stage_4_walls,
        bn::fixed_point(220, 95), bn::fixed_point(-235, -157),
        std::initializer_list<base_enemy*>({
            &_enemy1, &_enemy2, &_enemy3, &_enemy4, 
            &_enemy5, &_enemy6, &_enemy7}),
        scene_type::STORY_END, scene_type::STAGE_4,
        stage_4_defs::horizontal_walls.data(), stage_4_defs::horizontal_walls.size(),
        stage_4_defs::vertical_walls.data(), stage_4_defs::vertical_walls.size()),
        _enemy1(this, &_player, bn::fixed_point(0, -66),
            {bn::fixed_point(0, -128), bn::fixed_point(100, -128), bn::fixed_point(100, -66), bn::fixed_point(0, -66)}),
        _enemy2(this, &_player, bn::fixed_point(100, -212),
            {bn::fixed_point(100, -270), bn::fixed_point(10, -270), bn::fixed_point(10, -212), bn::fixed_point(100, -212)}),
        _enemy3(this, &_player, bn::fixed_point(-160, -340),
            {bn::fixed_point(-160, -340), bn::fixed_point(15, -340)}),
        _enemy4(this, &_player, bn::fixed_point(-185, -340),
            {bn::fixed_point(-185, -340), bn::fixed_point(-185, -215)}),
        _enemy5(this, &_player, bn::fixed_point(-130, -290),
            {bn::fixed_point(-130, -285)}),
        _enemy6(this, &_player, bn::fixed_point(-61, -303),
            {bn::fixed_point(-61, -303), bn::fixed_point(-61, -230)}),
        _enemy7(this, &_player, bn::fixed_point(-239, -157),
            {bn::fixed_point(-239, -157), bn::fixed_point(-111, -157)})
{
    _backdrop_manager.set_color_fade(bn::color(20, 0, 20), bn::color(25, 9, 2));
}

stage_4_scene::~stage_4_scene()
{
}

void stage_4_scene::_update_enemies(const bn::fixed_point& movement)
{
    _enemy1.update(movement, _walls);
    _enemy2.update(movement, _walls);
    _enemy3.update(movement, _walls);
    _enemy4.update(movement, _walls);
    _enemy5.update(movement, _walls);
    _enemy6.update(movement, _walls);
    _enemy7.update(movement, _walls);
}