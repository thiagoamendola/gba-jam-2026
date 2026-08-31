"""Generate a wall background from a one-pixel-per-tile level layout."""

from __future__ import annotations

import argparse
from dataclasses import dataclass
from pathlib import Path

from PIL import Image


BACKGROUND_COLOR = (0, 255, 0)
BLACK = (0, 0, 0)
WALL_TILE_SIZE = 8
HORIZONTAL_WALL_TILE_INDEX = 0
VERTICAL_WALL_TILE_INDEX = 1
CONNECTION_WALL_TILE_INDEX = 2
WALL_TILE_COUNT = 3


@dataclass(frozen=True)
class WallDefinition:
    """A wall run expressed as upper-left pixel coordinates in the full-size stage image."""

    start_x: int
    start_y: int
    end_x: int
    end_y: int


def stage_name(layout_path: Path) -> str:
    """Return the stage name derived from a small level-layout path."""
    stem = layout_path.stem

    if stem.endswith("_small"):
        return stem.removesuffix("_small")

    return stem


def default_output_path(layout_path: Path) -> Path:
    """Return the wall background path derived from a small level-layout path."""
    return layout_path.with_name(f"{stage_name(layout_path)}_walls.bmp")


def default_definitions_path(layout_path: Path) -> Path:
    """Return the generated C++ definitions path for a stage layout."""
    project_root = Path(__file__).resolve().parent.parent
    return project_root / "include" / f"{stage_name(layout_path)}_defs.h"


def wall_tiles(tiles_path: Path) -> list[Image.Image]:
    """Load the horizontal, vertical, and connection 8x8 wall frames as RGB images."""
    with Image.open(tiles_path) as tiles_image:
        required_height = WALL_TILE_SIZE * WALL_TILE_COUNT

        if tiles_image.width < WALL_TILE_SIZE or tiles_image.height < required_height:
            raise ValueError(
                f"Wall tile image must be at least {WALL_TILE_SIZE}x{required_height}: {tiles_path}"
            )

        return [
            tiles_image.crop((0, index * WALL_TILE_SIZE, WALL_TILE_SIZE, (index + 1) * WALL_TILE_SIZE)).convert("RGB")
            for index in range(WALL_TILE_COUNT)
        ]


def indexed_tiles(tiles: list[Image.Image]) -> tuple[list[Image.Image], list[int]]:
    """Create indexed wall tiles that share a palette with green at palette index zero."""
    palette_colors = [BACKGROUND_COLOR]
    color_indexes = {BACKGROUND_COLOR: 0}
    tiles_indexes: list[list[int]] = []

    for tile in tiles:
        tile_indexes: list[int] = []
        tile_pixels = tile.load()

        for y in range(WALL_TILE_SIZE):
            for x in range(WALL_TILE_SIZE):
                color = tile_pixels[x, y]

                if color not in color_indexes:
                    color_indexes[color] = len(palette_colors)
                    palette_colors.append(color)

                tile_indexes.append(color_indexes[color])

        tiles_indexes.append(tile_indexes)

    if len(palette_colors) > 256:
        raise ValueError("Wall tiles contain more than 256 colors")

    palette = [component for color in palette_colors for component in color]
    palette.extend([0] * (768 - len(palette)))
    result: list[Image.Image] = []

    for tile_indexes in tiles_indexes:
        tile = Image.new("P", (WALL_TILE_SIZE, WALL_TILE_SIZE))
        tile.putpalette(palette)
        tile.putdata(tile_indexes)
        result.append(tile)

    return result, palette


def _is_black(layout_pixels, width: int, height: int, x: int, y: int) -> bool:
    return 0 <= x < width and 0 <= y < height and layout_pixels[x, y] == BLACK


def wall_tile_index(layout_pixels, width: int, height: int, x: int, y: int) -> int:
    """Return the wall frame appropriate for a black layout cell's cardinal neighbors."""
    has_left = _is_black(layout_pixels, width, height, x - 1, y)
    has_right = _is_black(layout_pixels, width, height, x + 1, y)
    has_top = _is_black(layout_pixels, width, height, x, y - 1)
    has_bottom = _is_black(layout_pixels, width, height, x, y + 1)

    if has_left and has_right and not (has_top or has_bottom):
        return HORIZONTAL_WALL_TILE_INDEX

    if has_top and has_bottom and not (has_left or has_right):
        return VERTICAL_WALL_TILE_INDEX

    return CONNECTION_WALL_TILE_INDEX


def _wall_definition(start_x: int, start_y: int, end_x: int, end_y: int) -> WallDefinition:
    """Convert inclusive small-layout cell coordinates into full-size stage pixel coordinates."""
    return WallDefinition(
        start_x * WALL_TILE_SIZE,
        start_y * WALL_TILE_SIZE,
        end_x * WALL_TILE_SIZE,
        end_y * WALL_TILE_SIZE,
    )


def extract_wall_definitions(layout: Image.Image) -> tuple[list[WallDefinition], list[WallDefinition]]:
    """Extract maximal horizontal and vertical black-cell runs from a level layout."""
    layout_pixels = layout.load()
    horizontal_walls: list[WallDefinition] = []
    vertical_walls: list[WallDefinition] = []

    for y in range(layout.height):
        x = 0

        while x < layout.width:
            if layout_pixels[x, y] != BLACK:
                x += 1
                continue

            start_x = x

            while _is_black(layout_pixels, layout.width, layout.height, x + 1, y):
                x += 1

            if x > start_x:
                horizontal_walls.append(_wall_definition(start_x, y, x, y))

            x += 1

    for x in range(layout.width):
        y = 0

        while y < layout.height:
            if layout_pixels[x, y] != BLACK:
                y += 1
                continue

            start_y = y

            while _is_black(layout_pixels, layout.width, layout.height, x, y + 1):
                y += 1

            if y > start_y:
                vertical_walls.append(_wall_definition(x, start_y, x, y))

            y += 1

    for y in range(layout.height):
        for x in range(layout.width):
            if layout_pixels[x, y] != BLACK:
                continue

            has_neighbor = (
                _is_black(layout_pixels, layout.width, layout.height, x - 1, y) or
                _is_black(layout_pixels, layout.width, layout.height, x + 1, y) or
                _is_black(layout_pixels, layout.width, layout.height, x, y - 1) or
                _is_black(layout_pixels, layout.width, layout.height, x, y + 1)
            )

            if not has_neighbor:
                horizontal_walls.append(_wall_definition(x, y, x, y))

    return horizontal_walls, vertical_walls


def _header_guard(stage: str) -> str:
    return "".join(character.upper() if character.isalnum() else "_" for character in stage) + "_DEFS_H"


def _render_wall_array(name: str, walls: list[WallDefinition]) -> list[str]:
    lines = [f"    constexpr inline std::array<wall_data, {len(walls)}> {name} = {{"]

    for wall in walls:
        lines.append(
            "        wall_data{ "
            f"bn::fixed_point({wall.start_x}, {wall.start_y}), "
            f"bn::fixed_point({wall.end_x}, {wall.end_y}) "
            "},"
        )

    lines.extend(["    };", ""])
    return lines


def write_stage_definitions(
        layout_path: Path, output_path: Path,
        horizontal_walls: list[WallDefinition], vertical_walls: list[WallDefinition]) -> None:
    """Write the generated horizontal and vertical wall runs as a C++ header."""
    stage = stage_name(layout_path)
    namespace = f"{stage}_defs"
    header_lines = [
        "// Generated by tools/generate_walls.py. Do not edit manually.",
        f"// Source layout: {layout_path.as_posix()}",
        "",
        f"#ifndef {_header_guard(stage)}",
        f"#define {_header_guard(stage)}",
        "",
        "#include <array>",
        "",
        "#include \"walls.h\"",
        "",
        f"namespace {namespace}",
        "{",
    ]
    header_lines.extend(_render_wall_array("horizontal_walls", horizontal_walls))
    header_lines.extend(_render_wall_array("vertical_walls", vertical_walls))
    header_lines.extend(["}", "", "#endif", ""])

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(header_lines), encoding="utf-8")


def generate_wall_background(layout_path: Path, tiles_path: Path, output_path: Path) -> int:
    """Generate an 8x-scale indexed BMP using wall frames selected from each black cell's neighbors."""
    if output_path.resolve() in {layout_path.resolve(), tiles_path.resolve()}:
        raise ValueError("Output path must differ from the layout and wall tile paths")

    with Image.open(layout_path) as layout_image:
        layout = layout_image.convert("RGB")

    tiles, palette = indexed_tiles(wall_tiles(tiles_path))
    output = Image.new(
        "P",
        (layout.width * WALL_TILE_SIZE, layout.height * WALL_TILE_SIZE),
        color=0,
    )
    output.putpalette(palette)

    layout_pixels = layout.load()
    wall_count = 0
    tile_counts = [0] * WALL_TILE_COUNT

    for y in range(layout.height):
        for x in range(layout.width):
            if layout_pixels[x, y] == BLACK:
                tile_index = wall_tile_index(layout_pixels, layout.width, layout.height, x, y)
                output.paste(tiles[tile_index], (x * WALL_TILE_SIZE, y * WALL_TILE_SIZE))
                wall_count += 1
                tile_counts[tile_index] += 1

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output.save(output_path, format="BMP")
    print(
        f"Generated {output_path} ({output.width}x{output.height}) "
        f"from {wall_count} black layout cells using {tile_counts[HORIZONTAL_WALL_TILE_INDEX]} horizontal, "
        f"{tile_counts[VERTICAL_WALL_TILE_INDEX]} vertical, and "
        f"{tile_counts[CONNECTION_WALL_TILE_INDEX]} connection tiles from {tiles_path}."
    )
    return wall_count


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate an 8x wall background from black pixels in a small BMP layout."
    )
    parser.add_argument("layout", type=Path, help="Small BMP level layout; black pixels become wall tiles.")
    parser.add_argument(
        "--tiles",
        type=Path,
        required=True,
        help="BMP wall sprite sheet; its first three stacked 8x8 frames are horizontal, vertical, and connection tiles.",
    )
    parser.add_argument(
        "--output",
        type=Path,
        help="Generated BMP path. Defaults to <layout-without-_small>_walls.bmp beside the layout.",
    )
    parser.add_argument(
        "--definitions-output",
        type=Path,
        help="Generated C++ header path. Defaults to include/<layout-without-_small>_defs.h.",
    )
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    output_path = arguments.output or default_output_path(arguments.layout)
    definitions_path = arguments.definitions_output or default_definitions_path(arguments.layout)

    try:
        generate_wall_background(arguments.layout, arguments.tiles, output_path)

        with Image.open(arguments.layout) as layout_image:
            layout = layout_image.convert("RGB")

        horizontal_walls, vertical_walls = extract_wall_definitions(layout)
        write_stage_definitions(arguments.layout, definitions_path, horizontal_walls, vertical_walls)
        print(
            f"Generated {definitions_path} with {len(horizontal_walls)} horizontal and "
            f"{len(vertical_walls)} vertical wall definitions."
        )
    except (OSError, ValueError) as error:
        print(f"Error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
