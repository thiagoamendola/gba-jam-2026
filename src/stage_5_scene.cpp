#include "stage_5_scene.h"

#include "bn_regular_bg_items_stage_5.h"
#include "bn_regular_bg_items_stage_5_walls.h"
#include "bn_sprite_items_gatito.h"
#include "stage_5_defs.h"

stage_5_scene::stage_5_scene(game_state* game_state) :
    base_gameplay_scene(
        game_state,
        bn::regular_bg_items::stage_5, bn::regular_bg_items::stage_5_walls,
        bn::fixed_point(24, 419), bn::fixed_point(0, 226),
        std::initializer_list<base_enemy*>({
            &_enemy1, /*&_enemy2,*/ &_enemy3, &_enemy4, /*&_enemy5,*/ &_enemy6,
            &_enemy7, /*&_enemy8,*/ &_enemy9, /*&_enemy10,*/ &_enemy11, &_enemy12, &_enemy13, &_enemy14, &_enemy15,
            &_enemy5, &_enemy8, &_enemy10, &_enemy2, &_enemy16, &_enemy17
        }),
        scene_type::STORY_END, scene_type::STAGE_5,
        stage_5_defs::horizontal_walls.data(), stage_5_defs::horizontal_walls.size(),
        stage_5_defs::vertical_walls.data(), stage_5_defs::vertical_walls.size()),
        _enemy1(this, &_player, bn::fixed_point(-143, 103),
            {bn::fixed_point(-143, 103), bn::fixed_point(150, 103)}),
        // _enemy2(this, &_player, bn::fixed_point(155, 96),
        //     {bn::fixed_point(155, 96), bn::fixed_point(155, 290)}),
        _enemy3(this, &_player, bn::fixed_point(128, 290),
            {bn::fixed_point(128, 290), bn::fixed_point(128, 106)}),
        _enemy4(this, &_player, bn::fixed_point(-163, 103),
            {bn::fixed_point(-163, 103), bn::fixed_point(-163, 290)}),
        // _enemy5(this, &_player, bn::fixed_point(-130, 290),
        //     {bn::fixed_point(-130, 290), bn::fixed_point(-130, 103)}),
        _enemy6(this, &_player, bn::fixed_point(142, 292),
            {bn::fixed_point(142, 292), bn::fixed_point(-115, 292)}),
    
        _enemy7(this, &_player, bn::fixed_point(-238, 56),
            {bn::fixed_point(-238, 56), bn::fixed_point(-238, 260)}),
        // _enemy8(this, &_player, bn::fixed_point(-238, 260),
        //     {bn::fixed_point(-238, 56), bn::fixed_point(-238, 260)}),
        _enemy9(this, &_player, bn::fixed_point(-236, 301),
            {bn::fixed_point(-234, 304)}),
        // _enemy10(this, &_player, bn::fixed_point(-155, 334),
        //     {bn::fixed_point(-160, 336)}),
        _enemy11(this, &_player, bn::fixed_point(-270, 479),
            {bn::fixed_point(-268, 477)}),
        _enemy12(this, &_player, bn::fixed_point(-262, 425),
            {bn::fixed_point(-262, 425), bn::fixed_point(-171, 425)}),
        _enemy13(this, &_player, bn::fixed_point(-141, 474),
            {bn::fixed_point(-139, 474)}),
        _enemy14(this, &_player, bn::fixed_point(-110, 351),
            {bn::fixed_point(-110, 351), bn::fixed_point(-110, 414), bn::fixed_point(-40, 414)}),
        _enemy15(this, &_player, bn::fixed_point(-28, 345),
            {bn::fixed_point(-28, 345), bn::fixed_point(-28, 409), bn::fixed_point(-90, 345)}),
        
        _enemy5(this, &_player, bn::fixed_point(223, 57),
            {bn::fixed_point(223, 57), bn::fixed_point(223, 131)}),
        _enemy8(this, &_player, bn::fixed_point(207, 192),
            {bn::fixed_point(207, 192), bn::fixed_point(207, 289), bn::fixed_point(267, 289), bn::fixed_point(207, 289)}),
        _enemy10(this, &_player, bn::fixed_point(274, 328),
            {bn::fixed_point(274, 328), bn::fixed_point(274, 218), bn::fixed_point(329, 218), bn::fixed_point(274, 218)}),
        _enemy2(this, &_player, bn::fixed_point(30, 353),
                {bn::fixed_point(30, 353), bn::fixed_point(137, 431), bn::fixed_point(137, 348), bn::fixed_point(25, 430)}),
        _enemy16(this, &_player, bn::fixed_point(208, 388),
            {bn::fixed_point(212, 392)}),
        _enemy17(this, &_player, bn::fixed_point(302, 400),
            {bn::fixed_point(302, 400), bn::fixed_point(320, 457), bn::fixed_point(219, 451)})
{
    _backdrop_manager.set_color_fade(bn::color(20, 0, 20), bn::color(25, 9, 2));
}

stage_5_scene::~stage_5_scene()
{
}

void stage_5_scene::_update_enemies(const bn::fixed_point& movement)
{
    _enemy1.update(movement, _walls);
    // _enemy2.update(movement, _walls);
    _enemy3.update(movement, _walls);
    _enemy4.update(movement, _walls);
    // _enemy5.update(movement, _walls);
    _enemy6.update(movement, _walls);
    
    _enemy7.update(movement, _walls);
    // _enemy8.update(movement, _walls);
    _enemy9.update(movement, _walls);
    // _enemy10.update(movement, _walls);
    _enemy11.update(movement, _walls);
    _enemy12.update(movement, _walls);
    _enemy13.update(movement, _walls);
    _enemy14.update(movement, _walls);
    _enemy15.update(movement, _walls);

    _enemy5.update(movement, _walls);
    _enemy8.update(movement, _walls);
    _enemy10.update(movement, _walls);
    _enemy2.update(movement, _walls);
    _enemy16.update(movement, _walls);
    _enemy17.update(movement, _walls);
}