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

Movement needs engine pathfinding, not a greedy adjacent-tile walk. The exposed
`unit:GeneratePath` binding is explicitly NYI in this release and raises a Lua
error. Normal destination missions use the game’s pathfinder; use those and
verify arrival or a blocker. Select a living opposing team already at war for
combat scenarios, rather than the first other player. Record actual damage;
a declaration or an attack request alone does not establish combat coverage.
