#!/bin/bash

# Sync LaTeX Editor project to Windows
# Usage: ./sync.sh [full|incremental]

WINDOWS_PATH="/mnt/c/Users/andre/Desktop/Projects/LaTeXEditor"
SOURCE_PATH="/root/projects/LaTeXEditor"

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

# Default to incremental sync
SYNC_TYPE="${1:-incremental}"

echo -e "${BLUE}=== LaTeX Editor Sync to Windows ===${NC}"
echo -e "${BLUE}Source: ${NC}$SOURCE_PATH"
echo -e "${BLUE}Target: ${NC}$WINDOWS_PATH"
echo -e "${BLUE}Type: ${NC}$SYNC_TYPE"
echo ""

# Create target directory if it doesn't exist
if [ ! -d "$WINDOWS_PATH" ]; then
    echo -e "${YELLOW}Creating target directory...${NC}"
    mkdir -p "$WINDOWS_PATH"
fi

# Rsync options
RSYNC_OPTS="-av --progress"

# Exclude patterns
EXCLUDE_OPTS=(
    --exclude='.git/'
    --exclude='build/'
    --exclude='cmake-build-*/'
    --exclude='.idea/'
    --exclude='.vscode/'
    --exclude='.qtcreator/'
    --exclude='.claude/'
    --exclude='*.o'
    --exclude='*.a'
    --exclude='*.so'
    --exclude='*.dll'
    --exclude='*.dylib'
    --exclude='*.user'
    --exclude='*.autosave'
    --exclude='moc_*'
    --exclude='qrc_*'
    --exclude='ui_*'
    --exclude='Makefile'
    --exclude='.DS_Store'
    --exclude='*.swp'
    --exclude='*.bak'
    --exclude='*.tmp'
)

if [ "$SYNC_TYPE" == "full" ]; then
    echo -e "${GREEN}Performing FULL sync (will delete files in target that don't exist in source)...${NC}"
    RSYNC_OPTS="$RSYNC_OPTS --delete"
else
    echo -e "${GREEN}Performing INCREMENTAL sync (will only copy new/modified files)...${NC}"
fi

echo ""

# Perform the sync
rsync $RSYNC_OPTS "${EXCLUDE_OPTS[@]}" "$SOURCE_PATH/" "$WINDOWS_PATH/"

RSYNC_EXIT=$?

echo ""

if [ $RSYNC_EXIT -eq 0 ]; then
    echo -e "${GREEN}✓ Sync completed successfully!${NC}"
    echo ""
    echo -e "${BLUE}Files synced to:${NC} $WINDOWS_PATH"
    echo -e "${BLUE}You can now build in Windows using:${NC}"
    echo "  cd $WINDOWS_PATH"
    echo "  mkdir build && cd build"
    echo "  cmake .."
    echo "  cmake --build ."
else
    echo -e "${RED}✗ Sync failed with error code $RSYNC_EXIT${NC}"
    exit $RSYNC_EXIT
fi
