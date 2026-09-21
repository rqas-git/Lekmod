# Building a Windows release

The checked-in `LEKMOD/CvGameCore_Expansion2.dll` predates the current source. Do not distribute it as a current release. The New Zealand Lua preserves science awards when running on an older core, but native engine fixes still require rebuilding the DLL.

On Windows with the project's Visual C++ v90 toolchain and MSBuild installed, build the `Mod` configuration. From the repository root in a configured developer command prompt:

```bat
MSBuild LEKMOD_DLL\CvGameCoreDLL_Expansion2\CvGameCoreDLL_Expansion2.vs2013.sln /p:Configuration=Mod /p:Platform=x86 /m
```

`LEKMOD_DLL\build_mod.bat` also supports the repository's existing Visual Studio 2008/MSBuild 12 installation layout and resolves the solution relative to the script.

Package the actual build output, using the output path reported by MSBuild:

```bat
python tools\package_lekmod.py --dll "path\to\built\CvGameCore_Expansion2.dll" --destination "path\to\release\LEKMOD"
```

The destination must be a new directory outside the source `LEKMOD` folder. The packager checks the PE signature and the `ChangeOverflowResearch` and `GetLekmodCoreVersion` capability names before creating the package, then copies the supplied DLL. These checks prevent accidentally packaging the known older core; they are not a substitute for a Windows build and in-game validation. `Game.GetLekmodCoreVersion()` returns `20260912` for the core containing this audit's native fixes.

# Reusing a macOS build

`python3 macos/build.py --release` records a manifest beside the linked library only after the import/ABI checks pass and the source fingerprint remains stable. A subsequent `python3 macos/install.py --skip-build` requires matching source, library bytes, and release configuration. Missing manifests, old builds, debug builds, and libraries replaced during installation are rejected before the staged app is committed. Rebuild without `--skip-build` when provenance is missing or stale.
