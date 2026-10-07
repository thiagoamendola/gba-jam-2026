#ifndef WARNINGS_SCENE_H
#define WARNINGS_SCENE_H

#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"

#include "base_scene.h"

class warnings_scene : public base_scene
{
public:
    warnings_scene();

    bn::optional<scene_type> update() override;

private:
    enum class phase
    {
        PHOTOSENSITIVE_FADE_IN,
        PHOTOSENSITIVE_SHOW,
        PHOTOSENSITIVE_FADE_OUT,
        PHONES_FADE_IN,
        PHONES_SHOW,
        PHONES_FADE_OUT,
    };

    static constexpr int FADE_FRAMES = 60;
    static constexpr int SHOW_FRAMES = 180;

    bn::regular_bg_ptr _photosensitive_background;
    bn::regular_bg_ptr _phones_background;
    phase _phase = phase::PHOTOSENSITIVE_FADE_IN;
    int _phase_elapsed_frames = 0;
};

#endif // WARNINGS_SCENE_H