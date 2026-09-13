#ifndef STORY_2_SCENE_H
#define STORY_2_SCENE_H

#include "audio_base_scene.h"

#include "bn_sound_items.h"

class story_2_scene : public audio_base_scene
{
public:
    explicit story_2_scene(game_state* game_state) :
        audio_base_scene(570, scene_type::STAGE_3, game_state)
    {
        play_audio(bn::sound_items::car_traffic, 30);
        play_audio(bn::sound_items::crowd_talking, 180);
        play_audio(bn::sound_items::door_close, 210);
        play_audio(bn::sound_items::keyboard_1, 390);
    };
};

#endif // STORY_2_SCENE_H
