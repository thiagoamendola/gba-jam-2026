#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "base_gameplay_scene.h"
#include "melee_enemy.h"

class test_scene : public base_gameplay_scene
{
public:
    test_scene();
    virtual ~test_scene();

private:
    // dog_enemy _dog1;
    // dog_enemy _dog2;
    melee_enemy _melee_enemy1;
    melee_enemy _melee_enemy2;

    void _update_enemies(const bn::fixed_point& movement) override;
};

#endif // TEST_SCENE_H