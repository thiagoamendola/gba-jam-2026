#ifndef AUDIO_BASE_SCENE_H
#define AUDIO_BASE_SCENE_H

#include "bn_fixed.h"
#include "bn_optional.h"
#include "bn_regular_bg_ptr.h"
#include "bn_sound_item.h"
#include "bn_vector.h"

#include "base_scene.h"

struct game_state;

class audio_base_scene : public base_scene
{
public:
    audio_base_scene(int frames_to_end, scene_type next_scene, game_state* game_state);
    virtual ~audio_base_scene() = 0;

    bn::optional<scene_type> update() override;

protected:
    void play_audio(const bn::sound_item& audio, int start_frame);

private:
    struct audio_event
    {
        bn::sound_item audio;
        int start_frame;
        bool played;
    };

    static constexpr int MAX_AUDIO_EVENTS = 16;
    static constexpr int AUDIO_SCENE_BG_MOVE_INTERVAL = 4;
    static constexpr int FADE_DURATION = 60;

    int _frames_to_end;
    int _current_frame = 0;
    scene_type _next_scene;
    bn::regular_bg_ptr _audio_scene_bg;
    bn::regular_bg_ptr _black_cover_bg;
    bn::fixed _fade_alpha = 1;
    bool _closing = false;
    bn::vector<audio_event, MAX_AUDIO_EVENTS> _audio_events;
};

#endif // AUDIO_BASE_SCENE_H