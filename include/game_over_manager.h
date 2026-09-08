#ifndef GAME_OVER_MANAGER_H
#define GAME_OVER_MANAGER_H

#include "bn_fixed.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_text_generator.h"
#include "bn_string.h"
#include "bn_vector.h"
#include "bn_optional.h"

#include "controller.h"
#include "scene_type.h"

class game_over_manager
{
public:
    game_over_manager(controller *controller, scene_type restart_scene);
    ~game_over_manager();

    bn::optional<scene_type> update();

private:
    static constexpr int MESSAGE_SPRITE_COUNT = 32;
    static constexpr int MESSAGE_MAX_SIZE = 40;
    static constexpr int FADE_DURATION = 60;
    static constexpr int RESTART_DELAY = 30;

    controller *_controller;
    scene_type _restart_scene;

    bn::regular_bg_ptr _overlay_bg;
    bn::sprite_text_generator _message_text_generator;
    bn::vector<bn::sprite_ptr, MESSAGE_SPRITE_COUNT> _message_sprites;
    bn::string<MESSAGE_MAX_SIZE> _message;
    bn::fixed _fade_alpha;
    int _elapsed_frames;
    bool _started;
};

#endif // GAME_OVER_MANAGER_H