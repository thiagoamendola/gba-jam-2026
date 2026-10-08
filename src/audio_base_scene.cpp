#include "audio_base_scene.h"

#include "bn_blending.h"
#include "bn_regular_bg_items_audio_scene_bg.h"
#include "bn_regular_bg_items_black.h"

audio_base_scene::audio_base_scene(int frames_to_end, scene_type next_scene, game_state* game_state) :
    _frames_to_end(frames_to_end),
    _next_scene(next_scene),
    _audio_scene_bg(bn::regular_bg_items::audio_scene_bg.create_bg()),
    _black_cover_bg(bn::regular_bg_items::black.create_bg())
{
    _audio_scene_bg.set_priority(1);
    _black_cover_bg.set_priority(0);
    _black_cover_bg.set_blending_enabled(true);
    bn::blending::set_transparency_alpha(_fade_alpha);
}

audio_base_scene::~audio_base_scene()
{
    bn::blending::restore();
}

bn::optional<scene_type> audio_base_scene::update()
{
    if (_closing)
    {
        _fade_alpha += bn::fixed(1) / FADE_DURATION;

        if (_fade_alpha > 1)
        {
            _fade_alpha = 1;
        }

        bn::blending::set_transparency_alpha(_fade_alpha);

        if (++_current_frame >= FADE_DURATION)
        {
            return _next_scene;
        }

        return bn::nullopt;
    }

    if (_current_frame < FADE_DURATION)
    {
        _fade_alpha = 1 - bn::fixed(_current_frame + 1) / FADE_DURATION;
        bn::blending::set_transparency_alpha(_fade_alpha);
    }

    if (_current_frame % AUDIO_SCENE_BG_MOVE_INTERVAL == 0)
    {
        _audio_scene_bg.set_x(_audio_scene_bg.x() - 1);
    }

    // Start all events applicable for current frame.
    for (audio_event& event : _audio_events)
    {
        if (!event.played && _current_frame >= event.start_frame)
        {
            event.audio.play();
            event.played = true;
        }
    }

    if (_current_frame >= _frames_to_end)
    {
        _closing = true;
        _current_frame = 0;
    }
    else
    {
        _current_frame++;
    }

    return bn::nullopt;
}

void audio_base_scene::play_audio(const bn::sound_item& audio, int start_frame)
{
    // Add audio event to the list if possible.
    if (_audio_events.size() < MAX_AUDIO_EVENTS)
    {
        _audio_events.push_back({ audio, start_frame, false });
    }
}