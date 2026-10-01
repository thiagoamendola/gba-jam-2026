#ifndef STORY_1_SCENE_H
#define STORY_1_SCENE_H

#include "audio_base_scene.h"

#include "bn_sound_items.h"

class story_1_scene : public audio_base_scene
{
public:
    explicit story_1_scene(game_state* game_state) :
        audio_base_scene(710, scene_type::STAGE_2, game_state)
    {
        play_audio(bn::sound_items::brushing_teeth, 35);
        // play_audio(bn::sound_items::filling_sink, 190);
        play_audio(bn::sound_items::door_close, 250);
        play_audio(bn::sound_items::bus_announce, 330);
        play_audio(bn::sound_items::crowd_talking, 380);
        play_audio(bn::sound_items::crowd_talking, 500);
    };
};

#endif // STORY_1_SCENE_H