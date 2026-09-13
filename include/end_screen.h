#ifndef END_SCREEN_H
#define END_SCREEN_H

#include "bn_affine_bg_ptr.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"

#include "base_scene.h"
#include "eyelid.h"

class end_screen : public base_scene
{
public:
    end_screen();

    bn::optional<scene_type> update() override;

private:
    static constexpr int SCALE_FRAMES = 480;
    static constexpr int BLACK_WAIT_FRAMES = 120;
    static constexpr int MUSIC_START_DELAY_FRAMES = 40;
    static constexpr int EYELID_DURATION_FRAMES = 40;
    static constexpr bn::fixed START_SCALE = bn::fixed(2.0);
    static constexpr bn::fixed END_SCALE = bn::fixed(1.1);

    bn::affine_bg_ptr _background;
    bn::regular_bg_ptr _black_background;
    eyelid _eyelid;
    int _elapsed_frames = 0;
    int _black_wait_frames = 0;
    int _music_start_delay_frames = 0;
    bool _music_start_handled = false;
};

#endif // END_SCREEN_H