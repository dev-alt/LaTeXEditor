# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

LaTeX Editor is a feature-rich desktop application built with C++17 and Qt6 that provides a modern interface for creating and editing LaTeX documents with real-time preview capabilities.

## Features

### Core Editing
- **Syntax Highlighting** - Color-coded LaTeX commands, environments, BibTeX entries, brackets, and comments
- **Line Numbers** - Left-margin line numbers with current line highlighting and error indicators
- **Auto-completion Toolbar** - Quick-insert buttons for LaTeX commands, math symbols, and document structures
- **Find & Replace** - Full-featured search with case sensitivity and whole word options
- **Syntax Error Detection** - Real-time detection of LaTeX errors with visual indicators and error panel

### File Management
- **Save As** - Save documents with a new filename
- **Recent Files** - Persistent menu of up to 10 recently opened files
- **Document Templates** - Pre-configured templates for Article, Report, Beamer presentations, and Letters

### Preview & Export
- **Auto-updating Preview** - Live preview with 500ms debounce as you type
- **PDF Export** - Direct export of editor content to PDF format
- **Manual Rebuild** - Force preview refresh when needed

### LaTeX & BibTeX Support
- **BibTeX Integration** - Syntax highlighting and quick-insert templates for citations
- **Bibliography Templates** - Pre-configured entries for @article, @book, @inproceedings, @misc
- **Citation Commands** - Quick insertion of \cite{} and bibliography sections

### Customization
- **Theme System** - Multiple color themes for editor and UI
- **Spell Checking** - Framework for spell checking (placeholder for Hunspell integration)

## Keyboard Shortcuts

| Action | Shortcut | Menu Location |
|--------|----------|---------------|
| New File | Ctrl+N | File → New |
| Open File | Ctrl+O | File → Open |
| Save | Ctrl+S | File → Save |
| Save As | Ctrl+Shift+S | File → Save As |
| Export to PDF | Ctrl+E | File → Export to PDF |
| Find & Replace | Ctrl+F | Edit → Find and Replace |
| Show Errors | Ctrl+L | View → Show Errors |
| Rebuild Preview | Ctrl+R | View → Rebuild Preview |
| Quit | Ctrl+Q | File → Exit |

## Build System

This project uses CMake (minimum 3.16) with Qt6.

### Syncing to Windows (WSL Development)

If developing in WSL and building on Windows:

```bash
# Sync to Windows
./sync.sh              # Incremental sync (default)
./sync.sh full         # Full sync with deletion

# Target: C:\Users\andre\Desktop\Projects\LaTeXEditor
```

See [SYNC_README.md](SYNC_README.md) for details.

### Build Commands

**Windows (PowerShell):**
```powershell
.\build.ps1           # Build in Release mode
.\build.ps1 run       # Build and run
.\build.ps1 rebuild   # Clean rebuild
.\build.ps1 help      # Show all options

# See BUILD_WINDOWS.md for details
```

**Linux/macOS:**
```bash
mkdir build && cd build
cmake ..
cmake --build .
./LaTeXEditor
```

### CMake Configuration Notes

- C++ Standard: C++17 (set in CMakeLists.txt:4)
- Qt6 modules: Widgets (required), WebEngineWidgets (optional)
- Auto MOC, RCC, and UIC are enabled
- macOS: Builds as MACOSX_BUNDLE with deployment target 11.0, x86_64 architecture
- Preview fallback: If Qt6WebEngineWidgets is not found, falls back to QTextBrowser

## Architecture

The codebase follows an **MVC (Model-View-Controller)** pattern with clear separation:

### Directory Structure

- `src/models/` - Data models
  - `DocumentModel` - LaTeX document content and state
  - `Theme` - Color scheme definitions

- `src/views/` - UI components
  - `MainWindow` - Primary application window with editor and menus
  - `PreviewWindow` - LaTeX preview pane (QWebEngineView or QTextBrowser)
  - `LatexToolbar` - LaTeX command toolbar with quick-insert buttons
  - `FindReplaceDialog` - Find and replace dialog with search options

- `src/controllers/` - Business logic
  - `EditorController` - Manages editor text changes and model updates
  - `FileController` - Handles file I/O operations (new/open/save/saveAs)
  - `PreviewController` - Manages debounced auto-preview updates (500ms delay)
  - `LatexToolbarController` - Handles toolbar interactions

- `src/utils/` - Utilities
  - `LaTeXHighlighter` - Syntax highlighting for LaTeX and BibTeX
  - `LaTeXErrorChecker` - Real-time syntax error detection for LaTeX documents
  - `ThemeManager` - Singleton managing theme selection and application
  - `CodeEditor` - Enhanced QPlainTextEdit with line numbers, error indicators, and current line highlighting

### Key Patterns

- **Singleton Pattern**: `ThemeManager::getInstance()` provides global theme access
- **Qt Signals/Slots**: Communication between MVC layers uses Qt's signal/slot mechanism
- **Controller Mediation**: Controllers connect model signals to view slots and vice versa

### Component Relationships

```
MainWindow (View)
  ├─ owns CodeEditor (custom editor with line numbers)
  ├─ owns DocumentModel (Model)
  ├─ owns FileController → interacts with DocumentModel
  │   └─ manages recent files via QSettings
  ├─ owns EditorController → connects editor ↔ DocumentModel
  ├─ owns PreviewWindow (View) + PreviewController
  │   └─ auto-updates with 500ms debounce timer
  ├─ owns LatexToolbar (View) + LatexToolbarController
  └─ owns FindReplaceDialog (spawned on demand)

ThemeManager (Singleton)
  └─ broadcasts theme changes to all components

Document Templates
  └─ stored as string literals in MainWindow::getTemplate()
```

## Qt-Specific Considerations

- **Q_OBJECT macro**: Required in all classes using signals/slots
- **MOC (Meta-Object Compiler)**: Automatically processes headers with Q_OBJECT via CMAKE_AUTOMOC
- **Resource files**: resources.qrc compiled into executable
- **Conditional compilation**: `#ifdef QT_WEBENGINEWIDGETS_LIB` for preview backend selection

## Platform-Specific Notes

### macOS
- Builds as application bundle with custom icon (latex_editor_icon.icns)
- Post-build step runs macdeployqt for deployment
- Info.plist.in template for bundle configuration

### Cross-Platform
- Uses Qt's cross-platform file dialogs and path handling
- Theme system uses QColor for platform-independent colors

## Implementation Notes

### Recent Files
- Stored persistently using QSettings (defaults to platform-specific locations)
- Maximum 10 files tracked
- Automatically removes non-existent files when accessed

### Preview System
- Uses QTimer with 500ms single-shot delay for debouncing
- Automatically updates on every text change (debounced)
- Manual rebuild available via Ctrl+R
- Converts LaTeX to HTML with MathJax for math rendering
- Supports:
  - Common LaTeX commands (sections, formatting, lists)
  - Math environments: inline `$...$` and display `$$...$$`
  - MathJax 3 loaded from CDN (requires internet connection)
  - LaTeX-like typography with proper styling

### Syntax Highlighting
- Highlights: LaTeX commands, environments, brackets, comments
- BibTeX support: @entry types and field names
- Theme-aware coloring

### Error Detection
- Real-time syntax checking with 1-second debounce
- Detects:
  - Unmatched braces `{}`
  - Unmatched brackets `[]`
  - Unmatched `\begin{}` and `\end{}` environments
  - Unmatched math delimiters `$`, `$$`, `\(`, `\)`, `\[`, `\]`
- Visual indicators:
  - Red `!` in line number margin
  - Red wavy underline on errors
  - Status bar shows error count
- Error panel (Ctrl+L) lists all errors with line/column numbers

### PDF Export
- Uses QPrinter to export editor content directly to PDF
- Note: Exports plain text, not rendered LaTeX (for rendered output, compile LaTeX externally)

### Spell Checking
- Currently a placeholder implementation
- Framework in place for future Hunspell integration
- Would require: QSyntaxHighlighter subclass, dictionary files, context menu for suggestions

## Known Limitations

1. **Preview rendering** - MathJax-based HTML preview (requires internet for CDN); complex LaTeX packages/commands not fully supported (use external LaTeX compiler for full rendering)
2. **Spell checking** - Placeholder only, no actual spell checking yet
3. **Multi-file projects** - Not yet implemented (no `\include` or `\input` support)
4. **Auto-save** - Not implemented
5. **Error detection** - Basic syntax checking only; does not validate LaTeX command semantics or package requirements
6. **Bibliography compilation** - BibTeX syntax highlighting only; no automatic compilation of .bib files
