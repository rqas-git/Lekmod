# Acceptance coverage

Thirty turns establish a bounded smoke test, not complete game compatibility.
Build a coverage matrix from the upstream diff and the user's requested scenarios.
Record setup, observed action, result and evidence for each row; use untested or
blocked when an action did not occur. Do not infer coverage from elapsed turns.

For crossplay, require two real network humans plus the requested AI slots.
Record each slot's human/AI/minor status and civilization after initialization.
Observe AI research, production, expansion and contact rather than merely listing
participants. Use normal network commands for humans, never AI autoplay.

Common rows: initial and additional city founding; growth; worker improvements;
research and production completion; policy branch unlock and adoption; pantheon;
unit movement and stacked-unit resolution; AI diplomacy; war declaration; actual
ranged/melee combat and damage; peace; city-state contact; save and reload;
reconnection. Expansion, contact and combat may need a deliberately arranged map
or a separate targeted game rather than hoping they occur in 30 turns.

For v35.4, additionally check No Destructive Recapture enabled and disabled,
city capture/recapture, Tithe scaling, adjacent-improvement yields, Buganda and
Lake Victoria yields, early/later city-state gold and crossbow ranged defense.
Exact source equivalence and focused behavioral tests complement runtime evidence;
they do not prove the gameplay event occurred in the network session.

Keep every per-peer turn ID in the requested interval and independently parsed
final save headers. A 30-turn checkpoint continuation is, for example, 30–60;
record its initial turn and verify the loaded participants before proceeding. Compare
settled snapshots of every civilization, not snapshots captured at different
points during simultaneous turns. Include source/core hashes and actual Windows
version. Report runtime findings separately from synchronization success.

Use unique run directories, ledger names, save names and fresh guest envelopes.
Configure human slots as OPEN before HostGame claims its slot; pre-setting a slot
TAKEN produced a phantom third human in one four-player test and blocked drafting.
Inspect actual slot claims and readiness before blaming the game or network.

The basic multiplayer player is a smoke-test bot: it does not produce
additional settlers, explore toward enemies, declare war or attack. Adapt its
normal network orders for those scenarios and verify results. Three seconds
without issuing orders is not a synchronization barrier; final acceptance needs
repeated unchanged per-peer snapshots or an acknowledged settled checkpoint.

For programmatic custom-map hosting, select the canonical map filename found
in MapScriptOptions, initialize every custom option from the ordered possible
values and DefaultValue, and broadcast the settings. A v6.3 lobby without those
defaults produced nil-option map errors and an empty game whose turns advanced
automatically. Require living civilizations and the expected map dimensions
before accepting turn progress.

## Planning broader validation

Choose explicit fixtures for combat/capture, religion/beliefs, city-state rewards,
embarkation and unit upgrades, trade routes, elimination and victory conditions.
Check that prerequisites actually exist and that each action completed. Include
both values of changed game rules, relevant map wraps/sizes, and save/reload or
reconnect across those states. Pairwise setup combinations help control cost;
changed mechanics and prior crashes still need individual regression fixtures.
A random full game cannot replace those assertions.

The October 4 numeric adapter removed the demonstrated initial Pangaea mismatch:
64 bounded random calls and four fractal samples matched, without an initial
resync. The 0–30 run still logged one late AI building-class-making warning,
despite repeated matching final live-entity snapshots. Retain that finding and
inspect production counters/queues at a settled checkpoint; matching aggregate
scores or raw save hashes are insufficient. Raw saves also contain platform build
headers, so compare normalized state rather than requiring identical save bytes.
