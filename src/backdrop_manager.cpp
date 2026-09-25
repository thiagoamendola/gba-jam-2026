#include "backdrop_manager.h"

#include "bn_backdrop.h"

backdrop_manager::backdrop_manager() :
    _color1(0, 0, 0),
    _color2(0, 0, 0),
    _elapsed_frames(0),
    _fading_to_color2(true),
    _fade_enabled(false)
{
}

backdrop_manager::~backdrop_manager()
{
}

void backdrop_manager::update()
{
    if (!_fade_enabled)
    {
        return;
    }

    _elapsed_frames++;

    // Select appropriate color.
    const bn::color &start_color = _fading_to_color2 ? _color1 : _color2;
    const bn::color &end_color = _fading_to_color2 ? _color2 : _color1;
    int remaining_frames = FRAMES_TO_NEXT_COLOR - _elapsed_frames;

    // Calculate the interpolated color.
    bn::backdrop::set_color(bn::color(
        (start_color.red() * remaining_frames + end_color.red() * _elapsed_frames) /
            FRAMES_TO_NEXT_COLOR,
        (start_color.green() * remaining_frames + end_color.green() * _elapsed_frames) /
            FRAMES_TO_NEXT_COLOR,
        (start_color.blue() * remaining_frames + end_color.blue() * _elapsed_frames) /
            FRAMES_TO_NEXT_COLOR));

    // Loop back animation at the end.
    if (_elapsed_frames == FRAMES_TO_NEXT_COLOR)
    {
        _elapsed_frames = 0;
        _fading_to_color2 = !_fading_to_color2;
    }
}

void backdrop_manager::set_color_fade(bn::color color1, bn::color color2)
{
    // Set up color fade variables.
    _color1 = color1;
    _color2 = color2;
    _elapsed_frames = 0;
    _fading_to_color2 = true;
    _fade_enabled = true;
    bn::backdrop::set_color(_color1);
}

void backdrop_manager::stop_fade()
{
    _fade_enabled = false;
}