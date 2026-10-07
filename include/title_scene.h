#ifndef TITLE_SCENE_H
#define TITLE_SCENE_H

#include "bn_optional.h"
#include "bn_rect_window.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "base_scene.h"
#include "controller.h"

class title_scene : public base_scene
{
public:
    title_scene();
    ~title_scene();

    bn::optional<scene_type> update() override;

private:
    static constexpr int LIGHT_START_Y = -200;
    static constexpr int LIGHT_END_Y = 200;
    static constexpr int LIGHT_TRAVEL_FRAMES = 35;
    static constexpr int FINGER_START_FRAME = 60;
    static constexpr bn::fixed_point FINGER_START_POSITION =
        { 55, -250 };
    static constexpr bn::fixed_point FINGER_END_POSITION =
        { 55, 10 };
    static constexpr bn::fixed_point FINGER_END_POSITION_2 =
        { 55, -250 };
    static constexpr int FINGER_TRAVEL_FRAMES = 40;
    static constexpr int FINGER_TRAVEL_FRAMES_2 = 30;
    static constexpr int BACKGROUND_START_Y = 0;
    static constexpr int BACKGROUND_END_Y = -50;

    static constexpr int APPICON_START_FRAME = FINGER_START_FRAME + FINGER_TRAVEL_FRAMES + FINGER_TRAVEL_FRAMES_2;
    static constexpr int APPICON_START_Y = 112;
    static constexpr int APPICON_CENTER_Y = 0;
    static constexpr int APPICON_TRAVEL_FRAMES = FINGER_TRAVEL_FRAMES_2;
    static constexpr int APPICON_HOLD_FRAMES = 120;

    // Finger downward motion for touching app icon.
    static constexpr int FINGER_TOUCH_START_FRAME =
        APPICON_START_FRAME +
        APPICON_TRAVEL_FRAMES +
        APPICON_HOLD_FRAMES;
    static constexpr bn::fixed_point FINGER_TOUCH_POSITION =
        { 25, 20 };
    static constexpr int FINGER_TOUCH_TRAVEL_FRAMES = 90;

    // Finger touch pressing motion on the app icon.
    static constexpr int FINGER_TOUCH_MOTION_START_FRAME =
        FINGER_TOUCH_START_FRAME +
        FINGER_TOUCH_TRAVEL_FRAMES;
    static constexpr bn::fixed_point FINGER_TOUCH_PRESSED_POSITION =
        { 29, 17 };
    static constexpr int FINGER_TOUCH_MOTION_FRAMES = 20;
    static constexpr int FINGER_TOUCH_MOTION_HALF_FRAMES = FINGER_TOUCH_MOTION_FRAMES / 2;

    // Make finger leave screen to the side.
    static constexpr int FINGER_EXIT_START_FRAME =
        FINGER_TOUCH_MOTION_START_FRAME +
        FINGER_TOUCH_MOTION_FRAMES;
    static constexpr bn::fixed_point FINGER_EXIT_POSITION =
        { 300, 0 };
    static constexpr int FINGER_EXIT_TRAVEL_FRAMES = 60;

    static constexpr int FRAMES_TO_NEXT_SCENE =
        FINGER_EXIT_START_FRAME +
        FINGER_EXIT_TRAVEL_FRAMES;

    bn::regular_bg_ptr _background;
    bn::regular_bg_ptr _light;
    bn::regular_bg_ptr _finger;
    bn::sprite_ptr _appicon;
    controller _controller;
    bn::sprite_text_generator _start_message_text_generator;
    bn::vector<bn::sprite_ptr, 32> _start_message_sprites;
    bn::string<32> _start_message;
    int _elapsed_frames = 0;
    int _light_elapsed_frames = 0;
    int _finger_elapsed_frames = 0;
    int _finger_elapsed_frames_2 = 0;
    int _appicon_elapsed_frames = 0;
    int _touch_finger_elapsed_frames = 0;
    int _touch_motion_elapsed_frames = 0;
    int _finger_exit_elapsed_frames = 0;
    bool _started = false;

    void _update_background_window();
    bn::optional<scene_type> handle_update();
};

#endif // TITLE_SCENE_H