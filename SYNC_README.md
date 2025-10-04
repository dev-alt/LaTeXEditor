# Sync Script Documentation

## Overview

The `sync.sh` script synchronizes the LaTeX Editor project from WSL/Linux to Windows for building and testing with Qt Creator or Visual Studio.

## Usage

```bash
# Incremental sync (default) - only copies new/modified files
./sync.sh

# Or explicitly:
./sync.sh incremental

# Full sync - deletes files in target that don't exist in source
./sync.sh full
```

## Target Location

**Windows Path:** `C:\Users\andre\Desktop\Projects\LaTeXEditor`
**WSL Path:** `/mnt/c/Users/andre/Desktop/Projects/LaTeXEditor`

## What Gets Synced

All source files, headers, CMake configuration, documentation, and resources.

## What Gets Excluded

The script automatically excludes:
- Build artifacts (`.o`, `.a`, `.so`, `.dll`, etc.)
- Build directories (`build/`, `cmake-build-*/`)
- IDE configuration (`.idea/`, `.vscode/`, `.qtcreator/`)
- Version control (`.git/`)
- Temporary files (`.swp`, `.bak`, `.tmp`)
- Qt generated files (`moc_*`, `qrc_*`, `ui_*`, `Makefile`)

See `.syncignore` for the complete list.

## Building on Windows After Sync

1. **Open in Qt Creator:**
   ```
   File → Open File or Project → Navigate to:
   C:\Users\andre\Desktop\Projects\LaTeXEditor\CMakeLists.txt
   ```

2. **Or build via command line:**
   ```bash
   cd C:\Users\andre\Desktop\Projects\LaTeXEditor
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

3. **Or use Visual Studio:**
   - Open the folder in Visual Studio
   - It will detect CMakeLists.txt automatically
   - Build → Build All

## Troubleshooting

### Sync fails with permission errors
- Ensure the Windows directory is not open in any application
- Close Qt Creator or Visual Studio before syncing

### Files not syncing
- Check if the file is in `.syncignore`
- Try running `./sync.sh full` for a complete sync

### Line ending issues
- The script automatically handles Linux (LF) to Windows (CRLF) conversion
- rsync preserves file permissions

## Notes

- The sync is **one-way** (WSL → Windows)
- Changes made in Windows will **not** sync back to WSL automatically
- If you modify files in Windows, manually copy them back to WSL or work primarily in WSL
