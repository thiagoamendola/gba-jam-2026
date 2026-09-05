#include "end_transition_manager.h"

#include <cstdint>

#include "bn_affine_bg_map_ptr.h"
#include "bn_affine_bg_tiles_ptr.h"
#include "bn_array.h"
#include "bn_bg_palette_item.h"
#include "bn_bg_palette_ptr.h"
#include "bn_bpp_mode.h"
#include "bn_display.h"
#include "bn_log.h"
#include "bn_rect_window.h"
#include "bn_regular_bg_map_cell_info.h"
#include "bn_size.h"
#include "bn_span.h"
#include "bn_tile.h"
#include "bn_utility.h"
#include "bn_vector.h"
#include "bn_window.h"

int end_transition_manager::_add_transition_palette_color(
        bn::color color, bn::array<bn::color, TRANSITION_MAX_PALETTE_COLORS>& transition_palette_colors,
        int& transition_palette_colors_count)
{
    for(int index = 1; index < transition_palette_colors_count; ++index)
    {
        if(transition_palette_colors[index] == color)
        {
            return index;
        }
    }

    if(transition_palette_colors_count >= TRANSITION_MAX_PALETTE_COLORS)
    {
        return -1;
    }

    const int transition_color_index = transition_palette_colors_count;
    transition_palette_colors[transition_color_index] = color;
    ++transition_palette_colors_count;
    return transition_color_index;
}

int end_transition_manager::_floor_division(int value, int divisor)
{
    if(value >= 0)
    {
        return value / divisor;
    }

    return -(((-value) + divisor - 1) / divisor);
}

int end_transition_manager::_find_transition_tile_variant(
        const bn::vector<transition_tile_variant, TRANSITION_MAX_SOURCE_TILE_VARIANTS>& variants,
        const transition_tile_variant& variant)
{
    for(int index = 0; index < variants.size(); ++index)
    {
        const transition_tile_variant& existing_variant = variants[index];

        if(existing_variant.source_tile_index == variant.source_tile_index &&
           existing_variant.source_palette_index == variant.source_palette_index &&
           existing_variant.horizontal_flip == variant.horizontal_flip &&
           existing_variant.vertical_flip == variant.vertical_flip)
        {
            return index;
        }
    }

    return -1;
}

bool end_transition_manager::_add_palette_colors(
        const bn::regular_bg_item& source_item, transition_color_indexes& source_color_indexes,
        bn::array<bn::color, TRANSITION_MAX_PALETTE_COLORS>& transition_palette_colors,
        int& transition_palette_colors_count, int transparent_color_index)
{
    const bn::span<const bn::color>& source_colors = source_item.palette_item().colors_ref();

    if(source_colors.size() > source_color_indexes.size() ||
       source_colors.size() % PALETTE_COLORS_PER_BANK != 0 ||
       transparent_color_index < 0 || transparent_color_index >= TRANSITION_MAX_PALETTE_COLORS)
    {
        return false;
    }

    for(int source_color_index = 0; source_color_index < source_colors.size(); ++source_color_index)
    {
        if(source_color_index % PALETTE_COLORS_PER_BANK == 0)
        {
            source_color_indexes[source_color_index] = transparent_color_index;
            continue;
        }

        const bn::color source_color = source_colors[source_color_index];
        int transition_color_index = -1;

        for(int index = 1; index < transition_palette_colors_count; ++index)
        {
            if(transition_palette_colors[index] == source_color)
            {
                transition_color_index = index;
                break;
            }
        }

        if(transition_color_index < 0)
        {
            if(transition_palette_colors_count >= TRANSITION_MAX_PALETTE_COLORS)
            {
                return false;
            }

            transition_color_index = transition_palette_colors_count;
            transition_palette_colors[transition_color_index] = source_color;
            ++transition_palette_colors_count;
        }

        source_color_indexes[source_color_index] = transition_color_index;
    }

    return true;
}

bool end_transition_manager::_transition_tile_index(
        const bn::regular_bg_item& source_item, const transition_color_indexes& source_color_indexes,
        int source_x, int source_y,
        bn::vector<transition_tile_variant, TRANSITION_MAX_SOURCE_TILE_VARIANTS>& variants,
        int& output_tile_index)
{
    const bn::regular_bg_map_item& source_map = source_item.map_item();
    const bn::size source_dimensions = source_map.dimensions();

    if(source_x < 0 || source_x >= source_dimensions.width() ||
       source_y < 0 || source_y >= source_dimensions.height())
    {
        output_tile_index = 0;
        return true;
    }

    bn::regular_bg_map_cell_info source_cell(source_map.cell(source_x, source_y));
    const int source_palette_index = source_cell.palette_id();
    const int source_palette_color_index = source_palette_index * PALETTE_COLORS_PER_BANK;

    if(source_palette_color_index < 0 ||
       source_palette_color_index + PALETTE_COLORS_PER_BANK > source_color_indexes.size() ||
       source_palette_color_index + PALETTE_COLORS_PER_BANK > source_item.palette_item().colors_ref().size())
    {
        return false;
    }

    const int source_tile_index = source_cell.tile_index();

    if(source_tile_index < 0 || source_tile_index >= source_item.tiles_item().tiles_ref().size())
    {
        return false;
    }

    const transition_tile_variant variant = {
        source_tile_index,
        source_palette_index,
        source_cell.horizontal_flip(),
        source_cell.vertical_flip()
    };
    int variant_index = _find_transition_tile_variant(variants, variant);

    if(variant_index < 0)
    {
        if(variants.full())
        {
            return false;
        }

        variant_index = variants.size();
        variants.push_back(variant);
    }

    output_tile_index = variant_index + 1;
    return true;
}

bool end_transition_manager::_write_transition_tiles(
        const bn::regular_bg_item& source_item, const transition_color_indexes& source_color_indexes,
        const bn::vector<transition_tile_variant, TRANSITION_MAX_SOURCE_TILE_VARIANTS>& variants,
        int background_color_index, bn::span<bn::tile>& destination_tiles)
{
    for(int tile_index = 0; tile_index < destination_tiles.size(); ++tile_index)
    {
        for(int row = 0; row < TRANSITION_TILE_SIZE; ++row)
        {
            destination_tiles[tile_index].data[row] = 0;
        }
    }

    if(background_color_index)
    {
        const std::uint16_t background_color_word = std::uint16_t(
                background_color_index | (background_color_index << 8));
        auto* background_tile_words = reinterpret_cast<std::uint16_t*>(destination_tiles.data());
        constexpr int WORDS_PER_BPP8_TILE = (sizeof(bn::tile) * 2) / sizeof(std::uint16_t);

        for(int word_index = 0; word_index < WORDS_PER_BPP8_TILE; ++word_index)
        {
            background_tile_words[word_index] = background_color_word;
        }
    }

    const bn::span<const bn::tile>& source_tiles = source_item.tiles_item().tiles_ref();

    for(int variant_index = 0; variant_index < variants.size(); ++variant_index)
    {
        const transition_tile_variant& variant = variants[variant_index];
        const bn::tile& source_tile = source_tiles[variant.source_tile_index];
        auto* destination_words = reinterpret_cast<std::uint16_t*>(
                &destination_tiles[(variant_index + 1) * 2]);

        for(int destination_y = 0; destination_y < TRANSITION_TILE_SIZE; ++destination_y)
        {
            const int source_y = variant.vertical_flip ?
                    (TRANSITION_TILE_SIZE - 1) - destination_y : destination_y;
            const std::uint32_t source_row = source_tile.data[source_y];

            for(int destination_x = 0; destination_x < TRANSITION_TILE_SIZE; destination_x += 2)
            {
                std::uint16_t destination_word = 0;

                for(int pixel_index = 0; pixel_index < 2; ++pixel_index)
                {
                    const int current_destination_x = destination_x + pixel_index;
                    const int source_x = variant.horizontal_flip ?
                            (TRANSITION_TILE_SIZE - 1) - current_destination_x : current_destination_x;
                    const int source_color = int((source_row >> (source_x * 4)) & 0x0F);
                    const int source_color_index =
                            (variant.source_palette_index * PALETTE_COLORS_PER_BANK) + source_color;
                    const int destination_color_index = source_color_indexes[source_color_index];

                    if(destination_color_index < 0)
                    {
                        return false;
                    }

                    destination_word |= std::uint16_t(destination_color_index << (pixel_index * 8));
                }

                destination_words[(destination_y * (TRANSITION_TILE_SIZE / 2)) + (destination_x / 2)] =
                        destination_word;
            }
        }
    }

    return true;
}

bool end_transition_manager::_create_transition_layer(
        const bn::regular_bg_item& source_item, const transition_color_indexes& source_color_indexes,
        const bn::bg_palette_ptr& transition_palette, const bn::fixed_point& captured_position,
        int background_color_index, int priority, bn::optional<bn::affine_bg_ptr>& destination_bg)
{
    const bn::size source_dimensions = source_item.map_item().dimensions();
    const int source_center_x = (source_dimensions.width() * TRANSITION_TILE_SIZE / 2) -
            captured_position.x().shift_integer();
    const int source_center_y = (source_dimensions.height() * TRANSITION_TILE_SIZE / 2) -
            captured_position.y().shift_integer();
    const int source_origin_tile_x = _floor_division(
            source_center_x - TRANSITION_HALF_SIZE, TRANSITION_TILE_SIZE);
    const int source_origin_tile_y = _floor_division(
            source_center_y - TRANSITION_HALF_SIZE, TRANSITION_TILE_SIZE);
    const bn::fixed_point transition_pivot(
            source_center_x - ((source_origin_tile_x * TRANSITION_TILE_SIZE) + TRANSITION_HALF_SIZE),
            source_center_y - ((source_origin_tile_y * TRANSITION_TILE_SIZE) + TRANSITION_HALF_SIZE));
    bn::vector<transition_tile_variant, TRANSITION_MAX_SOURCE_TILE_VARIANTS> variants;

    for(int y = 0; y < TRANSITION_MAP_DIMENSION; ++y)
    {
        for(int x = 0; x < TRANSITION_MAP_DIMENSION; ++x)
        {
            int output_tile_index;

            if(! _transition_tile_index(
                       source_item, source_color_indexes,
                       source_origin_tile_x + x, source_origin_tile_y + y,
                       variants, output_tile_index))
            {
                return false;
            }
        }
    }

    const int transition_tiles_count = (variants.size() + 1) * 2;
    bn::optional<bn::affine_bg_tiles_ptr> transition_tiles =
            bn::affine_bg_tiles_ptr::allocate_optional(transition_tiles_count);

    if(! transition_tiles)
    {
        return false;
    }

    bn::optional<bn::span<bn::tile>> transition_tiles_vram = transition_tiles->vram();

    if(! transition_tiles_vram ||
       ! _write_transition_tiles(
               source_item, source_color_indexes, variants, background_color_index, *transition_tiles_vram))
    {
        return false;
    }

    bn::optional<bn::affine_bg_map_ptr> transition_map = bn::affine_bg_map_ptr::allocate_optional(
            bn::size(TRANSITION_MAP_DIMENSION, TRANSITION_MAP_DIMENSION),
            bn::move(*transition_tiles), bn::bg_palette_ptr(transition_palette));

    if(! transition_map)
    {
        return false;
    }

    const int tiles_offset = transition_map->tiles_offset();

    if(tiles_offset + variants.size() + 1 > TRANSITION_MAX_PALETTE_COLORS)
    {
        return false;
    }

    bn::optional<bn::span<bn::affine_bg_map_cell>> transition_map_vram = transition_map->vram();

    if(! transition_map_vram)
    {
        return false;
    }

    auto* transition_map_words = reinterpret_cast<std::uint16_t*>(transition_map_vram->data());

    for(int y = 0; y < TRANSITION_MAP_DIMENSION; ++y)
    {
        for(int x = 0; x < TRANSITION_MAP_DIMENSION; x += 2)
        {
            int first_output_tile_index;
            int second_output_tile_index;

            if(! _transition_tile_index(
                       source_item, source_color_indexes,
                       source_origin_tile_x + x, source_origin_tile_y + y,
                       variants, first_output_tile_index) ||
               ! _transition_tile_index(
                       source_item, source_color_indexes,
                       source_origin_tile_x + x + 1, source_origin_tile_y + y,
                       variants, second_output_tile_index))
            {
                return false;
            }

            transition_map_words[((y * TRANSITION_MAP_DIMENSION) + x) / 2] = std::uint16_t(
                    (tiles_offset + first_output_tile_index) |
                    ((tiles_offset + second_output_tile_index) << 8));
        }
    }

    bn::optional<bn::affine_bg_ptr> transition_bg =
            bn::affine_bg_ptr::create_optional(bn::move(*transition_map));

    if(! transition_bg)
    {
        return false;
    }

    transition_bg->set_position(0, 0);
    transition_bg->set_pivot_position(transition_pivot);
    transition_bg->set_scale(1);
    transition_bg->set_priority(priority);
    transition_bg->set_wrapping_enabled(false);
    destination_bg = bn::move(*transition_bg);
    return true;
}

end_transition_manager::end_transition_manager(
        const bn::regular_bg_item& bg_item, const bn::regular_bg_item& walls_item)
    : _bg_item(&bg_item),
      _walls_item(&walls_item),
      _started(false)
{
}

end_transition_manager::~end_transition_manager()
{
    _bg.reset();
    _walls_bg.reset();

    if(_started)
    {
        bn::rect_window::internal().restore();
        bn::window::outside().restore();
    }
}

void end_transition_manager::start(const bn::fixed_point& captured_position, bn::color backdrop_color)
{
    if(_started)
    {
        return;
    }

    _captured_position = captured_position;
    _backdrop_color = backdrop_color;
    _started = true;
}

bool end_transition_manager::update(const bn::fixed_point& snapshot_position, bn::fixed scale)
{
    if(! _started)
    {
        return false;
    }

    if(! _bg || ! _walls_bg)
    {
        if(! _create_backgrounds())
        {
            return false;
        }
    }

    _bg->set_position(snapshot_position);
    _bg->set_scale(scale);
    _walls_bg->set_position(snapshot_position);
    _walls_bg->set_scale(scale);
    _update_window(snapshot_position, scale);
    return true;
}

bool end_transition_manager::started() const
{
    return _started;
}

bool end_transition_manager::_create_backgrounds()
{
    transition_color_indexes bg_color_indexes;
    transition_color_indexes walls_color_indexes;
    bn::array<bn::color, TRANSITION_MAX_PALETTE_COLORS> transition_palette_colors;

    for(int index = 0; index < TRANSITION_MAX_PALETTE_COLORS; ++index)
    {
        bg_color_indexes[index] = -1;
        walls_color_indexes[index] = -1;
        transition_palette_colors[index] = bn::color(0, 0, 0);
    }

    int transition_palette_colors_count = 1;
    const int transition_backdrop_color_index = _add_transition_palette_color(
            _backdrop_color, transition_palette_colors, transition_palette_colors_count);

    if(transition_backdrop_color_index < 0 ||
       ! _add_palette_colors(
               *_bg_item, bg_color_indexes, transition_palette_colors, transition_palette_colors_count,
               transition_backdrop_color_index) ||
       ! _add_palette_colors(
               *_walls_item, walls_color_indexes, transition_palette_colors, transition_palette_colors_count, 0))
    {
        BN_LOG("Unable to create end transition palette");
        return false;
    }

    const int palette_colors_count =
            ((transition_palette_colors_count + PALETTE_COLORS_PER_BANK - 1) / PALETTE_COLORS_PER_BANK) *
            PALETTE_COLORS_PER_BANK;
    const bn::bg_palette_item transition_palette_item(
            bn::span<const bn::color>(transition_palette_colors.data(), palette_colors_count),
            bn::bpp_mode::BPP_8);
    bn::optional<bn::bg_palette_ptr> transition_palette =
            bn::bg_palette_ptr::create_optional(transition_palette_item);

    if(! transition_palette ||
       ! _create_transition_layer(
               *_bg_item, bg_color_indexes, *transition_palette, _captured_position,
               transition_backdrop_color_index, 1, _bg) ||
       ! _create_transition_layer(
               *_walls_item, walls_color_indexes, *transition_palette, _captured_position, 0, 0, _walls_bg))
    {
        _bg.reset();
        _walls_bg.reset();
        return false;
    }

    _configure_window();
    return true;
}

void end_transition_manager::_configure_window()
{
    bn::rect_window internal_window = bn::rect_window::internal();
    internal_window.set_visible(true);
    internal_window.set_show_bg(*_bg, true);
    internal_window.set_show_bg(*_walls_bg, true);
    internal_window.set_show_sprites(true);
    bn::window::outside().set_show_bg(*_bg, false);
    bn::window::outside().set_show_bg(*_walls_bg, false);
    bn::window::outside().set_show_sprites(false);
}

void end_transition_manager::_update_window(const bn::fixed_point& snapshot_position, bn::fixed scale)
{
    const bn::fixed half_width = (bn::display::width() / 2) * scale;
    const bn::fixed half_height = (bn::display::height() / 2) * scale;
    const bn::fixed snapshot_left = snapshot_position.x() - half_width;
    const bn::fixed snapshot_top = snapshot_position.y() - half_height;
    const bn::fixed snapshot_right = snapshot_position.x() + half_width;
    const bn::fixed snapshot_bottom = snapshot_position.y() + half_height;
    const bn::fixed screen_left = -bn::display::width() / 2;
    const bn::fixed screen_top = -bn::display::height() / 2;
    const bn::fixed screen_right = bn::display::width() / 2;
    const bn::fixed screen_bottom = bn::display::height() / 2;
    const bn::fixed visible_left = snapshot_left > screen_left ? snapshot_left : screen_left;
    const bn::fixed visible_top = snapshot_top > screen_top ? snapshot_top : screen_top;
    const bn::fixed visible_right = snapshot_right < screen_right ? snapshot_right : screen_right;
    const bn::fixed visible_bottom = snapshot_bottom < screen_bottom ? snapshot_bottom : screen_bottom;
    bn::rect_window internal_window = bn::rect_window::internal();

    if(visible_left < visible_right && visible_top < visible_bottom)
    {
        internal_window.set_boundaries(visible_top, visible_left, visible_bottom, visible_right);
        internal_window.set_visible(true);
    }
    else
    {
        internal_window.set_visible(false);
    }
}