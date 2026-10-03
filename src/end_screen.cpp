#include "end_screen.h"

#include "bn_affine_bg_items_end_screen.h"
#include "bn_affine_bg_items_end_screen_inner.h"
#include "bn_backdrop.h"
#include "bn_color.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_sound_items.h"

#include "bn_sprite_items_black_tile.h"

end_screen::end_screen() :
    _background(bn::affine_bg_items::end_screen.create_bg()),
    _inner_background(bn::affine_bg_items::end_screen_inner.create_bg()),
    _eyelid(bn::sprite_items::black_tile, EYELID_DURATION_FRAMES)
{
    bn::backdrop::set_color(bn::color(0, 0, 0));
    _background.set_position(0, 0);
    _inner_background.set_position(0, 0);
    _background.set_scale(START_SCALE);
    _inner_background.set_scale(INNER_START_SCALE);
    _background.set_priority(2);
    _inner_background.set_priority(1);
    _background.set_wrapping_enabled(false);
    _inner_background.set_wrapping_enabled(false);
    bn::sound_items::jumpscared.play();
}

bn::optional<scene_type> end_screen::update()
{
    _eyelid.update();

    // Starts music with the same delay as gameplay scenes.
    if (!_music_start_handled)
    {
        ++_music_start_delay_frames;

        if (_music_start_delay_frames >= MUSIC_START_DELAY_FRAMES)
        {
            bn::music_items::supernovaexplosion_2.play(0.7);
            _music_start_handled = true;
        }
    }

    // Handle scaling.
    if (_elapsed_frames < SCALE_FRAMES)
    {
        ++_elapsed_frames;
        const bn::fixed progress = bn::fixed(_elapsed_frames) / SCALE_FRAMES;
        const bn::fixed background_scale = START_SCALE + (END_SCALE - START_SCALE) * progress;
        _background.set_scale(background_scale);
        // Maintain the initial scale ratio so both centered layers stay registered.
        _inner_background.set_scale(INNER_START_SCALE * background_scale.safe_division(START_SCALE));

        return bn::nullopt;
    }

    // Hide both affine layers to reveal the black backdrop before the next scene.
    if (_black_wait_frames < BLACK_WAIT_FRAMES)
    {
        if (_black_wait_frames == 0)
        {
            _background.set_visible(false);
            _inner_background.set_visible(false);
        }

        ++_black_wait_frames;
        return bn::nullopt;
    }

    // Leave to title screen.
    bn::music::stop();
    return scene_type::TITLE;
}