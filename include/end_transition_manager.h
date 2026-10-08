#ifndef END_TRANSITION_MANAGER_H
#define END_TRANSITION_MANAGER_H

#include "bn_affine_bg_ptr.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"

class end_transition_manager
{
public:
    // App BG movement parameters.
    static constexpr int APP_BG_FADE_IN_FRAMES = 60;
    static constexpr int APP_BG_WAIT_FRAMES = 25;
    static constexpr int APP_BG_ZOOM_OUT_FRAMES = 40;
    static constexpr int APP_BG_WAIT_2_FRAMES = 80;
    static constexpr int APP_BG_MOVE_OUT_FRAMES = 50;
    static constexpr int APP_BG_ZOOM_OUT_START_FRAME = APP_BG_FADE_IN_FRAMES + APP_BG_WAIT_FRAMES;
    static constexpr int APP_BG_MOVE_OUT_START_FRAME =
            APP_BG_ZOOM_OUT_START_FRAME + APP_BG_ZOOM_OUT_FRAMES + APP_BG_WAIT_2_FRAMES;
    static constexpr int APP_BG_ANIMATION_FRAMES = APP_BG_MOVE_OUT_START_FRAME + APP_BG_MOVE_OUT_FRAMES;
    static constexpr bn::fixed_point APP_BG_START_POSITION = { 0, 0 };
    static constexpr bn::fixed_point APP_BG_ZOOM_OUT_POSITION = { 0, -10 };
    static constexpr bn::fixed_point APP_BG_END_POSITION = { 0, -300 };
    static constexpr bn::fixed APP_BG_START_SCALE = bn::fixed(1);
    static constexpr bn::fixed APP_BG_ZOOM_OUT_SCALE = bn::fixed(0.5);
    static constexpr bn::fixed APP_BG_END_SCALE = bn::fixed(0.4);

    // Finger movement parameters.
    static constexpr int FINGER_WAIT_FRAMES = 80;
    static constexpr int FINGER_RISE_FRAMES = 40;
    static constexpr int FINGER_PREPARE_FRAMES = 80;
    static constexpr int FINGER_EXIT_FRAMES = 30;
    static constexpr int FINGER_ANIMATION_FRAMES =
        FINGER_WAIT_FRAMES + FINGER_RISE_FRAMES + FINGER_PREPARE_FRAMES + FINGER_EXIT_FRAMES;
    static constexpr bn::fixed_point FINGER_START_POSITION = { 0, 256 };
    static constexpr bn::fixed_point FINGER_CENTER_POSITION = { 0, 0 };
    static constexpr bn::fixed_point FINGER_PREPARE_POSITION = { 35, 50 };
    static constexpr bn::fixed_point FINGER_END_POSITION = { 20, -256 };

    static constexpr int FULL_ANIMATION_FRAMES = APP_BG_ANIMATION_FRAMES > FINGER_ANIMATION_FRAMES ?
        APP_BG_ANIMATION_FRAMES : FINGER_ANIMATION_FRAMES;

    end_transition_manager() = default;
    ~end_transition_manager();

    void start();

    [[nodiscard]] bool update();
    [[nodiscard]] bool started() const;
    [[nodiscard]] bool fade_in_complete() const;

private:
    bn::optional<bn::affine_bg_ptr> _app_bg;
    bn::optional<bn::regular_bg_ptr> _finger_bg;
    int _animation_frame = 0;
    bool _started = false;

    void _configure_windows();
    void _update_app_background();
    void _update_finger();
    void _update_finger_window();
};

#endif // END_TRANSITION_MANAGER_H
