#pragma once

#include "includes.h"

namespace modmenu {

    class HookManager {
    private:
        static HookManager* s_instance;
        bool m_hooksEnabled = false;

        HookManager() = default;

    public:
        static HookManager* getInstance() {
            if (!s_instance) s_instance = new HookManager();
            return s_instance;
        }

        void enableHooks();
        void disableHooks();
        bool areHooksEnabled() const { return m_hooksEnabled; }

        // Individual hook controls
        void setNoclipHook(bool enabled);
        void setHitboxHook(bool enabled);
        void setAutoClickerHook(bool enabled);
    };

} // namespace modmenu