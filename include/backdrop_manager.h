#ifndef BACKDROP_MANAGER_H
#define BACKDROP_MANAGER_H

#include "bn_color.h"

class backdrop_manager
{
public:
    backdrop_manager();
    ~backdrop_manager();

    void update();

    void set_color_fade(bn::color color1, bn::color color2);
    void stop_fade();

private:
    static constexpr int FRAMES_TO_NEXT_COLOR = 240;

    bn::color _color1;
    bn::color _color2;
    int _elapsed_frames;
    bool _fading_to_color2;
    bool _fade_enabled;
};

#endif // BACKDROP_MANAGER_H