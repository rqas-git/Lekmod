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

## Database loaders

`GetOrPrepareResults` resets an existing cached statement or prepares one on a
miss. Keep cache keys, SQL, and bind order intact. Dynamic SQL is still built only
on a miss. `SetYieldMatrix` shares 20 single-matrix assignment loaders; accumulation
and multi-matrix loaders remain explicit. Alternative allocation configurations
retain their original row-count bookkeeping.

`MaxRows` continues to cache statements rather than results. The SQLite fixture
executes the production helper and sizing methods against sparse IDs, duplicate
rows, changed bindings, changed table contents, schema recreation, failed query
preparation, and cache clearing. Run it with a C++98-capable compiler and SQLite
development files:

```sh
python3 -B -m unittest discover -s tests -p test_yield_matrix.py -v
```

## Installer lifecycle

The Windows installer captures choices and confirmations on the UI thread before
starting work. Lekmod and Lekmap share temporary download/extraction ownership and
schedule completion dialogs and controls through `root.after`. The Lekmod worker
calls the transactional `UIManager.install_mod` directly: it must never delete old
installations before staging the replacement. Self-update validation and rollback
remain in `installer_updater.py`.

Run both controller integration and filesystem rollback checks:

```sh
python3 -B -m unittest discover -s tests -p test_installer_flow.py -v
python3 -B -m unittest discover -s LekmodInstaller/tests -v
```
