#include "includes.h"
#include "ModManager.hpp"
#include "ModMenuLayer.hpp"
#include "Offsets.hpp"

using namespace modmenu;
using namespace offsets;

// Hook function pointers
bool (__thiscall* MenuLayer_init)(CCLayer* self);
bool __fastcall MenuLayer_init_H(CCLayer* self, void*) {
    if (!MenuLayer_init(self)) return false;

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
bool (__thiscall* PlayLayer_init)(CCLayer* self, void* level);
bool __fastcall PlayLayer_init_H(CCLayer* self, void*, void* level) {
    if (!PlayLayer_init(self, level)) return false;

    auto playLayer = static_cast<gd::PlayLayer*>(self);
    ModManager* manager = ModManager::getInstance();

    // Apply noclip if enabled - set cheat flag
    if (manager->getSettingBool("noclip")) {
        // This enables practice mode style invincibility
        playLayer->m_bHasCheated = true;
    }

    return true;
}

void (__thiscall* PlayLayer_update)(CCLayer* self, float dt);
void __fastcall PlayLayer_update_H(CCLayer* self, void*, float dt) {
    PlayLayer_update(self, dt);

    auto playLayer = static_cast<gd::PlayLayer*>(self);
    ModManager* manager = ModManager::getInstance();

    // Auto clicker
    if (manager->getSettingBool("autoClicker") && !playLayer->m_bIsPaused) {
        static float clickTimer = 0.0f;
        float speed = manager->getSettingFloat("autoClickerSpeed");
        clickTimer += dt * speed;

        if (clickTimer >= 1.0f) {
            clickTimer = 0.0f;
            // Simulate click - call the jump function
            if (playLayer->m_pPlayer1) {
                playLayer->m_pPlayer1->playerJump();
            }
        }
    }
}

// PlayerObject hook for noclip
bool (__thiscall* PlayerObject_checkCollision)(void* self, void* obj, int type);
bool __fastcall PlayerObject_checkCollision_H(void* self, void*, void* obj, int type) {
    ModManager* manager = ModManager::getInstance();

    if (manager->getSettingBool("noclip")) {
        // Skip collision check when noclip is enabled
        return false;
    }

    return PlayerObject_checkCollision(self, obj, type);
}

// PauseLayer hook to add mod menu button there too
void (__thiscall* PauseLayer_init)(CCLayer* self);
void __fastcall PauseLayer_init_H(CCLayer* self, void*) {
    PauseLayer_init(self);

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
void (__thiscall* PlayLayer_draw)(CCLayer* self);
void __fastcall PlayLayer_draw_H(CCLayer* self, void*) {
    PlayLayer_draw(self);

    ModManager* manager = ModManager::getInstance();
    if (manager->getSettingBool("showHitboxes")) {
        auto playLayer = static_cast<gd::PlayLayer*>(self);
        // Draw hitboxes for all objects
        // This would require access to the game's debug draw functionality
        // For now, we'll just note the setting is active
    }
}

// Thread function for hook initialization
DWORD WINAPI thread_func(void* hModule) {
    MH_Initialize();

    auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));

    // Hook MenuLayer::init
    MH_CreateHook(
        reinterpret_cast<void*>(base + MenuLayer_init),
        reinterpret_cast<void*>(&MenuLayer_init_H),
        reinterpret_cast<void**>(&MenuLayer_init)
    );

    // Hook PlayLayer::init
    MH_CreateHook(
        reinterpret_cast<void*>(base + PlayLayer_init),
        reinterpret_cast<void*>(&PlayLayer_init_H),
        reinterpret_cast<void**>(&PlayLayer_init)
    );

    // Hook PlayLayer::update
    MH_CreateHook(
        reinterpret_cast<void*>(base + PlayLayer_update),
        reinterpret_cast<void*>(&PlayLayer_update_H),
        reinterpret_cast<void**>(&PlayLayer_update)
    );

    // Hook PlayerObject::checkCollision
    MH_CreateHook(
        reinterpret_cast<void*>(base + PlayerObject_checkCollision),
        reinterpret_cast<void*>(&PlayerObject_checkCollision_H),
        reinterpret_cast<void**>(&PlayerObject_checkCollision)
    );

    // Hook PauseLayer::init
    MH_CreateHook(
        reinterpret_cast<void*>(base + PauseLayer_init),
        reinterpret_cast<void*>(&PauseLayer_init_H),
        reinterpret_cast<void**>(&PauseLayer_init)
    );

    // Hook PlayLayer::draw
    MH_CreateHook(
        reinterpret_cast<void*>(base + PlayLayer_draw),
        reinterpret_cast<void*>(&PlayLayer_draw_H),
        reinterpret_cast<void**>(&PlayLayer_draw)
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