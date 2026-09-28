#include "stage_3_scene.h"

#include "constants.h"

#include "bn_regular_bg_items_stage_3.h"
#include "bn_regular_bg_items_stage_3_walls.h"
#include "stage_3_defs.h"

stage_3_scene::stage_3_scene(game_state* game_state) :
    base_gameplay_scene(
        game_state,
        bn::regular_bg_items::stage_3, bn::regular_bg_items::stage_3_walls,
        bn::fixed_point(-70, 440), bn::fixed_point(-554, 5),
        std::initializer_list<base_enemy*>({
            &_enemy1, &_enemy2, &_enemy3, &_enemy4, &_enemy5, &_enemy6, &_enemy7, &_enemy8
        }),
        scene_type::STORY_3, scene_type::STAGE_3,
        stage_3_defs::horizontal_walls.data(), stage_3_defs::horizontal_walls.size(),
        stage_3_defs::vertical_walls.data(), stage_3_defs::vertical_walls.size()),
        _enemy1(this, &_player, bn::fixed_point(-135, 55),
            {bn::fixed_point(-200, 55), bn::fixed_point(-200, 85), bn::fixed_point(-135, 85), bn::fixed_point(-135, 55)}),
        _enemy2(this, &_player, bn::fixed_point(-240, 140),
            {bn::fixed_point(-240, 140), bn::fixed_point(-100, 140)}),
        _enemy3(this, &_player, bn::fixed_point(-250, 5), 
            {bn::fixed_point(-240, 5)}),
        _enemy4(this, &_player, bn::fixed_point(-330, 191),
            {bn::fixed_point(-295, 191), bn::fixed_point(-305, 289), bn::fixed_point(-330, 191)}),
        _enemy5(this, &_player, bn::fixed_point(-395, 185),
            {bn::fixed_point(-395, 185), bn::fixed_point(-395, 8)}),
        _enemy6(this, &_player, bn::fixed_point(-440, 70),
            {bn::fixed_point(-440, 8), bn::fixed_point(-440, 190)}),
        _enemy7(this, &_player, bn::fixed_point(-477, 194),
            {bn::fixed_point(-477, 194), bn::fixed_point(-477, 8)}),
        _enemy8(this, &_player, bn::fixed_point(-522, 75),
            {bn::fixed_point(-522, 8), bn::fixed_point(-522, 190)})
{
}

stage_3_scene::~stage_3_scene()
{
}

void stage_3_scene::_update_enemies(const bn::fixed_point& movement)
{
    _enemy1.update(movement, _walls);
    _enemy2.update(movement, _walls);
    _enemy3.update(movement, _walls);
    _enemy4.update(movement, _walls);
    _enemy5.update(movement, _walls);
    _enemy6.update(movement, _walls);
    _enemy7.update(movement, _walls);
    _enemy8.update(movement, _walls);
}
