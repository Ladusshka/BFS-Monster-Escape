# BFS-Monster-Escape


This program implements a function template Path find_escape_route(const Map& map, const Beast& beast) that computes the shortest path for a hero (Daník) to reach an exit without being devoured by a monster (the beast). The path is a sequence of positions from map.hero to map.exit—or empty if no such path exists.

The code uses a Map structure, which provides:

The hero's starting position
The beast's starting position
The exit position
An operator[](Position p) method to determine whether a position is a wall (Tile::WALL), an empty cell (Tile::EMPTY), or a trap (Tile::TRAP)
The hero cannot step onto walls or traps.

The Beast structure defines how the monster moves in response to the hero’s actions. It is copyable and provides a move(...) method that returns the beast's new position. The beast might move through walls or cover large distances in a single step, but it never leaves the map. The goal of the function is to avoid any scenario where the beast catches the hero, including at the exit tile.
