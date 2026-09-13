#ifndef STORY_3_SCENE_H
#define STORY_3_SCENE_H

#include "audio_base_scene.h"

#include "bn_sound_items.h"

class story_3_scene : public audio_base_scene
{
public:
    explicit story_3_scene(game_state* game_state) :
        audio_base_scene(400, scene_type::STAGE_2, game_state)
    {
        play_audio(bn::sound_items::crowd_talking, 30);
        play_audio(bn::sound_items::coffee_cup, 180);
        play_audio(bn::sound_items::coffee_cup, 210);
        play_audio(bn::sound_items::crowd_talking, 180);
        play_audio(bn::sound_items::coffee_cup, 270);
    };
};

#endif // STORY_3_SCENE_H
