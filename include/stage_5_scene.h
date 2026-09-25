#ifndef STAGE_5_SCENE_H
#define STAGE_5_SCENE_H

#include "base_gameplay_scene.h"

#include "dog_enemy.h"
#include "melee_enemy.h"
#include "ranged_enemy.h"
#include "game_state.h"

class stage_5_scene : public base_gameplay_scene
{
public:
    stage_5_scene(game_state* game_state);
    virtual ~stage_5_scene();

private:
    ranged_enemy _enemy1;
    ranged_enemy _enemy3;
    ranged_enemy _enemy4;
    dog_enemy _enemy6;

    ranged_enemy _enemy7;
    melee_enemy _enemy9;
    melee_enemy _enemy11;
    melee_enemy _enemy12;
    ranged_enemy _enemy13;
    dog_enemy _enemy14;
    melee_enemy _enemy15;

    melee_enemy _enemy5;
    ranged_enemy _enemy8;
    melee_enemy _enemy10;
    dog_enemy _enemy2;
    ranged_enemy _enemy16;
    melee_enemy _enemy17;


    void _update_enemies(const bn::fixed_point& movement) override;
};

#endif // STAGE_5_SCENE_H