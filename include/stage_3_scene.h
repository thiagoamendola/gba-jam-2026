#ifndef STAGE_3_SCENE_H
#define STAGE_3_SCENE_H

#include "base_gameplay_scene.h"

#include "melee_enemy.h"
#include "ranged_enemy.h"
#include "dog_enemy.h"
#include "game_state.h"

class stage_3_scene : public base_gameplay_scene
{
public:
    explicit stage_3_scene(game_state* game_state);
    virtual ~stage_3_scene();

private:
    melee_enemy _enemy1;
    melee_enemy _enemy2;
    ranged_enemy _enemy3;
    ranged_enemy _enemy4;
    dog_enemy _enemy5;
    ranged_enemy _enemy6;
    dog_enemy _enemy7;
    ranged_enemy _enemy8;

    void _update_enemies(const bn::fixed_point& movement) override;
};

#endif // STAGE_3_SCENE_H
