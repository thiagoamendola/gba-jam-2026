#include "warnings_scene.h"

#include "bn_blending.h"
#include "bn_regular_bg_items_warning_photosensitive.h"
#include "bn_regular_bg_items_warning_phones.h"

warnings_scene::warnings_scene() :
    _photosensitive_background(bn::regular_bg_items::warning_photosensitive.create_bg()),
    _phones_background(bn::regular_bg_items::warning_phones.create_bg())
{
    // Global fades only affect backgrounds that opt into blending.
    _photosensitive_background.set_priority(0);
    _photosensitive_background.set_blending_enabled(true);
    _phones_background.set_priority(0);
    _phones_background.set_blending_enabled(true);
    _phones_background.set_visible(false);

    bn::blending::set_black_fade_color();
    bn::blending::set_fade_alpha(1);
}

bn::optional<scene_type> warnings_scene::update()
{
    switch (_phase)
    {
        case phase::PHOTOSENSITIVE_FADE_IN:
        {
            _phase_elapsed_frames++;
            bn::blending::set_fade_alpha(1 - bn::fixed(_phase_elapsed_frames) / FADE_FRAMES);

            if (_phase_elapsed_frames >= FADE_FRAMES)
            {
                _phase = phase::PHOTOSENSITIVE_SHOW;
                _phase_elapsed_frames = 0;
            }

            break;
        }

        case phase::PHOTOSENSITIVE_SHOW:
        {
            if (_phase_elapsed_frames >= SHOW_FRAMES)
            {
                _phase = phase::PHOTOSENSITIVE_FADE_OUT;
                _phase_elapsed_frames = 0;
            }
            _phase_elapsed_frames++;

            break;
        }

        case phase::PHOTOSENSITIVE_FADE_OUT:
        {
            _phase_elapsed_frames++;
            bn::blending::set_fade_alpha(bn::fixed(_phase_elapsed_frames) / FADE_FRAMES);

            if (_phase_elapsed_frames >= FADE_FRAMES)
            {
                _photosensitive_background.set_visible(false);
                _phones_background.set_visible(true);
                _phase = phase::PHONES_FADE_IN;
                _phase_elapsed_frames = 0;
            }

            break;
        }

        case phase::PHONES_FADE_IN:
        {
            _phase_elapsed_frames++;
            bn::blending::set_fade_alpha(1 - bn::fixed(_phase_elapsed_frames) / FADE_FRAMES);

            if (_phase_elapsed_frames >= FADE_FRAMES)
            {
                _phase = phase::PHONES_SHOW;
                _phase_elapsed_frames = 0;
            }

            break;
        }

        case phase::PHONES_SHOW:
        {
            if (++_phase_elapsed_frames >= SHOW_FRAMES)
            {
                _phase = phase::PHONES_FADE_OUT;
                _phase_elapsed_frames = 0;
            }

            break;
        }

        case phase::PHONES_FADE_OUT:
        {
            _phase_elapsed_frames++;
            bn::blending::set_fade_alpha(bn::fixed(_phase_elapsed_frames) / FADE_FRAMES);

            if (_phase_elapsed_frames >= FADE_FRAMES)
            {
                return scene_type::TITLE;
            }

            break;
        }

        default:
            break;
    }

    return bn::nullopt;
}