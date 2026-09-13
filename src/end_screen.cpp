#include "end_screen.h"

#include "bn_affine_bg_items_end_screen.h"
#include "bn_music.h"
#include "bn_music_items.h"
#include "bn_regular_bg_items_black.h"
#include "bn_sound_items.h"
#include "bn_sprite_items_black_tile.h"

end_screen::end_screen() :
    _background(bn::affine_bg_items::end_screen.create_bg()),
    _black_background(bn::regular_bg_items::black.create_bg()),
    _eyelid(bn::sprite_items::black_tile, EYELID_DURATION_FRAMES)
{
    _background.set_scale(START_SCALE);
    _black_background.set_priority(0);
    _black_background.set_visible(false);
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
        _background.set_scale(START_SCALE + (END_SCALE - START_SCALE) * progress);

        return bn::nullopt;
    }

    // Handle black wait before next scene.
    if (_black_wait_frames < BLACK_WAIT_FRAMES)
    {
        if (_black_wait_frames == 0)
        {
            _black_background.set_visible(true);
        }

        ++_black_wait_frames;
        return bn::nullopt;
    }

    // Leave to title screen.
    bn::music::stop();
    return scene_type::TITLE;
}