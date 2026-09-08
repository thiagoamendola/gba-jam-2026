#ifndef TEST_SCENE_H
#define TEST_SCENE_H

#include "bn_display.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"

#include "base_scene.h"
#include "scenario.h"
#include "controller.h"
#include "player.h"
#include "dog_enemy.h"
#include "exit_route.h"
#include "walls.h"
#include "game_over_manager.h"

class test_scene : public base_scene
{
public:
    test_scene();
    virtual ~test_scene();

    bn::optional<scene_type> update() override;

private:
    static constexpr int LOCATION_HUD_MAX_SPRITES = 8;
    static constexpr int LOCATION_HUD_TEXT_MAX_SIZE = 32;
    static constexpr int LOCATION_HUD_MARGIN = 4;
    static constexpr int LOCATION_HUD_CHARACTER_HEIGHT = 8;
    static constexpr bn::fixed LOCATION_HUD_X = (bn::display::width() / 2) - LOCATION_HUD_MARGIN;
    static constexpr bn::fixed LOCATION_HUD_Y =
            (-bn::display::height() / 2) + LOCATION_HUD_MARGIN + (LOCATION_HUD_CHARACTER_HEIGHT / 2);

    controller _controller;
    scenario _scenario;
    player _player;
    walls _walls;

    dog_enemy _dog1;
    dog_enemy _dog2;
    exit_route _exit_route;
    game_over_manager _game_over_manager;

    bn::sprite_text_generator _location_hud_text_generator;
    bn::vector<bn::sprite_ptr, LOCATION_HUD_MAX_SPRITES> _location_hud_sprites;
    bn::string<LOCATION_HUD_TEXT_MAX_SIZE> _location_hud_text;

    void _update_location_hud();
};

#endif // TEST_SCENE_H