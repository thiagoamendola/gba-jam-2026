#ifndef EASING_H
#define EASING_H

#include "bn_fixed.h"

enum class easing
{
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT,
    EASE_IN_OUT_BACK,
    EASE_IN_OUT_BACK_QUAD,
    EASE_CUSTOM_DODGE,
};

inline bn::fixed apply_easing(bn::fixed t, easing e)
{
    switch(e)
    {
        case easing::EASE_IN:
            return t * t;

        case easing::EASE_OUT:
            return t * (bn::fixed(2) - t);

        case easing::EASE_IN_OUT:
            if(t < bn::fixed(.5))
                return bn::fixed(2) * t * t;
            else
                return bn::fixed(-1) + (bn::fixed(4) - bn::fixed(2) * t) * t;

        case easing::LINEAR:
        default:
            return t;
    }
}

#endif // EASING_H
