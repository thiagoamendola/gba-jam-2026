#include "test_scene.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_point.h"

#include "scene_type.h"

test_scene::test_scene()
    : _scenario()
{
    BN_LOG("HELLOOO");
}

test_scene::~test_scene()
{
}

bn::optional<scene_type> test_scene::update()
{
    _scenario.update(bn::point(0, 1)); // <-- DO SOMETHING HERE

    return bn::nullopt;
}
