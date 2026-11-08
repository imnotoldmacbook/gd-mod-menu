#!/bin/bash
# Install script for GD Mod Menu

set -e

GD_DIR="/home/aspxcachyos/Desktop/Geometry Dash"
DLL_NAME="gd-mod-menu.dll"

echo "=== Installing GD Mod Menu ==="

if [ ! -f "$DLL_NAME" ]; then
    echo "Error: $DLL_NAME not found. Build first with ./build.sh"
    exit 1
fi

if [ ! -d "$GD_DIR" ]; then
    echo "Error: Geometry Dash directory not found at $GD_DIR"
    exit 1
fi

# Copy DLL
cp "$DLL_NAME" "$GD_DIR/"
echo "Copied $DLL_NAME to $GD_DIR/"

# Create a simple loader config if using adaf-dll
if [ -f "$GD_DIR/adaf-dll/tBot.dll" ]; then
    echo "TASbot detected - mod menu will integrate automatically"
fi

echo "Installation complete!"
echo "Use your DLL loader (GDDLLLoader.dll or adaf-dll) to inject the mod"