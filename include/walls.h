#ifndef WALLS_H
#define WALLS_H

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

class scenario;

class walls
{
public:
    walls(scenario* associated_scenario);

    void create_horizontal_wall(const bn::fixed_point& start_position, bn::fixed end_x);
    void create_vertical_wall(const bn::fixed_point& start_position, bn::fixed end_y);

    [[nodiscard]] bn::fixed_point resolve_movement(
            const bn::fixed_point& collider_position, bn::fixed collider_radius,
            const bn::fixed_point& movement) const;

    [[nodiscard]] bool has_wall_between(
            const bn::fixed_point& start_position, const bn::fixed_point& end_position) const;

    void update(const bn::fixed_point& player_movement);

private:
    struct wall_cell
    {
        bn::fixed_point position;
        bn::sprite_ptr sprite;
    };

    struct wall_rectangle
    {
        bn::fixed_point upper_left;
        bn::fixed_point lower_right;
    };

    enum wall_graphics_index
    {
        HORIZONTAL_WALL_INDEX,
        VERTICAL_WALL_INDEX,
        CONNECTION_WALL_INDEX,
    };

    static constexpr int WALL_CELL_SIZE = 8;
    static constexpr int WALL_SPRITE_HALF_SIZE = 4;
    static constexpr int MAX_WALL_CELLS = 128;
    static constexpr int MAX_WALL_RECTANGLES = 128;

    scenario* _associated_scenario = nullptr;
    
    bn::vector<wall_cell, MAX_WALL_CELLS> _cells;
    bn::vector<wall_rectangle, MAX_WALL_RECTANGLES> _rectangles;

    [[nodiscard]] bn::fixed _resolve_horizontal_movement(
            const bn::fixed_point& collider_position, bn::fixed collider_radius, bn::fixed movement_x) const;

    [[nodiscard]] bn::fixed _resolve_vertical_movement(
            const bn::fixed_point& collider_position, bn::fixed collider_radius, bn::fixed movement_y) const;

    void _add_or_upgrade_wall(const bn::fixed_point& position, int graphics_index);
    void _add_wall_rectangle(const bn::fixed_point& upper_left, const bn::fixed_point& lower_right);
};

#endif