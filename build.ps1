#!/usr/bin/env pwsh
# LaTeX Editor Build Script for Windows
# Usage: .\build.ps1 [clean|rebuild|run|help]

param(
    [Parameter(Position=0)]
    [ValidateSet('build', 'clean', 'rebuild', 'run', 'help')]
    [string]$Action = 'build',

    [Parameter()]
    [ValidateSet('Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel')]
    [string]$Configuration = 'Release',

    [Parameter()]
    [switch]$VerboseBuild
)

# Color functions
function Write-ColorOutput {
    param(
        [Parameter(Mandatory=$true)]
        [string]$Message,
        [Parameter()]
        [ValidateSet('Green', 'Yellow', 'Red', 'Cyan', 'Blue', 'White')]
        [string]$Color = 'White'
    )

    $colorMap = @{
        'Green'  = 'Green'
        'Yellow' = 'Yellow'
        'Red'    = 'Red'
        'Cyan'   = 'Cyan'
        'Blue'   = 'Blue'
        'White'  = 'White'
    }

    Write-Host $Message -ForegroundColor $colorMap[$Color]
}

function Write-Success { Write-ColorOutput -Message $args[0] -Color Green }
function Write-Info { Write-ColorOutput -Message $args[0] -Color Cyan }
function Write-Warning { Write-ColorOutput -Message $args[0] -Color Yellow }
function Write-Error { Write-ColorOutput -Message $args[0] -Color Red }

# Help function
function Show-Help {
    Write-Info "=== LaTeX Editor Build Script ==="
    Write-Host ""
    Write-Host "Usage: .\build.ps1 [Action] [-Configuration <Config>] [-VerboseBuild]"
    Write-Host ""
    Write-Host "Actions:"
    Write-Host "  build    - Build the project (default)"
    Write-Host "  clean    - Clean build directory"
    Write-Host "  rebuild  - Clean and build"
    Write-Host "  run      - Build and run the application"
    Write-Host "  help     - Show this help message"
    Write-Host ""
    Write-Host "Options:"
    Write-Host "  -Configuration  Debug|Release|RelWithDebInfo|MinSizeRel (default: Release)"
    Write-Host "  -VerboseBuild   Show detailed build output"
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build.ps1                    # Build in Release mode"
    Write-Host "  .\build.ps1 rebuild -VerboseBuild   # Clean rebuild with verbose output"
    Write-Host "  .\build.ps1 run -Configuration Debug  # Build and run in Debug mode"
    exit 0
}

if ($Action -eq 'help') {
    Show-Help
}

# Project paths
$ProjectRoot = $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build"
$ExeName = "LaTeXEditor.exe"

Write-Info "=== LaTeX Editor Build Script ==="
Write-Info "Project: $ProjectRoot"
Write-Info "Build Directory: $BuildDir"
Write-Info "Configuration: $Configuration"
Write-Host ""

# Check for CMake
Write-Info "Checking prerequisites..."
try {
    $cmakeVersion = & cmake --version 2>&1 | Select-Object -First 1
    Write-Success "[OK] CMake found: $cmakeVersion"
} catch {
    Write-Error "[ERROR] CMake not found. Please install CMake and add it to PATH."
    Write-Info "  Download from: https://cmake.org/download/"
    exit 1
}

# Check for Qt6
$qt6Found = $false
$qt6Paths = @(
    $env:Qt6_DIR,
    "C:\Qt\6.7.3\msvc2019_64",
    "C:\Qt\6.7.0\msvc2019_64",
    "C:\Qt\6.6.0\msvc2019_64",
    "C:\Qt\6.5.0\msvc2019_64",
    "C:\Qt\6.4.0\msvc2019_64",
    "C:\Qt\6.3.0\msvc2019_64",
    "C:\Qt\6.2.0\msvc2019_64"
)

foreach ($qtPath in $qt6Paths) {
    if ($qtPath -and (Test-Path $qtPath)) {
        $env:Qt6_DIR = Join-Path $qtPath "lib\cmake\Qt6"
        $env:PATH = "$qtPath\bin;$env:PATH"
        Write-Success "[OK] Qt6 found: $qtPath"
        $qt6Found = $true
        break
    }
}

if (-not $qt6Found) {
    Write-Warning "[WARN] Qt6 not found in standard locations"
    Write-Info "  Please set Qt6_DIR environment variable or install Qt6"
    Write-Info "  Download from: https://www.qt.io/download-qt-installer"
    Write-Info "  Continuing anyway - CMake will search system paths..."
}

Write-Host ""

# Clean action
if ($Action -eq 'clean' -or $Action -eq 'rebuild') {
    Write-Info "Cleaning build directory..."
    if (Test-Path $BuildDir) {
        Remove-Item -Recurse -Force $BuildDir
        Write-Success "[OK] Build directory cleaned"
    } else {
        Write-Info "Build directory doesn't exist, skipping clean"
    }
    Write-Host ""

    if ($Action -eq 'clean') {
        Write-Success "=== Clean completed ==="
        exit 0
    }
}

# Create build directory
if (-not (Test-Path $BuildDir)) {
    Write-Info "Creating build directory..."
    New-Item -ItemType Directory -Path $BuildDir | Out-Null
    Write-Success "[OK] Build directory created"
}

# Change to build directory
Set-Location $BuildDir

# CMake configure
Write-Info "Configuring with CMake..."
Write-Host ""

$cmakeArgs = @(
    "..",
    "-DCMAKE_BUILD_TYPE=$Configuration"
)

if ($env:Qt6_DIR) {
    $cmakeArgs += "-DQt6_DIR=$env:Qt6_DIR"
}

try {
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        throw "CMake configuration failed"
    }
    Write-Host ""
    Write-Success "[OK] CMake configuration successful"
} catch {
    Write-Host ""
    Write-Error "[ERROR] CMake configuration failed"
    Set-Location $ProjectRoot
    exit 1
}

Write-Host ""

# Build
Write-Info "Building project..."
Write-Host ""

$buildArgs = @(
    "--build", ".",
    "--config", $Configuration
)

if ($VerboseBuild) {
    $buildArgs += "--verbose"
}

# Add parallel build flag
$buildArgs += "--parallel"

try {
    & cmake @buildArgs
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed"
    }
    Write-Host ""
    Write-Success "[OK] Build successful"
} catch {
    Write-Host ""
    Write-Error "[ERROR] Build failed"
    Set-Location $ProjectRoot
    exit 1
}

# Find executable
$exePaths = @(
    (Join-Path $BuildDir "$Configuration\$ExeName"),
    (Join-Path $BuildDir $ExeName)
)

$exePath = $null
foreach ($path in $exePaths) {
    if (Test-Path $path) {
        $exePath = $path
        break
    }
}

Write-Host ""
if ($exePath) {
    Write-Success "=== Build completed successfully ==="
    Write-Info "Executable: $exePath"
    Write-Host ""

    # Run if requested
    if ($Action -eq 'run') {
        Write-Info "Launching LaTeX Editor..."
        Write-Host ""
        & $exePath
    } else {
        Write-Info "To run the application:"
        Write-ColorOutput "  .\build.ps1 run" -Color Yellow
        Write-Host ""
        Write-Info "Or directly:"
        Write-ColorOutput "  $exePath" -Color Yellow
    }
} else {
    Write-Warning "[WARN] Build completed but executable not found"
    Write-Info "  Expected locations:"
    foreach ($path in $exePaths) {
        Write-Info "    - $path"
    }
}

# Return to project root
Set-Location $ProjectRoot
