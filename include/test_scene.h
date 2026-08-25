#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "bn_regular_bg_ptr.h"

#include "base_scene.h"
#include "scenario.h"
#include "controller.h"
#include "player.h"

class test_scene : public base_scene
{
public:
    test_scene();
    virtual ~test_scene();

    bn::optional<scene_type> update() override;

private:
    controller _controller;
    scenario _scenario;
    player _player;
};

#endif // TEST_SCENE_H