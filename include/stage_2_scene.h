#ifndef STAGE_2_SCENE_H
#define STAGE_2_SCENE_H

#include "base_gameplay_scene.h"

#include "melee_enemy.h"
#include "dog_enemy.h"
#include "game_state.h"

class stage_2_scene : public base_gameplay_scene
{
public:
    explicit stage_2_scene(game_state* game_state);
    virtual ~stage_2_scene();

private:
    melee_enemy _melee_enemy1;
    melee_enemy _melee_enemy2;
    melee_enemy _melee_enemy3;
    melee_enemy _melee_enemy4;
    melee_enemy _melee_enemy5;
    melee_enemy _melee_enemy6;

    void _update_enemies(const bn::fixed_point& movement) override;
};

#endif // STAGE_2_SCENE_H