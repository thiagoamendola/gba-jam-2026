#include "controller.h"

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_math.h"
#include "bn_keypad.h"

bn::fixed_point controller::get_normalized_directional()
{
    bn::fixed_point raw_dir_input(0.0, 0.0);

    if (bn::keypad::up_held())
    {
        raw_dir_input.set_y(-1.0f);
    }
    else if (bn::keypad::down_held())
    {
        raw_dir_input.set_y(1.0f);
    }

    if (bn::keypad::right_held())
    {
        raw_dir_input.set_x(1.0);
    }
    else if (bn::keypad::left_held())
    {
        raw_dir_input.set_x(-1.0);
    }

    // Normalize manually

    raw_dir_input = unit_vector(raw_dir_input);

    return raw_dir_input;
}

bn::fixed_point controller::get_smooth_directional()
{
    bn::fixed_point current_raw_dir_input = get_normalized_directional();

    if (current_raw_dir_input != _previous_raw_dir_input)
    {
        _interp = 0;
    }

    // Move interpolation counter
    _interp += INTERP_STEP;
    _interp = _interp > 1 ? 1 : _interp;

    // Grab diff
    bn::fixed_point diff_vec = current_raw_dir_input - _smooth_dir_input;

    // Calculate fractional diff
    diff_vec *= _interp;

    // Update smooth dir
    _smooth_dir_input += diff_vec;
    
    // Make it zero when raw input is zero
    if (abs(_smooth_dir_input.x()) < 0.02 && abs(_smooth_dir_input.y()) < 0.02)
    {
        _smooth_dir_input.set_x(0);
        _smooth_dir_input.set_y(0);
    }

    // Update previous raw value.
    _previous_raw_dir_input = current_raw_dir_input;

    return _smooth_dir_input;
}

// <-- MOVE SOMEWHERE ELSE. MAYBE UTILS
bn::fixed_point controller::unit_vector(bn::fixed_point original_vector)
{
    // Uses division, so not super efficient.
    bn::fixed magnitude = bn::sqrt((original_vector.x() * original_vector.x()) + (original_vector.y() * original_vector.y()));
    magnitude = magnitude == 0 ? 1 : magnitude;
    return original_vector / magnitude;
}

bool controller::is_any_button_pressed() const
{
    return bn::keypad::any_pressed();
}

bool controller::is_start_pressed() const
{
    return bn::keypad::start_pressed();
}