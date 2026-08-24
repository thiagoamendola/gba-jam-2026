#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "base_scene.h"
#include "bn_regular_bg_ptr.h"

class test_scene : public base_scene
{
public:
    test_scene();
    virtual ~test_scene();

    bn::optional<scene_type> update() override;

private:
    bn::regular_bg_ptr _bg;
};

#endif // TEST_SCENE_H