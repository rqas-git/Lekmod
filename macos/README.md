# macOS (experimental)

Requires Python 3.9+, Xcode Command Line Tools, Steam Civilization V (Aspyr build
180925) with all non-map DLC, and Rosetta on Apple silicon. Standard UI only.

## Launcher

In Finder, open `macos/` and double-click **Lekmod Launcher.command**.
The original **Install Lekmod.command** opens the same launcher. The native UI
requires macOS 13 or later and builds automatically on first use.

To open it from a terminal:
```sh
python3 macos/install.py --gui
```

Steam libraries, including external drives, are detected automatically. Use
**Change…** to select `Civilization V.app` or its containing folder.

The launcher checks the supported game build, Steam installation, DLC folders,
native library, mod file contents, checkout version, and app signature before
launching through Steam. **Repair & Play** restores the installation, verifies it
again, and launches; **Repair** updates the files without starting the game.
The left-side **Install** and **Uninstall** controls manage Lekmod and Lekmap
separately. Uninstalling Lekmod restores a verified original gameplay library
from the retained backups; uninstalling Lekmap removes only its map scripts.
Both actions preserve saved games and the other component. An intentionally
uninstalled Lekmap stays uninstalled when repairing or launching Lekmod.
Close Civilization V before repairing. An unsupported game executable or unknown
library stops the workflow with an explanation rather than applying a guessed fix.

The launcher installs this checkout's native library and assets inside the game app: Lekmod
under `Contents/Assets/Assets/DLC/LEKMOD`, Lekmap under
`Contents/Assets/Assets/Maps/Lekmap`. Open the launcher before playing so changes
from Steam verification or a checkout update can be detected and repaired.
Windows release downloads are not used. The menu background is read from the
installed game's texture archive; game artwork is not included in this repository.

The built app is `macos/build/Lekmod Launcher.app`; it can be kept in the Dock.
It uses this checkout and the Python interpreter used to build it. Reopen the
`.command` file after moving the checkout or changing Python. Launcher preferences
live in `~/Library/Application Support/Lekmod Launcher/settings.json`, outside the
Steam installation, so cross-play can be restored after Steam replaces game files.

Ready installations receive one full check under the installation lock before
Steam opens. App-signature verification overlaps the content checks; a repair
still requires fresh validation before launch. When Civ V starts, the launcher
updates its controls using process discovery without scanning the game files
again. Full validation resumes after the game exits.

The previous app is retained in `.lekmod-backups` beside the game; its exact
path is printed after installation. To restore, close the game, move the current
app aside, and move the backup app into its place. Steam updates/Verify Files
may undo installation. The command-line installer remains available with
`python3 macos/install.py` and `--component lekmod`, `lekmap`, or `both`.

## Experimental Windows cross-play

Cross-play is opt-in and requires the same Lekmod release and required DLC on
both machines. For the supported Aspyr 180925 executable, the native library
adapts the multiplayer registration string to Windows build 403694 at a verified
call site. The game retains its original global build metadata and version
checks, and the installer preserves the setting across Lekmod updates.

Use the launcher's **Crossplay** switch; the preference is applied by
**Repair & Play** or **Repair**. It can also be enabled from a terminal while
the game is closed:

```sh
python3 macos/crossplay.py --enable
```

Launch through Steam, host a multiplayer game on Windows, and invite the Mac
Steam account. To disable cross-play, close the game and run:

```sh
python3 macos/crossplay.py --disable
```

## Launcher development checks

```sh
python3 -m unittest discover -s macos/tests -v
python3 macos/tests/check_launcher_lifecycle.py
python3 macos/build_launcher.py
```

The lifecycle check compiles the production Swift model and verifies that game
startup avoids a full scan, game exit requests validation, running games disable
launch and repair, and pending Steam launches retain their grace period.
