# LaTeX Editor

A modern, feature-rich LaTeX editor built with C++17 and Qt6.

## Description

LaTeX Editor is a powerful, user-friendly desktop application designed to streamline the process of creating and editing LaTeX documents. Built with C++17 and Qt6 framework, it offers a robust, efficient, and cross-platform solution for LaTeX enthusiasts, researchers, and professionals.

## Features

### Core Editing
- **Syntax Highlighting** - LaTeX commands, environments, BibTeX entries, and comments
- **Line Numbers** - With current line highlighting and error indicators
- **Syntax Error Detection** - Real-time detection of LaTeX errors (braces, environments, math delimiters)
- **Find & Replace** - Full search functionality with case-sensitive and whole-word options
- **LaTeX Toolbar** - Quick-insert buttons for common commands and structures

### File Management
- **Save & Save As** - Standard file operations with .tex file filtering
- **Recent Files Menu** - Access up to 10 recently opened files
- **Document Templates** - Pre-configured templates for Article, Report, Beamer presentations, and Letters

### Preview & Export
- **Auto-updating Preview** - Real-time preview with 500ms debounce
- **PDF Export** - Direct export to PDF format
- **Manual Rebuild** - Force preview refresh (Ctrl+R)

### LaTeX & BibTeX Support
- **BibTeX Syntax Highlighting** - Recognizes citation entries and fields
- **BibTeX Templates** - Quick-insert for @article, @book, @inproceedings, @misc
- **Citation Tools** - Easy insertion of \cite{} and bibliography sections

### Customization
- **Multiple Themes** - Choose from various color schemes
- **Spell Checking** - Framework ready for Hunspell integration (placeholder)

See [SHORTCUTS.md](SHORTCUTS.md) for complete keyboard shortcuts reference.

## Getting Started

### Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, or MSVC 19.14+)
- Qt 6.2 or higher
- CMake 3.16 or higher
- (Optional) Qt6WebEngineWidgets for enhanced preview capabilities

### Building from Source

1. Clone the repository:
   ```
   git clone https://github.com/dev-alt/latex-editor.git
   cd latex-editor
   ```

2. **Building on Windows:**
   ```powershell
   # Easy way - use the build script
   .\build.ps1           # Build in Release mode
   .\build.ps1 run       # Build and run

   # See BUILD_WINDOWS.md for detailed instructions
   ```

3. **If developing in WSL:** Sync to Windows first
   ```bash
   ./sync.sh             # Sync to Windows
   # Then use build.ps1 in Windows PowerShell
   ```

4. **Or build directly (Linux/macOS):**
   ```bash
   mkdir build && cd build
   cmake ..
   cmake --build .
   ./LaTeXEditor
   ```

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the [Your chosen license] - see the [LICENSE.md](LICENSE.md) file for details.

## Acknowledgments

- [Qt Framework](https://www.qt.io/)
- [List any other libraries or resources you've used]

