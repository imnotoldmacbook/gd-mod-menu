#pragma once

// Geometry Dash 2.113 Offsets (Windows)
// These offsets need to be verified for the specific version

namespace offsets {
    // MenuLayer
    constexpr uintptr_t MenuLayer_init = 0x1907B0;

    // PlayLayer
    constexpr uintptr_t PlayLayer_init = 0x247ED0;
    constexpr uintptr_t PlayLayer_update = 0x2523D0;
    constexpr uintptr_t PlayLayer_draw = 0x25A740;
    constexpr uintptr_t PlayLayer_onQuit = 0x24F8B0;
    constexpr uintptr_t PlayLayer_destroyPlayer = 0x24E320;
    constexpr uintptr_t PlayLayer_postUpdate = 0x255A80;
    constexpr uintptr_t PlayLayer_resetLevel = 0x24F2E0;
    constexpr uintptr_t PlayLayer_updateProgress = 0x251F40;

    // PlayerObject
    constexpr uintptr_t PlayerObject_init = 0x1F7C20;
    constexpr uintptr_t PlayerObject_update = 0x1FC8D0;
    constexpr uintptr_t PlayerObject_playerJump = 0x1F9A50;
    constexpr uintptr_t PlayerObject_checkCollision = 0x1F8A30;
    constexpr uintptr_t PlayerObject_destroyPlayer = 0x1F9120;
    constexpr uintptr_t PlayerObject_pushButton = 0x1F9E80;
    constexpr uintptr_t PlayerObject_releaseButton = 0x1FA0C0;

    // PauseLayer
    constexpr uintptr_t PauseLayer_init = 0x1E5C60;
    constexpr uintptr_t PauseLayer_onResume = 0x1E6890;
    constexpr uintptr_t PauseLayer_onRestart = 0x1E6A20;
    constexpr uintptr_t PauseLayer_onQuit = 0x1E6B50;

    // GameManager
    constexpr uintptr_t GameManager_sharedState = 0xC4A50;

    // LevelEditorLayer
    constexpr uintptr_t LevelEditorLayer_init = 0x1685D0;

    // GJBaseGameLayer
    constexpr uintptr_t GJBaseGameLayer_init = 0x12F4A0;

    // FLAlertLayer
    constexpr uintptr_t FLAlertLayer_create = 0x22680;
    constexpr uintptr_t FLAlertLayer_show = 0x23560;

    // CCMenuItemSpriteExtra
    constexpr uintptr_t CCMenuItemSpriteExtra_create = 0x18EE0;

    // CCMenuItemToggler
    constexpr uintptr_t CCMenuItemToggler_create = 0x19600;

    // ButtonSprite
    constexpr uintptr_t ButtonSprite_create = 0x137D0;

    // More offsets can be added as needed
} // namespace offsets
