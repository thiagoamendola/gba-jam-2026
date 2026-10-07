#include "title_scene.h"

#include "bn_affine_bg_items_appbg.h"
#include "bn_blending.h"
#include "bn_display.h"
#include "bn_window.h"
#include "easing.h"

#include "common_variable_8x8_sprite_font.h"
#include "bn_regular_bg_items_titlescreen.h"
#include "bn_regular_bg_items_titlescreen_finger.h"
#include "bn_regular_bg_items_titlescreen_light.h"
#include "bn_sprite_items_appicon.h"

title_scene::title_scene() :
    _background(bn::regular_bg_items::titlescreen.create_bg()),
    _light(bn::regular_bg_items::titlescreen_light.create_bg()),
    _finger(bn::regular_bg_items::titlescreen_finger.create_bg()),
    _app_bg(bn::affine_bg_items::appbg.create_bg()),
    _appicon(bn::sprite_items::appicon.create_sprite(0, APPICON_START_Y)),
    _start_message_text_generator(common::variable_8x8_sprite_font),
    _start_message("Press any button to start")
{
    _background.set_y(BACKGROUND_START_Y);
    _background.set_blending_enabled(true);
    _light.set_priority(0);
    _light.set_y(LIGHT_START_Y);
    _light.set_blending_enabled(true);
    _light.set_visible(false);
    _finger.set_priority(0);
    _finger.set_position(FINGER_START_POSITION);
    _finger.set_blending_enabled(true);
    _finger.set_visible(false);
    _app_bg.set_position(0, 0);
    _app_bg.set_priority(1);
    _app_bg.set_scale(APP_BG_START_SCALE);
    _app_bg.set_blending_enabled(true);
    _app_bg.set_visible(false);
    _app_bg.set_wrapping_enabled(false);
    _appicon.set_bg_priority(1);
    _appicon.set_visible(false);
    bn::blending::set_fade_alpha(0);

    bn::rect_window internal_window = bn::rect_window::internal();
    internal_window.set_show_bg(_background, true);
    internal_window.set_show_bg(_light, true);
    internal_window.set_show_bg(_finger, true);
    internal_window.set_show_bg(_app_bg, true);

    bn::window::outside().set_show_bg(_background, true);
    bn::window::outside().set_show_bg(_light, false);
    bn::window::outside().set_show_bg(_finger, false);
    bn::window::outside().set_show_bg(_app_bg, true);
    _update_background_window();

    _start_message_text_generator.set_center_alignment();
    _start_message_text_generator.set_bg_priority(0);
    _start_message_text_generator.generate(0, 68, _start_message, _start_message_sprites);
}

title_scene::~title_scene()
{
    bn::blending::set_fade_alpha(0);
    bn::rect_window::internal().restore();
    bn::window::outside().restore();
}

bn::optional<scene_type> title_scene::update()
{
    if (!_started)
    {
        if (_controller.is_any_button_pressed())
        {
            _started = true;
            _light.set_visible(true);
            _start_message_sprites.clear();
        }

        return bn::nullopt;
    }

    return handle_update();
}

bn::optional<scene_type> title_scene::handle_update()
{
    // Animate screen light.
    if (_light_elapsed_frames < LIGHT_TRAVEL_FRAMES)
    {
        _light_elapsed_frames++;
        const bn::fixed progress = bn::fixed(_light_elapsed_frames) / LIGHT_TRAVEL_FRAMES;
        _light.set_y(LIGHT_START_Y + (LIGHT_END_Y - LIGHT_START_Y) * progress);
    }

    if (_elapsed_frames == FINGER_START_FRAME)
    {
        _light.set_visible(false);
        _finger.set_visible(true);
    }

    // Animate finger going down.
    if (_elapsed_frames >= FINGER_START_FRAME && 
        _finger_elapsed_frames < FINGER_TRAVEL_FRAMES)
    {
        _finger_elapsed_frames++;
        const bn::fixed linear_progress = 
            bn::fixed(_finger_elapsed_frames) / FINGER_TRAVEL_FRAMES;
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_OUT);
        _finger.set_position(
            FINGER_START_POSITION.x() + (FINGER_END_POSITION.x() - FINGER_START_POSITION.x()) * progress,
            FINGER_START_POSITION.y() + (FINGER_END_POSITION.y() - FINGER_START_POSITION.y()) * progress);
    }
    // Animate finger + BG sliding up.
    else if (_finger_elapsed_frames >= FINGER_TRAVEL_FRAMES &&
        _finger_elapsed_frames_2 < FINGER_TRAVEL_FRAMES_2)
    {
        _finger_elapsed_frames_2++;
        const bn::fixed linear_progress =
            bn::fixed(_finger_elapsed_frames_2) / FINGER_TRAVEL_FRAMES_2;
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_IN);
        _finger.set_position(
            FINGER_END_POSITION.x() + (FINGER_END_POSITION_2.x() - FINGER_END_POSITION.x()) * progress,
            FINGER_END_POSITION.y() + (FINGER_END_POSITION_2.y() - FINGER_END_POSITION.y()) * progress);
        _background.set_y(BACKGROUND_START_Y + (BACKGROUND_END_Y - BACKGROUND_START_Y) * progress);
    }

    // Animate the app icon after the screen has been dragged away.
    if (_elapsed_frames == APPICON_START_FRAME)
    {
        _finger.set_visible(false);
        _appicon.set_visible(true);
    }

    if (_elapsed_frames >= APPICON_START_FRAME &&
        _appicon_elapsed_frames < APPICON_TRAVEL_FRAMES)
    {
        _appicon_elapsed_frames++;
        const bn::fixed linear_progress =
            bn::fixed(_appicon_elapsed_frames) / APPICON_TRAVEL_FRAMES;
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_OUT);
        _appicon.set_y(APPICON_START_Y + (APPICON_CENTER_Y - APPICON_START_Y) * progress);
    }

    // Bring the finger back slowly from above to touch the app icon.
    if (_elapsed_frames == FINGER_TOUCH_START_FRAME)
    {
        _finger.set_position(FINGER_START_POSITION);
        _finger.set_visible(true);
    }

    if (_elapsed_frames >= FINGER_TOUCH_START_FRAME &&
        _touch_finger_elapsed_frames < FINGER_TOUCH_TRAVEL_FRAMES)
    {
        _touch_finger_elapsed_frames++;
        const bn::fixed linear_progress =
            bn::fixed(_touch_finger_elapsed_frames) / FINGER_TOUCH_TRAVEL_FRAMES;
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_OUT);
        _finger.set_position(
            FINGER_START_POSITION.x() + (FINGER_TOUCH_POSITION.x() - FINGER_START_POSITION.x()) * progress,
            FINGER_START_POSITION.y() + (FINGER_TOUCH_POSITION.y() - FINGER_START_POSITION.y()) * progress);
    }

    // Briefly press the app icon and return.
    if (_elapsed_frames >= FINGER_TOUCH_MOTION_START_FRAME &&
        _touch_motion_elapsed_frames < FINGER_TOUCH_MOTION_FRAMES)
    {
        _touch_motion_elapsed_frames++;
        const int touch_frame = _touch_motion_elapsed_frames <= FINGER_TOUCH_MOTION_HALF_FRAMES ?
            _touch_motion_elapsed_frames : FINGER_TOUCH_MOTION_FRAMES - _touch_motion_elapsed_frames;
        const bn::fixed progress = bn::fixed(touch_frame) / FINGER_TOUCH_MOTION_HALF_FRAMES;
        _finger.set_position(
            FINGER_TOUCH_POSITION.x() + (FINGER_TOUCH_PRESSED_POSITION.x() - FINGER_TOUCH_POSITION.x()) * progress,
            FINGER_TOUCH_POSITION.y() + (FINGER_TOUCH_PRESSED_POSITION.y() - FINGER_TOUCH_POSITION.y()) * progress);
    }

    // Launch the app bg from center and expand.
    if (_elapsed_frames == APP_BG_START_FRAME)
    {
        _appicon.set_visible(false);
        _app_bg.set_visible(true);
    }

    if (_elapsed_frames > APP_BG_START_FRAME &&
        _app_bg_elapsed_frames < APP_BG_SCALE_FRAMES)
    {
        _app_bg_elapsed_frames++;
        const bn::fixed progress = apply_easing(
            bn::fixed(_app_bg_elapsed_frames) / APP_BG_SCALE_FRAMES, easing::EASE_OUT);
        _app_bg.set_scale(APP_BG_START_SCALE + (APP_BG_END_SCALE - APP_BG_START_SCALE) * progress);
    }

    // Keep the completed app background visible, then fade the whole scene to black.
    if (_elapsed_frames == APP_BG_FADE_OUT_START_FRAME)
    {
        bn::blending::set_black_fade_color();
    }

    if (_elapsed_frames >= APP_BG_FADE_OUT_START_FRAME &&
        _elapsed_frames < APP_BG_FADE_OUT_START_FRAME + APP_BG_FADE_OUT_FRAMES)
    {
        const int fade_elapsed_frames =
            _elapsed_frames - APP_BG_FADE_OUT_START_FRAME + 1;
        bn::blending::set_fade_alpha(bn::fixed(fade_elapsed_frames) / APP_BG_FADE_OUT_FRAMES);
    }

    // Slide the finger off the screen.
    if (_elapsed_frames >= FINGER_EXIT_START_FRAME &&
        _finger_exit_elapsed_frames < FINGER_EXIT_TRAVEL_FRAMES)
    {
        _finger_exit_elapsed_frames++;
        const bn::fixed linear_progress =
            bn::fixed(_finger_exit_elapsed_frames) / FINGER_EXIT_TRAVEL_FRAMES;
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_IN);
        _finger.set_position(
            FINGER_TOUCH_POSITION.x() + (FINGER_EXIT_POSITION.x() - FINGER_TOUCH_POSITION.x()) * progress,
            FINGER_TOUCH_POSITION.y() + (FINGER_EXIT_POSITION.y() - FINGER_TOUCH_POSITION.y()) * progress);
    }

    _update_background_window();

    if (++_elapsed_frames >= FRAMES_TO_NEXT_SCENE)
    {
        return scene_type::TEST;
    }

    return bn::nullopt;
}

void title_scene::_update_background_window()
{
    bn::rect_window internal_window = bn::rect_window::internal();
    const bn::regular_bg_ptr* moving_background = nullptr;

    // Selects the BG to animate, if any.
    if (_finger.visible())
    {
        moving_background = &_finger;
    }
    else if (_light.visible())
    {
        moving_background = &_light;
    }

    if (!moving_background)
    {
        internal_window.set_visible(false);
        return;
    }

    // Setup boundaries.
    const bn::fixed screen_top = -bn::display::height() / 2;
    const bn::fixed screen_bottom = bn::display::height() / 2;
    const bn::fixed screen_left = -bn::display::width() / 2;
    const bn::fixed screen_right = bn::display::width() / 2;

    const bn::fixed bg_top = moving_background->y() - (moving_background->dimensions().height() / 2);
    const bn::fixed bg_bottom = moving_background->y() + (moving_background->dimensions().height() / 2);
    const bn::fixed bg_left = moving_background->x() - (moving_background->dimensions().width() / 2);
    const bn::fixed bg_right = moving_background->x() + (moving_background->dimensions().width() / 2);
    const bn::fixed visible_top = bg_top > screen_top ? bg_top : screen_top;
    const bn::fixed visible_bottom = bg_bottom < screen_bottom ? bg_bottom : screen_bottom;
    const bn::fixed visible_left = bg_left > screen_left ? bg_left : screen_left;
    const bn::fixed visible_right = bg_right < screen_right ? bg_right : screen_right;

    if (visible_left < visible_right && visible_top < visible_bottom)
    {
        internal_window.set_boundaries(visible_top, visible_left, visible_bottom, visible_right);
        internal_window.set_visible(true);
    }
    else
    {
        internal_window.set_visible(false);
    }
}