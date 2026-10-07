#ifndef CREDITS_SCENE_H
#define CREDITS_SCENE_H

#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_rect_window.h"
#include "bn_regular_bg_ptr.h"

#include "base_scene.h"

class credits_scene : public base_scene
{
public:
    credits_scene();
    ~credits_scene();

    bn::optional<scene_type> update() override;

private:
    static constexpr bn::fixed CREDITS_START_Y = bn::fixed(350);
    static constexpr bn::fixed CREDITS_END_Y = bn::fixed(-400);
    static constexpr bn::fixed CREDITS_SCROLL_SPEED = bn::fixed(0.5);

    bn::regular_bg_ptr _credits_background;

    void _update_credits_window();
};

#endif // CREDITS_SCENE_H