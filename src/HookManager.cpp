#include "HookManager.hpp"
#include "ModManager.hpp"
#include "Offsets.hpp"

namespace modmenu {

    HookManager* HookManager::s_instance = nullptr;

    void HookManager::enableHooks() {
        if (m_hooksEnabled) return;

        // All hooks are enabled in dllmain thread_func
        // This function can be used for runtime hook toggling
        m_hooksEnabled = true;
    }

    void HookManager::disableHooks() {
        if (!m_hooksEnabled) return;

        MH_DisableHook(MH_ALL_HOOKS);
        m_hooksEnabled = false;
    }

    void HookManager::setNoclipHook(bool enabled) {
        auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
        if (enabled) {
            MH_EnableHook(reinterpret_cast<void*>(base + OFF_PlayerObject_checkCollision));
        } else {
            MH_DisableHook(reinterpret_cast<void*>(base + OFF_PlayerObject_checkCollision));
        }
    }

    void HookManager::setHitboxHook(bool enabled) {
        auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
        if (enabled) {
            MH_EnableHook(reinterpret_cast<void*>(base + OFF_PlayLayer_draw));
        } else {
            MH_DisableHook(reinterpret_cast<void*>(base + OFF_PlayLayer_draw));
        }
    }

    void HookManager::setAutoClickerHook(bool enabled) {
        auto base = reinterpret_cast<uintptr_t>(GetModuleHandle(0));
        if (enabled) {
            MH_EnableHook(reinterpret_cast<void*>(base + OFF_PlayLayer_update));
        } else {
            MH_DisableHook(reinterpret_cast<void*>(base + OFF_PlayLayer_update));
        }
    }

} // namespace modmenu