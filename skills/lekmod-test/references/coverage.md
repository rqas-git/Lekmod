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

Keep per-peer turn IDs 0–30 and independently parsed final save headers. Compare
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
