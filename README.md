# GD Mod Menu for Geometry Dash 2.113

A feature-rich mod menu DLL for Geometry Dash 2.113 with TASbot compatibility.

## Features

### Main Menu
- **Noclip** - Pass through objects without dying
- **Show Hitboxes** - Visualize collision boxes
- **Auto Clicker** - Automatic clicking with adjustable speed (1-60 CPS)
- **Practice Mode Helper** - Enhanced practice mode features
- **Show FPS** - Display FPS counter

### Gameplay
- Auto practice mode
- Custom gameplay modifications
- Quick test buttons

### TASbot Integration
- **TASbot Compatible Mode** - Ensures mod doesn't interfere with TASbot recordings
- **Sync Modes**:
  - 0 = None (default)
  - 1 = Frame-sync
  - 2 = Click-sync
- Reload/Load/Save TASbot configuration
- Direct TASbot hook initialization

### Visual
- Hitbox visualization
- Object ID display
- Wireframe mode
- FPS counter customization

### Settings
- Save/Load mod configuration
- Reset to defaults
- Persistent settings (saved to `modmenu_settings.json`)

## Installation

1. Build the DLL (see Building section)
2. Place `gd-mod-menu.dll` in your Geometry Dash folder
3. Use a DLL loader (like the included `GDDLLLoader.dll` or `adaf-dll`) to inject the mod
4. Launch Geometry Dash - a "Mod Menu" button will appear in the main menu and pause menu

## Building

### Option 1: GitHub Actions (Free Windows Build - Recommended for Linux users)
No local toolchain needed. Just push to GitHub or trigger manually:

1. Push this repo to GitHub (or fork it)
2. Go to **Actions** tab → **Build GD Mod Menu** → **Run workflow**
3. Download `gd-mod-menu.dll` from the workflow artifacts

Or create a tag to trigger a release:
```bash
git tag v1.0.0 && git push origin v1.0.0
```

### Option 2: Windows (Visual Studio)
```cmd
build.bat
```

### Option 3: Manual CMake (Windows)
```bash
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release
```

### Why not Linux native?
The `gd.h` headers use MSVC-specific inline assembly (`__asm { movss xmm0, ... }`) which GCC/MinGW doesn't support. Cross-compilation requires the Windows SDK (Visual Studio Build Tools) which is only available on Windows.

## Requirements

- **CMake** 3.0+
- **Clang** or **MSVC** (Visual Studio 2019/2022)
- **MinHook** (included as submodule)
- **cocos-headers** (included as submodule)
- **gd.h** (included as submodule)

## TASbot Compatibility

This mod is designed to work alongside TASbot (tBot.dll). It:

1. Detects TASbot automatically on startup
2. Provides configuration sync options
3. Avoids hooking conflicts with TASbot's hooks
4. Allows saving/loading TASbot click configurations
5. Has a "TASbot Compatible Mode" that disables features that might desync recordings

### TASbot Integration Details

The mod looks for these TASbot exports:
- `?init_hook@bot@@YAXXZ` - Initialize TASbot hooks
- `?load_click_config@bot@@YA_NXZ` - Load click configuration
- `?save_click_config@bot@@YA_NXZ` - Save click configuration

## Configuration

Settings are saved to `modmenu_settings.json` in the Geometry Dash folder:

```json
{
  "noclip": false,
  "showHitboxes": false,
  "autoClicker": false,
  "autoClickerSpeed": 10.0,
  "practiceMode": false,
  "showFPS": true,
  "tasbotCompatible": true,
  "tasbotSyncMode": 0
}
```

## Offsets

This mod uses hardcoded offsets for Geometry Dash 2.113 (Windows). If you're using a different version, you'll need to update `src/Offsets.hpp` with the correct offsets.

Current offsets (2.113):
- `MenuLayer::init` - 0x1907B0
- `PlayLayer::init` - 0x247ED0
- `PlayLayer::update` - 0x2523D0
- `PlayLayer::draw` - 0x25A740
- `PlayerObject::checkCollision` - 0x1F8A30
- `PauseLayer::init` - 0x1E5C60

## Credits

- Template based on [matcool/gd-mod-example](https://github.com/matcool/gd-mod-example)
- [gd.h](https://github.com/poweredbypie/gd.h) - GD reverse engineering headers
- [cocos-headers](https://github.com/HJfod/cocos-headers) - Cocos2d-x headers
- [MinHook](https://github.com/TsudaKageyu/minhook) - Hooking library
- TASbot by the TASbot team

## License

MIT License - Feel free to use and modify for your own projects.

## Disclaimer

This is a mod for educational purposes. Use at your own risk. Modifying game memory can cause instability. Always backup your save files.