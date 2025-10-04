# Installing Qt6 on Windows (Minimal ~3-5 GB)

## Why Qt Installer Shows 60+ GB

The Qt installer shows the **total size of ALL components** (Android, iOS, WebAssembly, source code, debug symbols, etc.). You only need a **small subset** for this project.

## Minimal Installation Steps

### 1. Download Qt Online Installer
https://www.qt.io/download-qt-installer-oss

**File:** `qt-online-installer-windows-x64-4.x.x.exe` (~2 MB)

### 2. Run Installer

- Create Qt Account (free) or log in
- Accept license agreements (LGPL/GPL for open source)
- Choose installation folder: **`C:\Qt`** (recommended, script auto-detects this)

### 3. Select Components (CRITICAL - Only Check These!)

When you reach "Select Components" screen:

#### ✅ Check ONLY These Items:

```
Qt
└── Qt 6.7.3 (or latest 6.7.x)
    ├── ✅ MSVC 2019 64-bit          (~1.5 GB) ← REQUIRED
    ├── ✅ Qt WebEngine              (~800 MB) ← For preview window
    ├── ✅ Qt 5 Compatibility Module (~50 MB)  ← Sometimes needed
    └── ✅ Sources                   (~500 MB) ← OPTIONAL, uncheck to save space

Developer and Designer Tools
├── ✅ Qt Creator 13.x CDB Debugger (~500 MB) ← OPTIONAL but recommended
└── ✅ CMake 3.27.x                 (~100 MB) ← Only if you don't have CMake

Qt Design Studio
└── ❌ Uncheck all                 ← Not needed for this project
```

#### ❌ DO NOT Check:

- **Android builds** (saves ~10 GB)
- **iOS builds** (saves ~5 GB)
- **WebAssembly** (saves ~2 GB)
- **MinGW compiler** (we use MSVC)
- **Additional Libraries** (Qt Charts, Qt Data Visualization, etc.)
- **Debug Information Files** (saves ~5 GB)
- **Multiple Qt versions** (only install one version)
- **Qt Design Studio** (different tool, not needed)

### 4. Installation Size Breakdown

**Absolute Minimum (just compiler):**
- MSVC 2019 64-bit: ~1.5 GB

**Recommended for this project:**
- MSVC 2019 64-bit: ~1.5 GB
- Qt WebEngine: ~800 MB
- Qt Creator: ~500 MB
- CMake: ~100 MB
- **Total: ~3-4 GB**

**With sources (for reference):**
- Add ~500 MB
- **Total: ~4-5 GB**

### 5. After Installation

The installer will place Qt at:
```
C:\Qt\6.7.3\msvc2019_64\
```

Our `build.ps1` script automatically searches for Qt in these paths:
- `C:\Qt\6.7.0\msvc2019_64`
- `C:\Qt\6.6.0\msvc2019_64`
- `C:\Qt\6.5.0\msvc2019_64`
- `C:\Qt\6.4.0\msvc2019_64`
- `C:\Qt\6.3.0\msvc2019_64`
- `C:\Qt\6.2.0\msvc2019_64`

### 6. Build the Project

```powershell
# Navigate to project
cd C:\Users\andre\Desktop\Projects\LaTeXEditor

# Build (script auto-detects Qt)
.\build.ps1

# Or build and run
.\build.ps1 run
```

## Alternative: Offline Installer

If you have slow/metered internet:

1. Download **Qt 6.7.x Offline Installer** from:
   https://www.qt.io/offline-installers

2. Select file: `qt-opensource-windows-x86-6.7.x.exe` (~4 GB single download)

3. During install, still **uncheck** Android/iOS/WebAssembly to save disk space

## Verification

After installation, verify Qt is found:

```powershell
# Check if Qt6_DIR can be set
$qtPath = "C:\Qt\6.7.3\msvc2019_64"
if (Test-Path $qtPath) {
    Write-Host "✓ Qt6 found at $qtPath"
} else {
    Write-Host "✗ Qt6 not found, check installation path"
}
```

## Troubleshooting

### Still shows 60+ GB during installation

This is **total available size**, not what will be installed. Pay attention to the **checkboxes** you select, not the total size shown.

### CMake still can't find Qt6

If Qt installed to non-standard location:

```powershell
# Set environment variable
$env:Qt6_DIR = "C:\YourCustomPath\Qt\6.7.3\msvc2019_64\lib\cmake\Qt6"

# Then build
.\build.ps1
```

### Need even smaller installation?

Skip Qt Creator and use Visual Studio instead:
- Only install "MSVC 2019 64-bit" + "Qt WebEngine"
- **Size: ~2.3 GB**
- Build using `build.ps1` or Visual Studio

## What Each Component Does

| Component | Size | Purpose | Required? |
|-----------|------|---------|-----------|
| MSVC 2019 64-bit | 1.5 GB | Qt libraries for Windows | ✅ YES |
| Qt WebEngine | 800 MB | Chromium-based web rendering (preview) | ⚠️ Recommended |
| Qt Creator | 500 MB | IDE for Qt development | ⚠️ Recommended |
| CMake | 100 MB | Build system | Only if not installed |
| Sources | 500 MB | Qt source code (reference) | ❌ Optional |
| Android | 10+ GB | Mobile development | ❌ Not needed |
| iOS | 5+ GB | Mobile development | ❌ Not needed |
| WebAssembly | 2 GB | Browser deployment | ❌ Not needed |
| MinGW | 2 GB | Alternative compiler | ❌ Not needed (we use MSVC) |
| Debug Info | 5+ GB | Debugging Qt itself | ❌ Not needed |

## Summary

**For this LaTeX Editor project:**
- Download: 3-5 GB (not 60 GB!)
- Install time: 10-20 minutes
- Required components: MSVC 2019 64-bit + Qt WebEngine
- Optional: Qt Creator (makes development easier)

The 60 GB is if you select **everything** (all platforms, all modules, all versions, debug symbols, sources). You only need **~3 GB** for Windows desktop development.
