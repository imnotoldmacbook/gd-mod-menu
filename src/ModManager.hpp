#pragma once

#include "includes.h"

namespace modmenu {

    // Forward declarations for TASbot functions
    typedef void (*TASbot_InitHook)();
    typedef bool (*TASbot_LoadClickConfig)();
    typedef bool (*TASbot_SaveClickConfig)();

    class ModManager {
    private:
        static ModManager* s_instance;
        HMODULE m_tasbotModule = nullptr;

        // TASbot function pointers
        TASbot_InitHook m_tasbotInitHook = nullptr;
        TASbot_LoadClickConfig m_tasbotLoadClickConfig = nullptr;
        TASbot_SaveClickConfig m_tasbotSaveClickConfig = nullptr;

        public:
        // Mod settings (public for external access)
        struct Settings {
            bool noclip = false;
            bool showHitboxes = false;
            bool autoClicker = false;
            float autoClickerSpeed = 10.0f;
            bool practiceMode = false;
            bool showFPS = true;
            bool tasbotCompatible = true;
            int tasbotSyncMode = 0; // 0 = none, 1 = frame, 2 = click
        };

    private:
        Settings m_settings;

        std::filesystem::path m_configPath;

        ModManager();
        void loadSettings();
        void saveSettings();

    public:
        static ModManager* getInstance();

        // TASbot integration
        bool initTASbot();
        void callTASbotInitHook();
        bool loadTASbotConfig();
        bool saveTASbotConfig();
        bool isTASbotLoaded() const { return m_tasbotModule != nullptr; }

        // Settings access
        Settings& getSettings() { return m_settings; }
        const Settings& getSettings() const { return m_settings; }

        void toggleSetting(const char* key);
        void setSetting(const char* key, bool value);
        void setSetting(const char* key, float value);
        void setSetting(const char* key, int value);
        bool getSettingBool(const char* key) const;
        float getSettingFloat(const char* key) const;
        int getSettingInt(const char* key) const;

        void save();
        void load();
    };

    // Helper macros for settings
    #define MOD_SETTING_BOOL(key) ModManager::getInstance()->getSettingBool(key)
    #define MOD_SETTING_FLOAT(key) ModManager::getInstance()->getSettingFloat(key)
    #define MOD_SETTING_INT(key) ModManager::getInstance()->getSettingInt(key)
    #define MOD_TOGGLE(key) ModManager::getInstance()->toggleSetting(key)
    #define MOD_SET_BOOL(key, val) ModManager::getInstance()->setSetting(key, val)
    #define MOD_SET_FLOAT(key, val) ModManager::getInstance()->setSetting(key, val)
    #define MOD_SET_INT(key, val) ModManager::getInstance()->setSetting(key, val)

} // namespace modmenu