#!/usr/bin/env pwsh
# Find Qt6 Installation on Windows

Write-Host "Searching for Qt6 installation..." -ForegroundColor Cyan
Write-Host ""

# Common installation paths
$searchPaths = @(
    "C:\Qt",
    "C:\Qt6",
    "$env:USERPROFILE\Qt",
    "C:\Program Files\Qt",
    "C:\Program Files (x86)\Qt",
    "D:\Qt",
    "E:\Qt"
)

$found = @()

foreach ($basePath in $searchPaths) {
    if (Test-Path $basePath) {
        Write-Host "[Searching] $basePath" -ForegroundColor Yellow

        # Look for Qt versions
        Get-ChildItem -Path $basePath -Directory -ErrorAction SilentlyContinue | ForEach-Object {
            $versionPath = $_.FullName

            # Look for msvc2019_64 or similar
            Get-ChildItem -Path $versionPath -Directory -Filter "msvc*64" -ErrorAction SilentlyContinue | ForEach-Object {
                $qtPath = $_.FullName
                $cmakePath = Join-Path $qtPath "lib\cmake\Qt6"

                if (Test-Path $cmakePath) {
                    $configFile = Join-Path $cmakePath "Qt6Config.cmake"
                    if (Test-Path $configFile) {
                        $found += @{
                            Path = $qtPath
                            CMakePath = $cmakePath
                            ConfigFile = $configFile
                        }
                        Write-Host "[FOUND] $qtPath" -ForegroundColor Green
                    }
                }
            }
        }
    }
}

Write-Host ""
Write-Host "=== Results ===" -ForegroundColor Cyan

if ($found.Count -eq 0) {
    Write-Host "No Qt6 installations found." -ForegroundColor Red
    Write-Host ""
    Write-Host "Please verify Qt6 is installed:" -ForegroundColor Yellow
    Write-Host "1. Check Qt Maintenance Tool in Start Menu"
    Write-Host "2. Or install from: https://www.qt.io/download-qt-installer-oss"
    Write-Host ""
    Write-Host "Make sure you installed:" -ForegroundColor Yellow
    Write-Host "  - Qt 6.x.x"
    Write-Host "  - MSVC 2019 64-bit component"
} else {
    Write-Host "Found $($found.Count) Qt6 installation(s):" -ForegroundColor Green
    Write-Host ""

    for ($i = 0; $i -lt $found.Count; $i++) {
        $qt = $found[$i]
        Write-Host "[$($i + 1)] $($qt.Path)" -ForegroundColor Green
        Write-Host "    CMake path: $($qt.CMakePath)" -ForegroundColor Gray
    }

    Write-Host ""
    Write-Host "To use Qt6, run ONE of these commands:" -ForegroundColor Cyan
    Write-Host ""

    $qt = $found[0]

    Write-Host "Option 1: Set for current PowerShell session" -ForegroundColor Yellow
    Write-Host "`$env:Qt6_DIR = `"$($qt.CMakePath)`"" -ForegroundColor White
    Write-Host ".\build.ps1" -ForegroundColor White
    Write-Host ""

    Write-Host "Option 2: Set permanently (recommended)" -ForegroundColor Yellow
    Write-Host "[System.Environment]::SetEnvironmentVariable('Qt6_DIR', '$($qt.CMakePath)', 'User')" -ForegroundColor White
    Write-Host "# Then restart PowerShell and run:" -ForegroundColor Gray
    Write-Host ".\build.ps1" -ForegroundColor White
    Write-Host ""

    Write-Host "Option 3: Update build.ps1 search paths" -ForegroundColor Yellow
    Write-Host "Edit build.ps1 and add this path to the `$qt6Paths array:" -ForegroundColor Gray
    Write-Host "`"$($qt.Path)`"" -ForegroundColor White
}
