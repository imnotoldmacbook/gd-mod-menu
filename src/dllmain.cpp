#include "includes.h"
#include "ModManager.hpp"
#include "ModMenuLayer.hpp"
#include "Offsets.hpp"

using namespace modmenu;
using namespace offsets;

// Hook function pointers
bool (__thiscall* g_MenuLayer_init)(CCLayer* self);
bool __fastcall MenuLayer_init_H(CCLayer* self, void*) {
    if (!g_MenuLayer_init(self)) return false;

    auto menuLayer = static_cast<gd::MenuLayer*>(self);
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    // Create mod menu button
    auto btnSprite = gd::ButtonSprite::create("Mod Menu", 100, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f);
    auto modMenuBtn = gd::CCMenuItemSpriteExtra::create(
        btnSprite,
        menuLayer,
        menu_selector(ModMenuLayer::onModMenuButton)
    );

    // Find the main menu (usually the bottom menu with more games, etc.)
    CCArray* children = menuLayer->getChildren();
    CCObject* obj;
    CCARRAY_FOREACH(children, obj) {
        auto node = dynamic_cast<CCNode*>(obj);
        if (node && dynamic_cast<CCMenu*>(node)) {
            auto menu = static_cast<CCMenu*>(node);
            // Add our button to the menu
            menu->addChild(modMenuBtn);
            // Position it appropriately
            modMenuBtn->setPosition({winSize.width - 80, 35});
            break;
        }
    }

    // Initialize ModManager (singleton)
    ModManager::getInstance();

    return true;
}

// PlayLayer hooks for noclip, etc.
bool (__thiscall* g_PlayLayer_init)(CCLayer* self, void* level);
bool __fastcall PlayLayer_init_H(CCLayer* self, void*, void* level) {
    if (!g_PlayLayer_init(self, level)) return false;

    auto playLayer = static_cast<gd::PlayLayer*>(self);
    ModManager* manager = ModManager::getInstance();

    // Apply noclip if enabled - we'll handle this in PlayerObject hook
    (void)manager; // unused here
    (void)playLayer;

    return true;
}

void (__thiscall* g_PlayLayer_update)(CCLayer* self, float dt);
void __fastcall PlayLayer_update_H(CCLayer* self, void*, float dt) {
    g_PlayLayer_update(self, dt);

    auto playLayer = static_cast<gd::PlayLayer*>(self);
    ModManager* manager = ModManager::getInstance();

    // Auto clicker - use a static variable to track state
    if (manager->getSettingBool("autoClicker")) {
        static float clickTimer = 0.0f;
        float speed = manager->getSettingFloat("autoClickerSpeed");
        clickTimer += dt * speed;

        if (clickTimer >= 1.0f) {
            clickTimer = 0.0f;
            // Simulate click - call the jump function on player
            // We need to find the player object through the playlayer
            // For now, we'll hook PlayerObject::pushButton instead
        }
    }
}

// PlayerObject hook for noclip
bool (__thiscall* g_PlayerObject_checkCollision)(void* self, void* obj, int type);
bool __fastcall PlayerObject_checkCollision_H(void* self, void*, void* obj, int type) {
    ModManager* manager = ModManager::getInstance();

    if (manager->getSettingBool("noclip")) {
        // Skip collision check when noclip is enabled
        return false;
    }

    return g_PlayerObject_checkCollision(self, obj, type);
}

// PlayerObject pushButton hook for auto-clicker
void (__thiscall* g_PlayerObject_pushButton)(void* self, int btn);
void __fastcall PlayerObject_pushButton_H(void* self, void*, int btn) {
    ModManager* manager = ModManager::getInstance();

    if (manager->getSettingBool("autoClicker") && btn == 0) { // 0 = jump button
        static float clickTimer = 0.0f;
        // We can't easily get dt here, so use a different approach
        // Just call original for now
    }

    g_PlayerObject_pushButton(self, btn);
}

// PauseLayer hook to add mod menu button there too
void (__thiscall* g_PauseLayer_init)(CCLayer* self);
void __fastcall PauseLayer_init_H(CCLayer* self, void*) {
    g_PauseLayer_init(self);

    auto pauseLayer = static_cast<gd::PauseLayer*>(self);
    auto winSize = CCDirector::sharedDirector()->getWinSize();

    // Add mod menu button to pause menu
    auto btnSprite = gd::ButtonSprite::create("Mod Menu", 120, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f);
    auto modMenuBtn = gd::CCMenuItemSpriteExtra::create(
        btnSprite,
        pauseLayer,
        menu_selector(ModMenuLayer::onModMenuButton)
    );

    // Add to pause layer's menu
    CCArray* children = pauseLayer->getChildren();
    CCObject* obj;
    CCARRAY_FOREACH(children, obj) {
        auto node = dynamic_cast<CCNode*>(obj);
        if (node && dynamic_cast<CCMenu*>(node)) {
            auto menu = static_cast<CCMenu*>(node);
            menu->addChild(modMenuBtn);
            modMenuBtn->setPosition({winSize.width / 2, -150});
            break;
        }
    }
}

// Hook for showing hitboxes (debug draw)
void (__thiscall* g_PlayLayer_draw)(CCLayer* self);
void __fastcall PlayLayer_draw_H(CCLayer* self, void*) {
    g_PlayLayer_draw(self);

    ModManager* manager = ModManager::getInstance();
    if (manager->getSettingBool("showHitboxes")) {
        // Draw hitboxes - would need debug draw access
    }
}

// Thread function for hook initialization
DWORD WINAPI thread_func(void* hModule) {
    MH_Initialize();

    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

    // Hook MenuLayer::init
    MH_CreateHook(
        reinterpret_cast<void*>(base + OFF_MenuLayer_init),
        reinterpret_cast<void*>(&MenuLayer_init_H),
        reinterpret_cast<void**>(&g_MenuLayer_init)
    );

    // Hook PlayLayer::init
    MH_CreateHook(
        reinterpret_cast<void*>(base + OFF_PlayLayer_init),
        reinterpret_cast<void*>(&PlayLayer_init_H),
        reinterpret_cast<void**>(&g_PlayLayer_init)
    );

    // Hook PlayLayer::update
    MH_CreateHook(
        reinterpret_cast<void*>(base + OFF_PlayLayer_update),
        reinterpret_cast<void*>(&PlayLayer_update_H),
        reinterpret_cast<void**>(&g_PlayLayer_update)
    );

    // Hook PlayerObject::checkCollision
    MH_CreateHook(
        reinterpret_cast<void*>(base + OFF_PlayerObject_checkCollision),
        reinterpret_cast<void*>(&PlayerObject_checkCollision_H),
        reinterpret_cast<void**>(&g_PlayerObject_checkCollision)
    );

    // Hook PlayerObject::pushButton for auto-clicker
    MH_CreateHook(
        reinterpret_cast<void*>(base + OFF_PlayerObject_pushButton),
        reinterpret_cast<void*>(&PlayerObject_pushButton_H),
        reinterpret_cast<void**>(&g_PlayerObject_pushButton)
    );

    // Hook PauseLayer::init
    MH_CreateHook(
        reinterpret_cast<void*>(base + OFF_PauseLayer_init),
        reinterpret_cast<void*>(&PauseLayer_init_H),
        reinterpret_cast<void**>(&g_PauseLayer_init)
    );

    // Hook PlayLayer::draw
    MH_CreateHook(
        reinterpret_cast<void*>(base + OFF_PlayLayer_draw),
        reinterpret_cast<void*>(&PlayLayer_draw_H),
        reinterpret_cast<void**>(&g_PlayLayer_draw)
    );

    MH_EnableHook(MH_ALL_HOOKS);

    return 0;
}

BOOL APIENTRY DllMain(HMODULE handle, DWORD reason, LPVOID reserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        auto h = CreateThread(0, 0, thread_func, handle, 0, 0);
        if (h)
            CloseHandle(h);
        else
            return FALSE;
    }
    return TRUE;
}