#ifndef STORY_1_SCENE_H
#define STORY_1_SCENE_H

#include "audio_base_scene.h"

#include "bn_sound_items.h"

class story_1_scene : public audio_base_scene
{
public:
    story_1_scene() :
        audio_base_scene(570, scene_type::STAGE_2)
    {
        play_audio(bn::sound_items::brushing_teeth, 30);
        play_audio(bn::sound_items::filling_sink, 90);
        play_audio(bn::sound_items::door_close, 270);
        play_audio(bn::sound_items::crowd_talking, 330); // <-- Maybe replace?
        // <-- Add some transport sounds 
    };
};

#endif // STORY_1_SCENE_H