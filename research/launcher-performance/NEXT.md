The next focused launcher improvement is reusing a successful inspection when
the installation has not changed. The largest observed game-startup target is
graphics initialization, which merits an asset-packaging experiment.

Investigated September 12, 2026 in the perf worktree. This pass added a read-only
probe and evidence; it did not change production launcher code, settings, or the
installed game, and did not launch a game. Existing unrelated perf work was left
untouched. The previously committed startup changes remain at `2742e9d2`.

| Next candidate | Evidence | What the evidence establishes |
| --- | --- | --- |
| Reuse successful validation with change tracking | Full signature verification: 1.886 s median; bundle metadata walk: 0.093 s for 16,743 entries | A substantial repeat-launch opportunity, not an implemented cached validator or a measured end-to-end saving |
| Package graphics assets into supported archives | 3,327 loose art files, 400,538,449 bytes; earlier startup log: units 6.782 s, landmarks 4.516 s, cities 1.151 s | Graphics initialization is a much larger target than the remaining small launcher operations; archive speedup is unproven |
| Optional direct launch past Aspyr's Play window | Bundle executable is AppBundleExe; GameGuide identifies Contents/MacOS/Civilization V as the game | An extra process/UI handoff can potentially be removed; Steam integration must be tested |
| Investigate duplicate database prefetch and map-list work | Earlier log: PrefetchGameData 0.406 + 0.377 s; map parsing 0.512 s | Smaller startup budgets; the second prefetch cannot be assumed redundant without tracing the intervening changes |
| Trim installation-only source assets | Installed Lua/tmp has 123 files totaling 4,433,286 bytes | Small packaging and verification improvement; not a likely multi-second win |

The successful signature timings cover three repeated reads, not cold-start
measurements. The metadata walk uses lstat-style information and does not verify
file contents. The earlier complete graphics/database logs were last written at
14:04 UTC (09:04 CDT). They identify useful targets but do not attribute time to
the current perf branch or establish a whole-game improvement. Another task
changed the installed app and began a new game session during this audit. The
first signature samples failed and were excluded from the successful median;
the final three passed. Later log captures were partial as the new session wrote
them, so the table uses the earlier complete session.

**Reuse the opening inspection for an unchanged Play action.** Start with a
session cache: run the existing full inspection when opening the launcher, then
reuse that result only after confirming no relevant files or settings changed.
Keep process discovery and Steam readiness fresh at the handoff. A persistent
backend or native filesystem observer can track the game bundle and checkout
between actions. Any repair, replacement, changed preference, uncertain observer
state or lost events must require a fresh inspection. Cache successful results
only. Extending the cache across launcher restarts requires a separate durable
invalidation design.

The 0.093 s metadata sweep makes this worth prototyping, but it is not equivalent
to a fresh cryptographic check and does not cover the source checkout in that
number. Test in-place edits, preserved modification times, app replacement,
deletions, links, changed crossplay settings, and observer overflow before
adopting it. A successful design could remove much of the roughly 1.9 s repeat
Play check; no specific saving has yet been measured.

The diagnostic probe also tried codesign's `--ignore-resources`, which took
0.007 s. This deliberately skips resource validation and is not a proposed
drop-in optimization. In the first samples it passed while full verification
failed. Apple's code signatures protect resources as well as executables;
continue using the supported verifier or Code Signing Services API.
[Apple's explanation](https://developer.apple.com/documentation/technotes/tn3126-inside-code-signing-hashes).

**Test archive packaging for graphics.** About 371 MB of the art tree consists
of DDS textures, with 345 GR2 models and many XML resource descriptors. A
lossless archive experiment could reduce repeated file opens while retaining
the actual asset bytes and graphics quality. Civ V has an FPK format, and a Mac
tool author documents creating and extracting it; this is format feasibility,
not evidence of a modern Mac Lekmod speedup.
[Macapaka author's Civ V format announcement](https://forums.civfanatics.com/threads/macapaka-now-supports-civ5-fpk-files.400208/).

There are 119 repeated basename groups: 107 have identical contents, but 12 have
different contents, including unit icon atlases and art-definition XML. Packing
or deduplicating by filename could change which resource wins. Establish the
current virtual-filesystem resolution and preserve it before testing an isolated
packed installation. Compare menu startup, load an existing save, and inspect
units, improvements, cities and icons. The approximately 12.4 s graphics budget
includes engine work that archive packing might not accelerate.

**Offer an optional direct-game launch.** The adjacent civ5-cli implementation
already spawns the game binary with SteamAppId and SteamGameId set to 8930 and
the executable directory as its working directory. That provides a starting
point for a launcher option. Validate Steam startup, DLC, overlay, invitations,
crossplay and failure recovery through the normal installed app before replacing
the existing Steam URL handoff. No supported skip-launcher setting was verified
for this Aspyr build, and no handoff saving was timed here.

**Keep smaller opportunities in proportion.** CvDllDatabaseUtility already has
a cache-valid flag. Trace why it is invalidated between the two prefetch passes;
do not remove a pass that follows real database changes. For map listing, explore
delaying heavy generation helpers until generation is requested, while retaining
the information required by GetMapScriptInfo and GetMapInitData. The older Lua
log also reports a FeatureGenerator error in LekmapTeamerMapLegacy.lua, which is
a separate correctness issue rather than proof of a performance improvement.
Removing Lua/tmp after packaging requires confirming all runtime and later UI
switching paths use the checkout as their source. Logos were previously only
about 16 ms; optimizing them is a much smaller priority.

Evidence: [successful signature and metadata samples](next-evidence.json),
[earlier complete startup logs and excluded failed signature samples](concurrent-change-evidence.json),
and [follow-up successful signature diagnostic](current-signature-diagnostic.txt).
Run the probe from perf with
`python3 research/launcher-performance/inspect_next.py`; process/signature access
must work outside the agent sandbox for meaningful results.
