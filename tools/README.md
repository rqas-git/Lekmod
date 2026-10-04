# Packaging for standard Lekmod v35.4

The checked-in `LEKMOD/CvGameCore_Expansion2.dll` is the official v35.4 binary.
The packager pins its SHA-256 to
`a5ac79567eff7c738f12c655457d2255199ef5da1e9e43aeb0e2b688b9161fd2`.
It accepts that binary and rejects custom or modified Windows DLLs, including
the previous audit build, before creating the package.

```sh
python3 tools/package_lekmod.py --destination /path/to/new/release/LEKMOD
```

The destination must be a new directory outside the source `LEKMOD` folder.
`--dll /path/to/official/CvGameCore_Expansion2.dll` accepts another copy of the
same official binary. UI materialization and packaging improvements are retained.

The native C++ sources retain tested performance improvements and Mac adapters.
A Windows source build is a development artifact, not an accepted substitute for
the public-release binary. The Visual C++ v90 `Mod` configuration and
`LEKMOD_DLL/build_mod.bat` remain available for development.

# Building and reusing a macOS core

```sh
python3 macos/build.py --release
```

This rebuilds the native library and records provenance after the import and ABI
checks pass. `macos/install.py --skip-build` requires matching source, library
bytes, and release configuration. A library from before the stock-behavior
restoration is rejected. Build validation does not establish Mac–Windows
multiplayer compatibility; the cross-play option remains experimental.

See [the compatibility policy](../COMPATIBILITY.md) for the restored rules,
retained improvements, test commands, and remaining live validation.

# Agent runtime testing

The repository includes the complete [Lekmod test skill](../skills/lekmod-test/SKILL.md),
including its native runner, background guard, multiplayer Lua hooks, and Steam
peer reader. Follow the [Mac and UTM Windows setup guide](../skills/lekmod-test/references/utm-setup.md)
to provision another testing machine and install the skill. The AI runner is a
CLI; two-client multiplayer still requires per-run host/join orchestration.
