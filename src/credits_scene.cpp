#include "credits_scene.h"

#include "bn_backdrop.h"
#include "bn_color.h"
#include "bn_display.h"
#include "bn_regular_bg_items_credits.h"
#include "bn_size.h"
#include "bn_window.h"
#include "bn_music.h"

credits_scene::credits_scene() :
    _credits_background(bn::regular_bg_items::credits.create_bg(0, CREDITS_START_Y))
{
    // Keep uncovered portions of the scrolling image black.
    bn::backdrop::set_color(bn::color(0, 0, 0));
    _credits_background.set_priority(0);

    bn::rect_window internal_window = bn::rect_window::internal();
    internal_window.set_show_bg(_credits_background, true);
    bn::window::outside().set_show_bg(_credits_background, false);
    _update_credits_window();
}

credits_scene::~credits_scene()
{
    bn::rect_window::internal().restore();
    bn::window::outside().restore();
}

bn::optional<scene_type> credits_scene::update()
{
    _credits_background.set_y(_credits_background.y() - CREDITS_SCROLL_SPEED);
    _update_credits_window();

    if (_credits_background.y() <= CREDITS_END_Y)
    {
        bn::music::stop();
        return scene_type::TITLE;
    }

    return bn::nullopt;
}

void credits_scene::_update_credits_window()
{
    const bn::fixed screen_top = -bn::display::height() / 2;
    const bn::fixed screen_bottom = bn::display::height() / 2;
    const bn::fixed screen_left = -bn::display::width() / 2;
    const bn::fixed screen_right = bn::display::width() / 2;
    const bn::size credits_dimensions = _credits_background.dimensions();
    const bn::fixed credits_top = _credits_background.y() - (credits_dimensions.height() / 2);
    const bn::fixed credits_bottom = _credits_background.y() + (credits_dimensions.height() / 2);
    const bn::fixed visible_top = credits_top > screen_top ? credits_top : screen_top;
    const bn::fixed visible_bottom = credits_bottom < screen_bottom ? credits_bottom : screen_bottom;

    bn::rect_window internal_window = bn::rect_window::internal();

    if (visible_top < visible_bottom)
    {
        internal_window.set_boundaries(visible_top, screen_left, visible_bottom, screen_right);
        internal_window.set_visible(true);
    }
    else
    {
        internal_window.set_visible(false);
    }
}