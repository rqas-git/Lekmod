---
name: civ5
description: Navigate Civilization V, inspect its current game rules, and plan or execute gameplay actions. Use for Civ V menus, saves, cities, units, diplomacy, research and policies; use lekmod-test for instrumented mod validation.
---

# Civilization V

Identify the edition (Steam or App Store), platform, expansion, active mods and
current screen before acting. Rules and menu layouts differ; Lekmod's installed
Civilopedia, Lua and game database take precedence over vanilla advice.

For visual play, use the available computer-use surface and inspect its current
state. Prefer a visible named control or a verified shortcut; batch independent
inspection, but observe the result of dependent actions before continuing.
For background testing, use lekmod-test's isolated runner instead of desktop input.
Do not overwrite an ordinary save or advance a user's live game unless that is
within the requested task.

Read [navigation.md](references/navigation.md) for menu routes, bindings and save
locations. Read [play.md](references/play.md) for common decisions and rules lookup.
When a task needs multiplayer, verify actual peers, expansions, mod/core versions,
map availability and lobby settings; seeing a lobby does not prove a successful join.

After an action, verify its effect: selected unit and coordinates, city production,
research queue, adopted policy, diplomatic state or loaded save turn. Distinguish
orders issued from actions completed and report unresolved blockers concretely.
