#include "scenario.h"

#include "bn_display.h"
#include "bn_rect_window.h"
#include "bn_size.h"
#include "bn_window.h"

scenario::scenario(const bn::regular_bg_item& bg_item, const bn::regular_bg_item& walls_item,
    const bn::fixed_point& initial_position)
    : _bg(bg_item.create_bg(initial_position)),
      _walls_bg(walls_item.create_bg(initial_position)),
      _end_transition_manager(),
      _initial_position(initial_position),
      _current_position(initial_position),
      _walls_dimensions(
          walls_item.map_item().dimensions().width() * 8,
          walls_item.map_item().dimensions().height() * 8)
{
    _configure_regular_bg_window();
}

scenario::~scenario()
{
}

bn::fixed_point scenario::walls_image_to_world_position(
    const bn::fixed_point& image_position) const
{
    const bn::fixed_point walls_half_dimensions(
        _walls_dimensions.width() / 2,
        _walls_dimensions.height() / 2);
    return _initial_position + image_position - walls_half_dimensions;
}

void scenario::update(bn::fixed_point movement)
{
    if(! _bg || ! _walls_bg)
    {
        return;
    }

    // Update scenario position
    _current_position -= movement;
    _bg->set_position(_current_position);
    _walls_bg->set_position(_current_position);
    _update_bg_window();
}

void scenario::start_exit_transition()
{
    // Crossfade the app background over the live stage before releasing its resources.
    _bg->set_blending_bottom_enabled(true);
    _walls_bg->set_blending_bottom_enabled(true);
    _exit_transition_stage_hidden = false;
    _end_transition_manager.start();
}

bool scenario::update_exit_transition()
{
    const bool updated = _end_transition_manager.update();

    if(updated && _end_transition_manager.fade_in_complete() && ! _exit_transition_stage_hidden)
    {
        _bg.reset();
        _walls_bg.reset();
        _exit_transition_stage_hidden = true;
    }

    return updated;
}

bool scenario::exit_transition_fade_in_complete() const
{
    return _end_transition_manager.fade_in_complete();
}

void scenario::_configure_regular_bg_window()
{
    bn::rect_window internal_window = bn::rect_window::internal();
    internal_window.set_visible(true);
    internal_window.set_show_bg(*_bg, true);
    bn::window::outside().set_show_bg(*_bg, false);
    _update_bg_window();
}

void scenario::_update_bg_window()
{
    if(! _bg)
    {
        return;
    }

    const bn::size bg_dimensions = _bg->dimensions();
    const bn::fixed bg_left = _current_position.x() - (bg_dimensions.width() / 2);
    const bn::fixed bg_top = _current_position.y() - (bg_dimensions.height() / 2);
    const bn::fixed bg_right = _current_position.x() + (bg_dimensions.width() / 2);
    const bn::fixed bg_bottom = _current_position.y() + (bg_dimensions.height() / 2);
    const bn::fixed screen_left = -bn::display::width() / 2;
    const bn::fixed screen_top = -bn::display::height() / 2;
    const bn::fixed screen_right = bn::display::width() / 2;
    const bn::fixed screen_bottom = bn::display::height() / 2;
    const bn::fixed visible_left = bg_left > screen_left ? bg_left : screen_left;
    const bn::fixed visible_top = bg_top > screen_top ? bg_top : screen_top;
    const bn::fixed visible_right = bg_right < screen_right ? bg_right : screen_right;
    const bn::fixed visible_bottom = bg_bottom < screen_bottom ? bg_bottom : screen_bottom;

    bn::rect_window internal_window = bn::rect_window::internal();

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
