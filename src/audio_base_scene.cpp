#include "audio_base_scene.h"

#include "bn_regular_bg_items_audio_scene_bg.h"

audio_base_scene::audio_base_scene(int frames_to_end, scene_type next_scene, game_state* game_state) :
    _frames_to_end(frames_to_end),
    _next_scene(next_scene),
    _audio_scene_bg(bn::regular_bg_items::audio_scene_bg.create_bg())
{
}

audio_base_scene::~audio_base_scene()
{
}

bn::optional<scene_type> audio_base_scene::update()
{
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

    // End scene if reached the end frame.
    if (_current_frame >= _frames_to_end)
    {
        return _next_scene;
    }

    _current_frame++;

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