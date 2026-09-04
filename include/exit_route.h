#ifndef EXIT_ROUTE_H
#define EXIT_ROUTE_H

#include <initializer_list>

#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_vector.h"

#include "scene_type.h"

class dog_enemy;
class player;

class exit_route
{
public:
    exit_route(
            const player* player, const bn::fixed_point& position,
        std::initializer_list<const dog_enemy*> dogs, scene_type next_scene);
    ~exit_route();

    [[nodiscard]] bn::optional<scene_type> update(const bn::fixed_point& player_movement);

private:
    static constexpr int MAX_DOGS = 8;
    static constexpr bn::fixed CLEAR_DISTANCE_SQUARED = 10 * 10;

    const player* _player;
    bn::fixed_point _position;
    bn::sprite_ptr _sprite;
    bn::vector<const dog_enemy*, MAX_DOGS> _dogs;
    scene_type _next_scene;
    bool _stage_cleared;

    [[nodiscard]] bool _all_dogs_dead() const;
};

#endif // EXIT_ROUTE_H