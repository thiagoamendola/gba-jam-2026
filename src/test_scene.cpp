#include "test_scene.h"

#include "bn_log.h"
#include "bn_optional.h"

#include "scene_type.h"

test_scene::test_scene()
{
    BN_LOG("HELLOOO");
}

test_scene::~test_scene()
{

}

bn::optional<scene_type> test_scene::update()
{
    return bn::nullopt;
}
