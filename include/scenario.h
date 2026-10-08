#ifndef SCENARIO_H
#define SCENARIO_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_regular_bg_item.h"
#include "bn_regular_bg_ptr.h"
#include "bn_size.h"

#include "end_transition_manager.h"

class scenario
{
public:
    scenario(const bn::regular_bg_item& bg_item, const bn::regular_bg_item& walls_item,
        const bn::fixed_point& initial_position);
    ~scenario();

    void update(bn::fixed_point movement);
    
    void start_exit_transition();
    [[nodiscard]] bool update_exit_transition();
    [[nodiscard]] bool exit_transition_fade_in_complete() const;

    const bn::fixed_point& initial_position() const { return _initial_position; }
    [[nodiscard]] bn::fixed_point walls_image_to_world_position(
            const bn::fixed_point& image_position) const;

private:
    bn::optional<bn::regular_bg_ptr> _bg;
    bn::optional<bn::regular_bg_ptr> _walls_bg;
    end_transition_manager _end_transition_manager;

    bn::fixed_point _initial_position;
    bn::fixed_point _current_position;
    bn::size _walls_dimensions;
    bool _exit_transition_stage_hidden = false;

    void _configure_regular_bg_window();
    void _update_bg_window();
};

#endif // SCENARIO_H