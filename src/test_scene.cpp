#include "test_scene.h"

#include "constants.h"

#include "bn_log.h"
#include "bn_optional.h"
#include "bn_fixed_point.h"
#include "bn_backdrop.h"
#include "bn_color.h"
#include "bn_display.h"
#include "bn_sstream.h"

#include "scene_type.h"
#include "controller.h"

// #include "bn_regular_bg_items_land.h"
#include "bn_regular_bg_items_stage_1.h"
#include "bn_regular_bg_items_stage_1_walls.h"
#include "stage_1_defs.h"

#include "common_variable_8x8_sprite_font.h"

namespace
{
    constexpr int LOCATION_HUD_MARGIN = 4;
    constexpr int LOCATION_HUD_CHARACTER_HEIGHT = 8;
    constexpr bn::fixed LOCATION_HUD_X = (bn::display::width() / 2) - LOCATION_HUD_MARGIN;
    constexpr bn::fixed LOCATION_HUD_Y =
            (-bn::display::height() / 2) + LOCATION_HUD_MARGIN + (LOCATION_HUD_CHARACTER_HEIGHT / 2);
}

test_scene::test_scene()
    : _controller(), 
      _scenario(bn::regular_bg_items::stage_1, bn::regular_bg_items::stage_1_walls, 
        bn::fixed_point(370, -370)),
      _player(), _walls(&_scenario),
      _dog1(&_player, bn::fixed_point(290, -315),
              { bn::fixed_point(290, -315), bn::fixed_point(470, -315) }),
      _dog2(&_player, bn::fixed_point(290, -315),
              { bn::fixed_point(290, -530), bn::fixed_point(470, -530) }),
          _exit_route(&_player, &_scenario, bn::fixed_point(485, -800),
        { &_dog1, &_dog2 }, scene_type::TEST),
      _location_hud_text_generator(common::variable_8x8_sprite_font)
{
    bn::backdrop::set_color(bn::color(16, 0, 0));

    // <-- REPLACE std::array WITH bn::span or bn::vector
    _walls.create_walls(
        stage_1_defs::horizontal_walls.data(), stage_1_defs::horizontal_walls.size(),
        stage_1_defs::vertical_walls.data(), stage_1_defs::vertical_walls.size());

    // Show the location HUD if enabled.
    if constexpr(SHOW_LOCATION_HUD)
    {
        _location_hud_text_generator.set_right_alignment();
        _location_hud_text_generator.set_bg_priority(0);
        _update_location_hud();
    }
}

test_scene::~test_scene()
{
}

bn::optional<scene_type> test_scene::update()
{
    if(_exit_route.is_end_animation_playing())
    {
        return _exit_route.update(bn::fixed_point());
    }

    bn::fixed_point movement = _controller.get_smooth_directional() * 3.0f; // <-- MAGIC NUMBER

    movement = _player.update(movement, _walls);
    _scenario.update(movement);
    _dog1.update(movement, _walls);
    _dog2.update(movement, _walls);

    _exit_route.update(movement);

    if(_exit_route.is_end_animation_playing())
    {
        // End transition started. Hide unnecessary elements.
        _location_hud_sprites.clear();
        _location_hud_text.clear();
        bn::backdrop::set_color(bn::color(0, 0, 0));
        return bn::nullopt;
    }

    _walls.update(movement);

    if constexpr(SHOW_LOCATION_HUD)
    {
        _update_location_hud();
    }

    return bn::nullopt;
}

// <-- Move this to a HUD class
void test_scene::_update_location_hud()
{
    const bn::fixed_point& world_position = _player.world_position();

    _location_hud_sprites.clear();
    _location_hud_text.clear();

    bn::ostringstream text_stream(_location_hud_text);
    text_stream.set_precision(5);
    text_stream << "P: " << world_position.x() << ", " << world_position.y();

        static_cast<void>(_location_hud_text_generator.generate_optional(
            LOCATION_HUD_X, LOCATION_HUD_Y, _location_hud_text, _location_hud_sprites));
}
