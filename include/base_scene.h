#ifndef BASE_SCENE_H
#define BASE_SCENE_H

#include "bn_optional.h"
#include "scene_type.h"

class base_scene
{
public:
    virtual ~base_scene() = default;
    virtual bn::optional<scene_type> update() = 0;

protected:
    base_scene() = default;
};

#endif // BASE_SCENE_H