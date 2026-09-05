#ifndef END_TRANSITION_MANAGER_H
#define END_TRANSITION_MANAGER_H

#include "bn_affine_bg_ptr.h"
#include "bn_array.h"
#include "bn_bg_palette_ptr.h"
#include "bn_color.h"
#include "bn_fixed.h"
#include "bn_fixed_point.h"
#include "bn_optional.h"
#include "bn_regular_bg_item.h"
#include "bn_span.h"
#include "bn_tile.h"
#include "bn_vector.h"

class end_transition_manager
{
public:
    end_transition_manager(const bn::regular_bg_item& bg_item, const bn::regular_bg_item& walls_item);
    ~end_transition_manager();

    void start(const bn::fixed_point& captured_position, bn::color backdrop_color);

        [[nodiscard]] bool update(const bn::fixed_point& snapshot_position, bn::fixed scale);
    [[nodiscard]] bool started() const;

private:
    static constexpr int TRANSITION_MAP_DIMENSION = 32;
    static constexpr int TRANSITION_TILE_SIZE = 8;
    static constexpr int TRANSITION_HALF_SIZE = (TRANSITION_MAP_DIMENSION * TRANSITION_TILE_SIZE) / 2;
    static constexpr int TRANSITION_MAX_SOURCE_TILE_VARIANTS = 255;
    static constexpr int TRANSITION_MAX_PALETTE_COLORS = 256;
    static constexpr int PALETTE_COLORS_PER_BANK = 16;

    struct transition_tile_variant
    {
        int source_tile_index;
        int source_palette_index;
        bool horizontal_flip;
        bool vertical_flip;
    };

    using transition_color_indexes = bn::array<int, TRANSITION_MAX_PALETTE_COLORS>;

    // Source stage data retained for constructing the runtime snapshot after normal backgrounds are released.
    const bn::regular_bg_item* _bg_item;
    const bn::regular_bg_item* _walls_item;

    // Temporary affine layers that make up the zooming transition image.
    bn::optional<bn::affine_bg_ptr> _bg;
    bn::optional<bn::affine_bg_ptr> _walls_bg;

    // Stage position and backdrop color captured when the transition starts.
    bn::fixed_point _captured_position;
    bn::color _backdrop_color;
    bool _started;

    // Finds an existing BPP8 palette index for a color or appends it to the transition palette.
    // Returns -1 when no palette entry remains.
    [[nodiscard]] static int _add_transition_palette_color(
            bn::color color, bn::array<bn::color, TRANSITION_MAX_PALETTE_COLORS>& transition_palette_colors,
            int& transition_palette_colors_count);
    // Adds colors from one BPP4 source palette to the shared BPP8 transition palette.
    // Each source palette bank's transparent color maps to transparent_color_index.
    [[nodiscard]] static bool _add_palette_colors(
            const bn::regular_bg_item& source_item, transition_color_indexes& source_color_indexes,
            bn::array<bn::color, TRANSITION_MAX_PALETTE_COLORS>& transition_palette_colors,
            int& transition_palette_colors_count, int transparent_color_index);

    // Divides toward negative infinity so snapshot tiles remain aligned for negative source coordinates.
    [[nodiscard]] static int _floor_division(int value, int divisor);

    // Finds the matching source-tile, palette-bank, and flip combination, or returns -1 if it is new.
    [[nodiscard]] static int _find_transition_tile_variant(
            const bn::vector<transition_tile_variant, TRANSITION_MAX_SOURCE_TILE_VARIANTS>& variants,
            const transition_tile_variant& variant);

    // Converts one source map cell into a snapshot tile index, adding a tile variant when necessary.
    // Out-of-bounds source coordinates select tile zero, the transition background tile.
    [[nodiscard]] static bool _transition_tile_index(
            const bn::regular_bg_item& source_item, const transition_color_indexes& source_color_indexes,
            int source_x, int source_y,
            bn::vector<transition_tile_variant, TRANSITION_MAX_SOURCE_TILE_VARIANTS>& variants,
            int& output_tile_index);

    // Converts the collected BPP4 tile variants into BPP8 pixel data in allocated tile VRAM.
    // Tile zero is cleared or filled with the supplied transition background color.
    [[nodiscard]] static bool _write_transition_tiles(
            const bn::regular_bg_item& source_item, const transition_color_indexes& source_color_indexes,
            const bn::vector<transition_tile_variant, TRANSITION_MAX_SOURCE_TILE_VARIANTS>& variants,
            int background_color_index, bn::span<bn::tile>& destination_tiles);

    // Builds one 256x256 affine snapshot layer around the captured stage position and stores it in destination_bg.
    [[nodiscard]] static bool _create_transition_layer(
            const bn::regular_bg_item& source_item, const transition_color_indexes& source_color_indexes,
            const bn::bg_palette_ptr& transition_palette, const bn::fixed_point& captured_position,
            int background_color_index, int priority, bn::optional<bn::affine_bg_ptr>& destination_bg);

    // Creates the shared BPP8 palette plus terrain and walls snapshot layers on the first animation frame.
    [[nodiscard]] bool _create_backgrounds();

    // Shows the snapshot layers and gameplay sprites only inside the internal rectangular window.
    void _configure_window();

    // Positions and shrinks the internal window to match the current snapshot transform.
    void _update_window(const bn::fixed_point& snapshot_position, bn::fixed scale);
};

#endif // END_TRANSITION_MANAGER_H