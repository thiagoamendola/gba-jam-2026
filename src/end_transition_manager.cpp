#include "end_transition_manager.h"

#include "bn_affine_bg_items_appbg.h"
#include "bn_blending.h"
#include "bn_display.h"
#include "bn_rect_window.h"
#include "bn_regular_bg_items_titlescreen_finger.h"
#include "bn_size.h"
#include "bn_window.h"

#include "easing.h"

end_transition_manager::~end_transition_manager()
{
    _app_bg.reset();
    _finger_bg.reset();

    if(_started)
    {
        bn::blending::restore();
        bn::rect_window::internal().restore();
        bn::window::outside().restore();
    }
}

void end_transition_manager::start()
{
    if(_started)
    {
        return;
    }

    // The app BG fades over the still-visible stage backgrounds.
    bn::blending::restore();
    _app_bg.emplace(bn::affine_bg_items::appbg.create_bg(APP_BG_START_POSITION));

    _app_bg->set_priority(1);
    _app_bg->set_scale(APP_BG_START_SCALE);
    _app_bg->set_wrapping_enabled(false);
    _app_bg->set_blending_enabled(true);

    _configure_windows();

    bn::blending::set_transparency_alpha(0);
    _animation_frame = 0;
    _started = true;
}

bool end_transition_manager::update()
{
    if(! _started || ! _app_bg)
    {
        return false;
    }

    if(_animation_frame < APP_BG_FADE_IN_FRAMES)
    {
        const bn::fixed progress = bn::fixed(_animation_frame + 1) / APP_BG_FADE_IN_FRAMES;
        bn::blending::set_transparency_alpha(progress);
    }
    else if(! _finger_bg)
    {
        // The stage backgrounds are released immediately after the fade-in completes.
        _finger_bg.emplace(bn::regular_bg_items::titlescreen_finger.create_bg(FINGER_START_POSITION));
        _finger_bg->set_priority(0);
        _finger_bg->set_visible(false);
        _configure_windows();
    }

    _update_app_background();
    _update_finger();
    ++_animation_frame;
    return true;
}

bool end_transition_manager::started() const
{
    return _started;
}

bool end_transition_manager::fade_in_complete() const
{
    return _started && _animation_frame >= APP_BG_FADE_IN_FRAMES;
}

void end_transition_manager::_configure_windows()
{
    bn::rect_window internal_window = bn::rect_window::internal();
    internal_window.set_show_bg(*_app_bg, true);

    bn::window::outside().set_show_bg(*_app_bg, true);

    if(_finger_bg)
    {
        internal_window.set_show_bg(*_finger_bg, true);
        bn::window::outside().set_show_bg(*_finger_bg, false);
    }
}

void end_transition_manager::_update_app_background()
{
    if(_animation_frame < APP_BG_ZOOM_OUT_START_FRAME)
    {
        return;
    }

    const int zoom_out_frame = _animation_frame - APP_BG_ZOOM_OUT_START_FRAME;

    if(zoom_out_frame < APP_BG_ZOOM_OUT_FRAMES)
    {
        const bn::fixed linear_progress = APP_BG_ZOOM_OUT_FRAMES > 1 ?
                bn::fixed(zoom_out_frame).safe_division(APP_BG_ZOOM_OUT_FRAMES - 1) : bn::fixed(1);
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_IN_OUT);
        _app_bg->set_position(
                APP_BG_START_POSITION + (APP_BG_ZOOM_OUT_POSITION - APP_BG_START_POSITION).safe_multiplication(progress));
        _app_bg->set_scale(APP_BG_START_SCALE + (APP_BG_ZOOM_OUT_SCALE - APP_BG_START_SCALE) * progress);
    }
    else if(_animation_frame < APP_BG_MOVE_OUT_START_FRAME)
    {
        // Keep the completed zoom visible while the finger repositions.
        return;
    }
    else if(_animation_frame < APP_BG_MOVE_OUT_START_FRAME + APP_BG_MOVE_OUT_FRAMES)
    {
        const int move_out_frame = _animation_frame - APP_BG_MOVE_OUT_START_FRAME;
        const bn::fixed linear_progress = APP_BG_MOVE_OUT_FRAMES > 1 ?
                bn::fixed(move_out_frame).safe_division(APP_BG_MOVE_OUT_FRAMES - 1) : bn::fixed(1);
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_IN);
        _app_bg->set_position(
                APP_BG_ZOOM_OUT_POSITION + (APP_BG_END_POSITION - APP_BG_ZOOM_OUT_POSITION).safe_multiplication(progress));
        _app_bg->set_scale(APP_BG_ZOOM_OUT_SCALE + (APP_BG_END_SCALE - APP_BG_ZOOM_OUT_SCALE) * progress);
    }
    else
    {
        _app_bg->set_position(APP_BG_END_POSITION);
        _app_bg->set_scale(APP_BG_END_SCALE);
    }
}

void end_transition_manager::_update_finger()
{
    if(! _finger_bg)
    {
        return;
    }

    if(_animation_frame < FINGER_WAIT_FRAMES)
    {
        _finger_bg->set_visible(false);
        _update_finger_window();
        return;
    }

    if(_animation_frame < FINGER_WAIT_FRAMES + FINGER_RISE_FRAMES)
    {
        const int rise_frame = _animation_frame - FINGER_WAIT_FRAMES;
        const bn::fixed linear_progress = FINGER_RISE_FRAMES > 1 ?
                bn::fixed(rise_frame).safe_division(FINGER_RISE_FRAMES - 1) : bn::fixed(1);
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_OUT);
        _finger_bg->set_position(
                FINGER_START_POSITION + (FINGER_CENTER_POSITION - FINGER_START_POSITION).safe_multiplication(progress));
        _finger_bg->set_visible(true);
        _update_finger_window();
        return;
    }

    if(_animation_frame < FINGER_WAIT_FRAMES + FINGER_RISE_FRAMES + FINGER_PREPARE_FRAMES)
    {
        const int prepare_frame = _animation_frame - FINGER_WAIT_FRAMES - FINGER_RISE_FRAMES;
        const bn::fixed linear_progress = FINGER_PREPARE_FRAMES > 1 ?
                bn::fixed(prepare_frame).safe_division(FINGER_PREPARE_FRAMES - 1) : bn::fixed(1);
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_IN_OUT);
        _finger_bg->set_position(
                FINGER_CENTER_POSITION + (FINGER_PREPARE_POSITION - FINGER_CENTER_POSITION).safe_multiplication(progress));
        _finger_bg->set_visible(true);
        _update_finger_window();
        return;
    }

    if(_animation_frame < FULL_ANIMATION_FRAMES)
    {
        const int exit_frame = _animation_frame - FINGER_WAIT_FRAMES - FINGER_RISE_FRAMES - FINGER_PREPARE_FRAMES;
        const bn::fixed linear_progress = FINGER_EXIT_FRAMES > 1 ?
                bn::fixed(exit_frame).safe_division(FINGER_EXIT_FRAMES - 1) : bn::fixed(1);
        const bn::fixed progress = apply_easing(linear_progress, easing::EASE_IN);
        _finger_bg->set_position(
                FINGER_PREPARE_POSITION + (FINGER_END_POSITION - FINGER_PREPARE_POSITION).safe_multiplication(progress));
        _finger_bg->set_visible(true);
        _update_finger_window();
        return;
    }

    _finger_bg->set_visible(false);
    _update_finger_window();
}

void end_transition_manager::_update_finger_window()
{
    if(! _finger_bg)
    {
        return;
    }

    bn::rect_window internal_window = bn::rect_window::internal();

    if(! _finger_bg->visible())
    {
        // The outside window renders the app BG while it hides the finger.
        internal_window.set_visible(false);
        return;
    }

    const bn::fixed screen_top = -bn::display::height() / 2;
    const bn::fixed screen_bottom = bn::display::height() / 2;
    const bn::fixed screen_left = -bn::display::width() / 2;
    const bn::fixed screen_right = bn::display::width() / 2;
    const bn::size finger_dimensions = _finger_bg->dimensions();
    const bn::fixed finger_top = _finger_bg->y() - (finger_dimensions.height() / 2);
    const bn::fixed finger_bottom = _finger_bg->y() + (finger_dimensions.height() / 2);
    const bn::fixed finger_left = _finger_bg->x() - (finger_dimensions.width() / 2);
    const bn::fixed finger_right = _finger_bg->x() + (finger_dimensions.width() / 2);
    const bn::fixed visible_top = finger_top > screen_top ? finger_top : screen_top;
    const bn::fixed visible_bottom = finger_bottom < screen_bottom ? finger_bottom : screen_bottom;
    const bn::fixed visible_left = finger_left > screen_left ? finger_left : screen_left;
    const bn::fixed visible_right = finger_right < screen_right ? finger_right : screen_right;

    if(visible_left < visible_right && visible_top < visible_bottom)
    {
        internal_window.set_boundaries(visible_top, visible_left, visible_bottom, visible_right);
        internal_window.set_visible(true);
    }
    else
    {
        internal_window.set_visible(false);
    }
}
