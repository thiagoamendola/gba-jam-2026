#include "game_over_manager.h"

#include "bn_blending.h"
#include "bn_fixed.h"

#include "common_variable_8x8_sprite_font.h"
#include "bn_regular_bg_items_red.h"

game_over_manager::game_over_manager(controller* controller, scene_type restart_scene)
    : _controller(controller),
    _restart_scene(restart_scene),
      _overlay_bg(bn::regular_bg_items::red.create_bg()),
      _message_text_generator(common::variable_8x8_sprite_font),
      _message("Press any button to try again."),
      _fade_alpha(0),
      _elapsed_frames(0),
      _started(false)
{
    _overlay_bg.set_priority(0);
    _overlay_bg.set_visible(false);

    _message_text_generator.set_center_alignment();
    _message_text_generator.set_bg_priority(0);
    _message_text_generator.generate(0, 68, _message, _message_sprites);
    _message_sprites.clear();
}

game_over_manager::~game_over_manager()
{
    bn::blending::set_transparency_alpha(0);
}

bn::optional<scene_type> game_over_manager::update()
{
    // Handle start if not started
    if(!_started)
    {
        _started = true;
        _overlay_bg.set_visible(true);
        _overlay_bg.set_blending_enabled(true);
    }

    // Handle message display after delay.
    if(_elapsed_frames < RESTART_DELAY)
    {
        ++_elapsed_frames;

        if (_elapsed_frames >= RESTART_DELAY)
        {
            _message_text_generator.generate(0, 68, _message, _message_sprites); // <-- Animate this
        }
    }

    // Restart scene if any button pressed and delay is over.
    if (_elapsed_frames >= RESTART_DELAY && _controller->is_any_button_pressed())
    {
        return _restart_scene;
    }

    // If not fully faded, handle fading.
    if(_fade_alpha < 1)
    {
        _fade_alpha += bn::fixed(1) / FADE_DURATION;

        if(_fade_alpha > 1)
        {
            _fade_alpha = 1;
        }
    }

    bn::blending::set_transparency_alpha(_fade_alpha);

    return bn::nullopt;
}