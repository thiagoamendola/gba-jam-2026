#ifndef STAGE_4_SCENE_H
#define STAGE_4_SCENE_H

#include "base_gameplay_scene.h"

#include "dog_enemy.h"
#include "melee_enemy.h"
#include "ranged_enemy.h"
#include "game_state.h"

class stage_4_scene : public base_gameplay_scene
{
public:
    explicit stage_4_scene(game_state* game_state);
    virtual ~stage_4_scene();

private:
    dog_enemy _enemy1;
    dog_enemy _enemy2;
    dog_enemy _enemy3;
    dog_enemy _enemy4;
    melee_enemy _enemy5;
    ranged_enemy _enemy6;
    ranged_enemy _enemy7;

    void _update_enemies(const bn::fixed_point& movement) override;
};

#endif // STAGE_4_SCENE_H