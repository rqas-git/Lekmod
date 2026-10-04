# Lekmod Launcher for Mac

Open the launcher DMG, drag **Lekmod Launcher** into **Applications**, then open
the app and choose **Repair**. Steam Civilization V and the required DLC must
already be installed. The app includes Python, the Lekmod/Lekmap files and a
verified native core; it does not depend on the original checkout or Homebrew.

The DMG's Read Me records its processor architecture and minimum macOS version.
These follow the build machine and bundled Python runtime. The current local
build targets Apple Silicon and macOS 26 or later. It is ad-hoc signed; public
distribution still needs Developer ID signing and notarization.

Packaging does not resolve the experimental Windows crossplay findings described
in [COMPATIBILITY.md](../COMPATIBILITY.md).

## Build a DMG

Building requires Python and Xcode command-line tools. First build the native
release against the supported installed Civilization V app:

```sh
python3 macos/build.py --release
python3 -m venv macos/build/launcher-env
macos/build/launcher-env/bin/python -m pip install -r macos/launcher-requirements.txt
macos/build/launcher-env/bin/python macos/package_launcher.py
```

The DMG is written to `macos/build/Lekmod-v35.4.003-Launcher-arm64.dmg` on the
current checkout and build machine. `--output PATH` selects another destination.
The packager rejects a core whose release manifest does not match the source,
checks the copied payload, signs and verifies the app, and includes an
Applications shortcut. The packaged service installs the verified prebuilt core
instead of invoking a compiler.

The runtime uses PyInstaller's documented
[one-directory layout](https://www.pyinstaller.org/en/stable/usage.html), with
paths resolved relative to the app so moving it does not break service commands.

For development, the launcher can still run directly against this checkout:

```sh
python3 macos/build_launcher.py --open
```
