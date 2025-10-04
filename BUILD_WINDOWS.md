# Building on Windows

## Quick Start

### Option 1: PowerShell Build Script (Recommended)

```powershell
# Build in Release mode (default)
.\build.ps1

# Clean and rebuild
.\build.ps1 rebuild

# Build and run
.\build.ps1 run

# Build in Debug mode
.\build.ps1 build -Configuration Debug

# Get help
.\build.ps1 help
```

### Option 2: Qt Creator

1. Open Qt Creator
2. File → Open File or Project
3. Select `CMakeLists.txt` from this directory
4. Choose Qt 6.2+ kit
5. Click "Configure Project"
6. Build → Build All (Ctrl+B)
7. Run (Ctrl+R)

### Option 3: Visual Studio 2019/2022

1. File → Open → Folder
2. Select this directory
3. Wait for CMake configuration to complete
4. Build → Build All
5. Select LaTeXEditor.exe as startup item
6. Debug → Start Without Debugging (Ctrl+F5)

### Option 4: Manual CMake

```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
.\Release\LaTeXEditor.exe
```

## Prerequisites

### Required

- **CMake** 3.16 or higher
  - Download: https://cmake.org/download/
  - Add to PATH during installation

- **Qt 6.2** or higher
  - Download: https://www.qt.io/download-qt-installer
  - Install the MSVC 2019 64-bit component
  - Recommended: Qt 6.5 or later

- **Visual Studio** 2019 or 2022
  - With "Desktop development with C++" workload
  - Or just the Build Tools if you don't need the IDE

### Optional

- **Qt6WebEngineWidgets** - For enhanced preview (included in Qt installation)
- **Git** - For version control

## Setting Up Qt6

The build script will automatically search for Qt6 in standard locations:
- `C:\Qt\6.7.0\msvc2019_64`
- `C:\Qt\6.6.0\msvc2019_64`
- `C:\Qt\6.5.0\msvc2019_64`
- `C:\Qt\6.4.0\msvc2019_64`
- `C:\Qt\6.3.0\msvc2019_64`
- `C:\Qt\6.2.0\msvc2019_64`

### Manual Qt6 Configuration

If Qt6 is installed in a different location, set the environment variable:

```powershell
# PowerShell
$env:Qt6_DIR = "C:\Path\To\Qt\6.x.x\msvc2019_64\lib\cmake\Qt6"

# Or add to system environment variables permanently:
# System Properties → Environment Variables → New
# Variable: Qt6_DIR
# Value: C:\Path\To\Qt\6.x.x\msvc2019_64\lib\cmake\Qt6
```

```cmd
rem CMD
set Qt6_DIR=C:\Path\To\Qt\6.x.x\msvc2019_64\lib\cmake\Qt6
```

## Build Script Options

### Actions

- `build` (default) - Build the project
- `clean` - Remove build directory
- `rebuild` - Clean then build
- `run` - Build and launch the application
- `help` - Show help message

### Configuration Types

- `Release` (default) - Optimized build for distribution
- `Debug` - Debug symbols, no optimization
- `RelWithDebInfo` - Optimized with debug info
- `MinSizeRel` - Optimized for size

### Flags

- `-Verbose` - Show detailed build output

### Examples

```powershell
# Standard release build
.\build.ps1

# Debug build with verbose output
.\build.ps1 build -Configuration Debug -Verbose

# Clean rebuild in release mode
.\build.ps1 rebuild

# Build and run immediately
.\build.ps1 run

# Build with size optimization
.\build.ps1 build -Configuration MinSizeRel
```

## Troubleshooting

### CMake not found

```
✗ CMake not found. Please install CMake and add it to PATH.
```

**Solution:**
1. Download CMake from https://cmake.org/download/
2. Run installer and check "Add CMake to system PATH"
3. Restart PowerShell/Terminal

### Qt6 not found

```
! Qt6 not found in standard locations
```

**Solution:**
1. Install Qt6 from https://www.qt.io/download-qt-installer
2. Make sure to install the MSVC 2019 64-bit component
3. Set `Qt6_DIR` environment variable (see above)

### Compiler not found

```
error: No suitable compiler found
```

**Solution:**
1. Install Visual Studio 2019 or 2022
2. Include "Desktop development with C++" workload
3. Or install Visual Studio Build Tools

### Build fails with "Access Denied"

**Solution:**
1. Close Qt Creator, Visual Studio, or any application using the build directory
2. Run `.\build.ps1 clean`
3. Try building again

### Application doesn't run - Missing DLLs

**Solution:**
1. Qt DLLs are not in PATH
2. Use Qt Creator to run (it sets up the environment automatically)
3. Or manually copy Qt DLLs to the executable directory
4. Or add Qt bin directory to PATH:
   ```powershell
   $env:PATH = "C:\Qt\6.x.x\msvc2019_64\bin;$env:PATH"
   ```

## Development Workflow (WSL → Windows)

If developing in WSL:

```bash
# In WSL
cd /root/projects/LaTeXEditor
# Make changes...
./sync.sh              # Sync to Windows
```

```powershell
# In Windows PowerShell
cd C:\Users\andre\Desktop\Projects\LaTeXEditor
.\build.ps1 run        # Build and test
```

## Output

After a successful build, the executable will be at:
- **Release:** `build\Release\LaTeXEditor.exe`
- **Debug:** `build\Debug\LaTeXEditor.exe`

## Packaging for Distribution

After building in Release mode:

```powershell
cd build\Release
windeployqt LaTeXEditor.exe
```

This copies all required Qt DLLs and plugins to the release directory, making it ready for distribution.

## Clean Build

If you encounter issues, try a clean rebuild:

```powershell
.\build.ps1 rebuild
```

This removes the build directory and builds from scratch.
