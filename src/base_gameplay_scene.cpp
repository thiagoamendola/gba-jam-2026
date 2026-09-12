#include "base_gameplay_scene.h"

#include "constants.h"

#include "bn_backdrop.h"
#include "bn_color.h"
#include "bn_fixed_point.h"
#include "bn_sstream.h"

#include "common_variable_8x8_sprite_font.h"

base_gameplay_scene::base_gameplay_scene(
        const bn::regular_bg_item& background_item, const bn::regular_bg_item& walls_item,
        const bn::fixed_point& initial_position, const bn::fixed_point& exit_position,
        std::initializer_list<base_enemy*> enemies, scene_type exit_next_scene, scene_type game_over_next_scene,
        const wall_data* horizontal_walls, int horizontal_walls_count,
        const wall_data* vertical_walls, int vertical_walls_count) :
    _controller(),
    _scenario(background_item, walls_item, initial_position),
    _player(),
    _walls(&_scenario),
    _bullets(),
    _exit_route(&_player, &_scenario, exit_position, enemies, exit_next_scene),
    _game_over_manager(&_controller, game_over_next_scene),
    _location_hud_text_generator(common::variable_8x8_sprite_font)
{
    _walls.create_walls(
            horizontal_walls, horizontal_walls_count, vertical_walls, vertical_walls_count);

    if constexpr(SHOW_LOCATION_HUD)
    {
        _location_hud_text_generator.set_right_alignment();
        _location_hud_text_generator.set_bg_priority(0);
    }

    bn::backdrop::set_color(bn::color(16, 0, 0));
}

base_gameplay_scene::~base_gameplay_scene()
{
}

bn::optional<scene_type> base_gameplay_scene::update()
{
    if (_exit_route.is_end_animation_playing())
    {
        return _exit_route.update(bn::fixed_point());
    }

    if (_player.is_dead())
    {
        _location_hud_sprites.clear();
        _location_hud_text.clear();
        return _game_over_manager.update();
    }

    // Not ending gameplay through exit or death so continue updates.

    bn::fixed_point movement = _controller.get_smooth_directional() * 3.0f; // <-- MAGIC NUMBER

    movement = _player.update(movement, _walls);
    _scenario.update(movement);

    // Update all existing bullets and remove any inactive ones.
    for (int index = _bullets.size() - 1; index >= 0; --index)
    {
        if (!_bullets[index].update(movement, _walls))
        {
            _bullets.erase(_bullets.begin() + index);
        }
    }

    _update_enemies(movement);
    _exit_route.update(movement);

    // If we started end animation in this frame, prepare it first.
    if (_exit_route.is_end_animation_playing())
    {
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

void base_gameplay_scene::_update_location_hud()
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

void base_gameplay_scene::create_bullet(
        const bn::fixed_point& position, const bn::fixed_point& world_position,
        bn::fixed rotation)
{
    // Create bullet if enough space available.
    if (_bullets.size() < MAX_BULLETS)
    {
        _bullets.emplace_back(position, world_position, rotation, &_player);
    }
}