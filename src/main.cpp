#include "bn_core.h"
#include "bn_optional.h"
#include "bn_unique_ptr.h"

#include "scene_type.h"
#include "test_scene.h"
#include "base_scene.h"

int main()
{
    bn::core::init();

    bn::unique_ptr<base_scene> scene;
    bn::optional<scene_type> next_scene = scene_type::TEST;

    while(true)
    {
        // Update scene
        if (scene)
        {
            next_scene = scene->update();
        }

        // Swap to another scene
        if (next_scene)
        {
            // Clear previous scene before creating a new one.
            if (scene)
            {
                scene.reset();
            }
            else
            {
                // Only create a new scene one frame after previous scene clearing.
                switch (*next_scene)
                {
                    case scene_type::TEST:
                        scene = bn::make_unique<test_scene>();
                        break;
                }
            }
        }

        bn::core::update();
    }
}
