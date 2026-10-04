# Civilization V automation research

The fastest implementation path is to extend the existing isolated runner and
network Lua hooks, rather than build a mouse-driven player. No generalized
automation framework was implemented in this investigation.

Use a scenario manifest containing release/source hashes, seed, map options,
slots, rules, expected actions, turn/time limits and assertions. Start with a
small set of compatible checkpoint saves. Loading a checkpoint avoids repeating
Lekmap generation and places policies, religion, wars and capture cases near the
action being tested. Keep fresh-map initialization as a separate required test.
Checkpoints must identify their release, map and participants; arbitrary old saves
cannot substitute for a current-release test.

Run cheap source/component comparisons first, targeted native gameplay cases
second, two-peer crossplay third, and full-game AI soaks last. The current native
runner owns cleanup, profiles, telemetry and independent save verification. It
needs scenario setup and event assertions, not a new launcher. The multiplayer
hooks already issue normal network orders and record live entity rosters, but
host/join, reconnect, blocker handling and a settled checkpoint need orchestration.
Three seconds of waiting is not a synchronization barrier. Require both peers
to acknowledge the same turn, no pending processing, repeated unchanged live
entity snapshots, and a matching state comparison before declaring success.

A useful initial matrix covers map sizes/wraps and seeds; AIs/city-states; city
founding and improvements; policy and religion transitions; melee/ranged combat;
capture/recapture with both rule settings; save/reload; reconnect; and victories.
Choose pairwise combinations for broad coverage, plus explicit regression cases
for every changed rule. Random full-game play is supplementary: it rarely reaches
all conditions and must report unobserved actions as uncovered.

A practical first scenario set:

| Scenario | Required observations | Fast setup |
| --- | --- | --- |
| Fresh generation | Matching RNG/starting state and living participants | Small fixed seed, then separate representative Pangaea seeds |
| Expansion/economy | Founding, workers, research, policies, trade | Checkpoint just before completions |
| War/combat | Human/AI contact, melee/ranged damage, promotions, peace | Opposing units and cities already within reach |
| Capture/recapture | Ownership, population, buildings and both rule values | Two fixtures before the capture action |
| Religion/yields | Tithe, modified civilization/natural-wonder and adjacency yields | Relevant belief, civilization and tiles already present |
| City-states/upgrades | Rewards at relevant eras, crossbow defense, embarkation | Fixtures on either side of the affected transition |
| Persistence/network | Save/reload, disconnect/reconnect, settled state | Reuse the action checkpoints with both peers |
| Full-game soak | AI interaction, elimination and actual victory outcome | Quick speed, explicit outcome and timeout predicates |

These are a proposed implementation sequence, not scenarios all validated by
this task. Keep assertions for changed mechanics explicit even when choosing
pairwise setup combinations for the broader matrix.

Retain per-peer RNG traces, network/resync logs, current entity rosters, saves,
crash reports, map/source/core hashes and a replay manifest. Keep UI rendering
checks separate from background simulations. Run one native Steam client at a
time on this host and use distinct Steam identities for network peers. Prefer
small hashed guest deltas over repeatedly transferring a whole Windows install.

The SDK is available through Steam Library Tools according to
[Civilization Support](https://support.civilization.com/hc/en-us/articles/37707596624531-Civilization-V-Where-is-the-SDK).
It can assist scenario construction and debugging, but the already demonstrated
Lua runner is the quickest starting point here; this is an engineering inference
from this checkout, not a vendor claim of supported headless testing.
[UTM documents scripting and guest execution](https://docs.getutm.app/scripting/scripting/);
[current CLI source](https://github.com/utmapp/UTM/blob/main/utmctl/UTMCtl.swift)
waits for execution results, while the installed 4.7.5 workflow still needs fresh
completion envelopes. Do not assume current source behavior matches installed tools.

Implement in stages: scenario manifest/event assertions; checkpoint fixtures;
network supervisor and settled comparisons; then full-game/victory runs. Fail
a network run immediately on unexplained initialization divergence rather than
spending a full-game budget on a desynchronized start. Use normalized gameplay
state comparisons; raw save bytes include platform build headers and are not
a portable state checksum. Exact
delivery time and full-game throughput remain unmeasured. The observed map
startup cost makes checkpoint reuse the first practical speed improvement.

Lekmap v6.3 currently chooses a 300-attempt cap. Each rejected attempt regenerates
terrain, features, rivers and starts. Lowering the cap or changing acceptance
criteria changes map quality and RNG consumption, so it is not a transparent
optimization. The repaired connected-region routine keeps numeric component
numbering and final table construction, avoids unused counts and scans only
visited cells. Seventy-two Lua 5.1 fixture cases produced identical outputs;
the combined routine timings were 1.84 seconds before and 0.087 seconds after.
That is a routine benchmark, not a 21x whole-game startup improvement.

This validation's eight-major-civilization, 56x50 Quick single-player game took
roughly 250 seconds to initialize and about 16 minutes to complete turns 0–30.
It is one observed scenario, not a throughput guarantee or a before/after speed
comparison. Native quick-play supplied eight majors despite a small requested
map: a future manifest must verify actual slots instead of trusting preset labels.

Guest supervision also matters for speed and reliability. Two diagnostic
PowerShell jobs exhausted Windows commit space; stopping the owned jobs restored
memory and lobby creation. Bound and identify guest jobs, not only the game.
A complete fork map-folder transfer fixed a demonstrated missing shared helper;
checking only the selected Pangaea file would have missed it.

A later explicit four-major, two-team Teamer scenario completed 30 turns in
roughly a minute of owned-process runtime, including startup. Its 36x26 map is
not equivalent to the eight-major Pangaea case; it demonstrates why fixture
choice and verified participant counts matter more than a single throughput
number. Keep Pangaea generation coverage separately.

For full games, completion must be a victory/elimination predicate or a bounded
turn limit, with the actual outcome recorded. A fixed turn-only acceptance rule
can reject a valid game that ended earlier. Use Quick speed for throughput,
explicitly test other speeds when their scaling changes matter, and retain
eliminated-player records instead of treating disappearance as missing telemetry.
Parallelize component checks here; scale game runs through separately provisioned
workers with independent Steam identities rather than concurrent native copies
sharing this host's Steam client.

Profile the harness as well as the game. The expanded hook currently scans every
plot for every major and writes individual SQLite fields once per second, even
while messages are processing. A future runner should use turn/event snapshots,
compact grouped records and full settled comparisons at checkpoints. This could
reduce test overhead; no speedup is claimed without a controlled measurement.
Read-only telemetry can also block execution: an R18 Windows dump and Lua call
metadata located an infinite engine wait in the hook's
`Network.HasSentNetTurnComplete()` query during message processing. The existing
hooks now omit that query and use subsequent turns and network-log acknowledgements.
The underlying engine wait is not explained by locating the blocked call.

The corrected-hook R19 continuation retained another compatibility finding:
3,835 synchronized RNG calls matched before city-production selection at turn 44
used the same seed with different ranges (Mac 2491, Windows 3324). Matching
loaded difficulty and speed did not explain it. Log headers and turn progression
excluded a retained earlier-run prefix. An automation supervisor should retain
the first divergent call and preceding state, not accept eventual turn completion
or patch simulation ordering without a demonstrated cause.

Movement needs engine pathfinding, not a greedy adjacent-tile walk. The exposed
`unit:GeneratePath` binding is explicitly NYI in this release and raises a Lua
error. Normal destination missions use the game’s pathfinder; use those and
verify arrival or a blocker. Select a living opposing team already at war for
combat scenarios, rather than the first other player. Record actual damage;
a declaration or an attack request alone does not establish combat coverage.

Verify rules through named runtime queries. GameInfo.GameOptions database IDs
are not the engine’s GameOptionTypes enum in this release; using them with
Game.IsOption can falsely report an enabled custom rule as disabled. Prefer
Game.IsOption("GAMEOPTION_...") and retain the actual loaded values. Quick
combat/movement must also be verified, not inferred from profile preferences.

The latest team run completed turns 0–30 with matching recorded live state,
including supplemental production counters, but retained an AI unit sync warning.
Its combat coverage remained absent despite expansion and ordinary movement.
This supports dedicated saved scenarios with opposing units already near the
trigger, rather than expecting a general early-game bot to exercise every rule.
End-turn requests also need acknowledgement: CanDoControl can succeed while
DoControl waits for AI/unit processing. A future supervisor should report that
state and bound recovery through normal game controls, not bypass engine gates.


For sustained throughput, benchmark a Windows peer on x86 hardware as well as
this Windows 11 ARM VM. [Microsoft documents that x86 applications run through
emulation on Windows ARM](https://learn.microsoft.com/en-us/windows/arm/apps-on-arm-x86-emulation).
The current Civ V Windows executable is x86; an x86 worker is therefore a useful
engineering candidate for avoiding that translation layer, not a measured speedup.
Keep the Mac client and official DLL in the acceptance pair. Increasing VM CPU
count alone does not demonstrate improved game or harness throughput. [UTM also
distinguishes guest CPU architecture from hardware virtualization](https://docs.getutm.app/settings-qemu/system/).


Separate cold application/database initialization, map-generation time, turns,
and telemetry/transfer overhead in measurements. The current launcher does not
clear the game's cache on every launch; a new test profile is intentionally cold.
Checkpoint loading avoids fresh map generation, while application startup still
has a cost. Reusing an already running client could reduce that cost, but needs
proven scenario-state reset and fresh acknowledgements before it is trustworthy.
