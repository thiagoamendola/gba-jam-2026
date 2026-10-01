#ifndef STORY_3_SCENE_H
#define STORY_3_SCENE_H

#include "audio_base_scene.h"

#include "bn_sound_items.h"

class story_3_scene : public audio_base_scene
{
public:
    explicit story_3_scene(game_state* game_state) :
        audio_base_scene(1250, scene_type::STAGE_4, game_state)
    {
        play_audio(bn::sound_items::keyboard_1, 30);
        play_audio(bn::sound_items::keyboard_1, 130);
        play_audio(bn::sound_items::digital_alarm, 250);
        play_audio(bn::sound_items::microwave_1, 450);
        play_audio(bn::sound_items::microwave_2, 600);
        play_audio(bn::sound_items::microwave_3, 750);
        play_audio(bn::sound_items::coffee_cup, 900);
        play_audio(bn::sound_items::coffee_cup, 980);
        play_audio(bn::sound_items::coffee_cup, 1040);
    };
};

#endif // STORY_3_SCENE_H
