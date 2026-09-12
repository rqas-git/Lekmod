# Shared implementation and regression checks

## Map scripts

All 12 map entry points use `Lekmap/HBMapOptions.lua` for menu options 1–14. Keep
these indices stable: generation code reads them by number. Each map supplies its
own overrides and extra options. Every call creates fresh option tables; numeric
size choices remain strings.

Rectangular maps share the common feature and map generators while retaining
their foliage, coast, and lake defaults. Teamer and Four Corners share river
generation, and Oval and Donut share regional start placement. Preserve include
order, iteration order, and random-call order when changing these helpers.

The map regression suite compares complete metadata for all 12 maps and 756
generator scenarios against commit `1bc2ff42`, including RNG traces. These use
engine facades, so full seeded map generation still needs game-level validation.
The optimized shuffle in `HBMapmakerUtilities.lua` retains its separate fixtures
in `tests/performance`.

Run the map tests with Python 3.9+ and Lua 5.1:

```sh
LUA51=/path/to/lua5.1 python3 -B -m unittest discover -s tests -p test_maps.py -v
```

Tests read their original fixtures from Git history. Use a full checkout in CI.
