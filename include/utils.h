#ifndef UTILS_H
#define UTILS_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"

namespace utils
{
    template<typename Type>
    [[nodiscard]] constexpr Type clamp(Type value, Type minimum, Type maximum)
    {
        if(value < minimum)
        {
            return minimum;
        }

        if(value > maximum)
        {
            return maximum;
        }

        return value;
    }

    [[nodiscard]] inline bool segment_intersects_axis(
        bn::fixed start, bn::fixed delta, bn::fixed minimum, bn::fixed maximum,
        bn::fixed& entry_time, bn::fixed& exit_time)
    {
        // If no delta to next position, just check if start is within the bounds.
        if(delta == 0)
        {
            return start >= minimum && start <= maximum;
        }

        // Get at which fraction of the segment we start and end the possible intersection.
        bn::fixed first_time = (minimum - start) / delta;
        bn::fixed last_time = (maximum - start) / delta;

        // If the first time is greater than the last time, swap them.
        if(first_time > last_time)
        {
            const bn::fixed temporary_time = first_time;
            first_time = last_time;
            last_time = temporary_time;
        }

        // Update the entry and exit times based on the intersection with this axis.
        if(first_time > entry_time)
        {
            entry_time = first_time;
        }
        if(last_time < exit_time)
        {
            exit_time = last_time;
        }

        // If intersection is valid, entry time must be less than or equal to exit time.
        return entry_time <= exit_time;
    }

    [[nodiscard]] inline bool segment_intersects_rectangle(
        const bn::fixed_point& start_position, const bn::fixed_point& end_position,
        const bn::fixed_point& upper_left, const bn::fixed_point& lower_right)
    {
        const bn::fixed delta_x = end_position.x() - start_position.x();
        const bn::fixed delta_y = end_position.y() - start_position.y();
        bn::fixed entry_time = 0;
        bn::fixed exit_time = 1;

        return segment_intersects_axis(
                       start_position.x(), delta_x, upper_left.x(), lower_right.x(), entry_time, exit_time) &&
               segment_intersects_axis(
                       start_position.y(), delta_y, upper_left.y(), lower_right.y(), entry_time, exit_time);
    }
}

#endif