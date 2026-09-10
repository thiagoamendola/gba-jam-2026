#include "story_1_scene.h"

#include "bn_sound_items.h"

story_1_scene::story_1_scene() :
    audio_base_scene(570, scene_type::TEST)
{
    play_audio(bn::sound_items::brushing_teeth, 30);
    play_audio(bn::sound_items::filling_sink, 90);
    play_audio(bn::sound_items::door_close, 270);
    play_audio(bn::sound_items::crowd_talking, 330); // <-- Maybe replace?
    // <-- Add some transport sounds 
}
