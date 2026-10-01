#ifndef STORY_4_SCENE_H
#define STORY_4_SCENE_H

#include "audio_base_scene.h"

class story_4_scene : public audio_base_scene
{
public:
    explicit story_4_scene(game_state* game_state) :
        audio_base_scene(1300, scene_type::STAGE_5, game_state)
    {
        play_audio(bn::sound_items::keyboard_1, 30);
        play_audio(bn::sound_items::keyboard_1, 130);
        play_audio(bn::sound_items::digital_alarm, 200);
        play_audio(bn::sound_items::door_close, 370);
        play_audio(bn::sound_items::car_traffic, 450);
        play_audio(bn::sound_items::bus_announce, 520);
        play_audio(bn::sound_items::crowd_talking, 580);
        play_audio(bn::sound_items::crowd_talking, 730);
        play_audio(bn::sound_items::door_close, 940);
        play_audio(bn::sound_items::exhale, 1100);


    };
};

#endif // STORY_4_SCENE_H