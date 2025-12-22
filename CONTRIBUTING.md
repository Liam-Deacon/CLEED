# Contributing

## Build prerequisites

CLEED builds “headless” (CLI tools + libraries) by default. GUI frontends are optional and require Qt.

### macOS (Homebrew)

```bash
brew install cmake ninja libtiff qt
```

Qt is keg-only on some setups; if `cmake` can’t find it, pass:

```bash
cmake -S . -B build -DWITH_QT=ON -DCMAKE_PREFIX_PATH="$(brew --prefix qt)"
```

### Debian/Ubuntu (apt)

Headless build deps:

```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake ninja-build pkg-config libtiff-dev
```

Qt6 GUI deps:

```bash
sudo apt-get install -y qt6-base-dev qt6-svg-dev
```

### Windows (winget + vcpkg)

Toolchain:

```powershell
winget install -e --id Microsoft.VisualStudio.2022.Community
winget install -e --id Kitware.CMake
winget install -e --id Ninja-build.Ninja
winget install -e --id Git.Git
winget install -e --id Python.Python.3.11
```

System libraries (matches CI approach) via vcpkg:

```powershell
git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg.exe install tiff:x64-windows
```

Qt6: install via the official Qt installer, then point CMake at it (example):

```powershell
cmake -S . -B build -DWITH_QT=ON -DCMAKE_PREFIX_PATH="C:\Qt\6.6.0\msvc2019_64"
```

## Build and test

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Pre-commit hooks (recommended)

This repository uses `pre-commit` for lightweight, fast checks on changed files.

Install and enable the git hook:

```bash
python3 -m pip install pre-commit
pre-commit install
```

Run the hooks manually:

```bash
pre-commit run --all-files
```
