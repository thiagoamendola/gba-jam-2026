#include "test_scene.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"

#include "scene_type.h"

#include "bn_regular_bg_items_floor_old.h"

test_scene::test_scene()
    : _bg(bn::regular_bg_items::floor_old.create_bg(0, 0))
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
