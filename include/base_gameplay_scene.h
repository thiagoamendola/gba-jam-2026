#ifndef BASE_GAMEPLAY_SCENE
#define BASE_GAMEPLAY_SCENE

#include "bn_display.h"
#include "bn_optional.h"
#include "bn_fixed_point.h"
#include "bn_regular_bg_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "base_scene.h"
#include "scenario.h"
#include "base_enemy.h"
#include "bullet.h"
#include "controller.h"
#include "player.h"
#include "exit_route.h"
#include "walls.h"
#include "game_over_manager.h"
#include "game_state.h"


class base_gameplay_scene : public base_scene
{
public:
    base_gameplay_scene(
            game_state* game_state,
            const bn::regular_bg_item& background_item, const bn::regular_bg_item& walls_item,
            const bn::fixed_point& initial_position, const bn::fixed_point& exit_position,
            std::initializer_list<base_enemy*> enemies, scene_type exit_next_scene, scene_type game_over_next_scene,
            const wall_data* horizontal_walls, int horizontal_walls_count,
            const wall_data* vertical_walls, int vertical_walls_count);
    virtual ~base_gameplay_scene();

    bn::optional<scene_type> update() override;

    void create_bullet(
        const bn::fixed_point& position, const bn::fixed_point& world_position,
        bn::fixed rotation);

protected:
    virtual void _update_enemies(const bn::fixed_point& movement) = 0;

    controller _controller;
    scenario _scenario;
    player _player;
    walls _walls;
    
    static constexpr int MAX_BULLETS = 12;
    bn::vector<bullet, MAX_BULLETS> _bullets;
    exit_route _exit_route;
    game_over_manager _game_over_manager;

private:
    static constexpr int LOCATION_HUD_MAX_SPRITES = 8;
    static constexpr int LOCATION_HUD_TEXT_MAX_SIZE = 32;
    static constexpr int LOCATION_HUD_MARGIN = 4;
    static constexpr int LOCATION_HUD_CHARACTER_HEIGHT = 8;
    static constexpr bn::fixed LOCATION_HUD_X = (bn::display::width() / 2) - LOCATION_HUD_MARGIN;
    static constexpr bn::fixed LOCATION_HUD_Y =
            (-bn::display::height() / 2) + LOCATION_HUD_MARGIN + (LOCATION_HUD_CHARACTER_HEIGHT / 2);

    bn::sprite_text_generator _location_hud_text_generator;
    bn::vector<bn::sprite_ptr, LOCATION_HUD_MAX_SPRITES> _location_hud_sprites;
    bn::string<LOCATION_HUD_TEXT_MAX_SIZE> _location_hud_text;

    void _update_location_hud();
};

#endif // BASE_GAMEPLAY_SCENE
