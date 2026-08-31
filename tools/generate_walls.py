"""Generate a wall background from a one-pixel-per-tile level layout."""

from __future__ import annotations

import argparse
from pathlib import Path

from PIL import Image


BACKGROUND_COLOR = (0, 255, 0)
BLACK = (0, 0, 0)
WALL_TILE_SIZE = 8
HORIZONTAL_WALL_TILE_INDEX = 0
VERTICAL_WALL_TILE_INDEX = 1
CONNECTION_WALL_TILE_INDEX = 2
WALL_TILE_COUNT = 3


def default_output_path(layout_path: Path) -> Path:
    """Return the wall background path derived from a small level-layout path."""
    stem = layout_path.stem

    if stem.endswith("_small"):
        stem = stem.removesuffix("_small")

    return layout_path.with_name(f"{stem}_walls.bmp")


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
    return parser.parse_args()


def main() -> int:
    arguments = parse_arguments()
    output_path = arguments.output or default_output_path(arguments.layout)

    try:
        generate_wall_background(arguments.layout, arguments.tiles, output_path)
    except (OSError, ValueError) as error:
        print(f"Error: {error}")
        return 1

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
