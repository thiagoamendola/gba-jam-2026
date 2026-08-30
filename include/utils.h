#ifndef UTILS_H
#define UTILS_H

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
}

#endif