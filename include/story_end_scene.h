#ifndef STORY_END_SCENE_H
#define STORY_END_SCENE_H

#include "audio_base_scene.h"

#include "bn_sound_items.h"

class story_end_scene : public audio_base_scene
{
public:
    explicit story_end_scene(game_state* game_state) :
        audio_base_scene(1250, scene_type::END_SCREEN, game_state)
    {
        // play_audio(bn::sound_items::crowd_talking, 30);
        play_audio(bn::sound_items::door_close, 30);
        play_audio(bn::sound_items::brushing_teeth, 150);
        play_audio(bn::sound_items::door_close, 350);
        play_audio(bn::sound_items::exhale, 450);
        play_audio(bn::sound_items::digital_alarm, 800);
        play_audio(bn::sound_items::exhale, 950);

    };
};

#endif // STORY_END_SCENE_H
