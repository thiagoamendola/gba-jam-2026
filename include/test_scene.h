#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "base_scene.h"

class test_scene : public base_scene
{
public:
    test_scene();
    virtual ~test_scene();

    bn::optional<scene_type> update() override;
};

#endif // TEST_SCENE_H