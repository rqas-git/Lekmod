# Packaging for standard Lekmod v35.3

The checked-in `LEKMOD/CvGameCore_Expansion2.dll` is the official v35.3 binary.
The packager pins its SHA-256 to
`c8c265d26e6d67bab7c99371692a5b001d274cea6e80a3d9794b5626c994f357`.
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
