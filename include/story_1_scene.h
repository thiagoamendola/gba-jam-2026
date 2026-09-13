#ifndef STORY_1_SCENE_H
#define STORY_1_SCENE_H

#include "audio_base_scene.h"

#include "bn_sound_items.h"

class story_1_scene : public audio_base_scene
{
public:
    explicit story_1_scene(game_state* game_state) :
        audio_base_scene(570, scene_type::STAGE_2, game_state)
    {
        play_audio(bn::sound_items::brushing_teeth, 30);
        play_audio(bn::sound_items::filling_sink, 100);
        play_audio(bn::sound_items::door_close, 280);
        play_audio(bn::sound_items::crowd_talking, 360);
    };
};

#endif // STORY_1_SCENE_H