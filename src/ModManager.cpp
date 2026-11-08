#include "ModManager.hpp"

namespace modmenu {

    ModManager* ModManager::s_instance = nullptr;

    ModManager::ModManager() {
        // Get config path
        char buffer[MAX_PATH];
        GetModuleFileNameA(GetModuleHandleA("GeometryDash.exe"), buffer, MAX_PATH);
        m_configPath = std::filesystem::path(buffer).parent_path() / "modmenu_settings.json";

        loadSettings();
        initTASbot();
    }

    ModManager* ModManager::getInstance() {
        if (!s_instance) {
            s_instance = new ModManager();
        }
        return s_instance;
    }

    bool ModManager::initTASbot() {
        // Try to find TASbot module
        m_tasbotModule = GetModuleHandleA("tBot.dll");
        if (!m_tasbotModule) {
            m_tasbotModule = GetModuleHandleA("TASbot.dll");
        }
        if (!m_tasbotModule) {
            // Try to load it
            m_tasbotModule = LoadLibraryA("tBot.dll");
        }
        if (!m_tasbotModule) {
            m_tasbotModule = LoadLibraryA("TASbot.dll");
        }

        if (m_tasbotModule) {
            // Get function pointers using decorated names from strings output
            m_tasbotInitHook = reinterpret_cast<TASbot_InitHook>(
                GetProcAddress(m_tasbotModule, "?init_hook@bot@@YAXXZ")
            );
            m_tasbotLoadClickConfig = reinterpret_cast<TASbot_LoadClickConfig>(
                GetProcAddress(m_tasbotModule, "?load_click_config@bot@@YA_NXZ")
            );
            m_tasbotSaveClickConfig = reinterpret_cast<TASbot_SaveClickConfig>(
                GetProcAddress(m_tasbotModule, "?save_click_config@bot@@YA_NXZ")
            );
            return true;
        }
        return false;
    }

    void ModManager::callTASbotInitHook() {
        if (m_tasbotInitHook) {
            m_tasbotInitHook();
        }
    }

    bool ModManager::loadTASbotConfig() {
        if (m_tasbotLoadClickConfig) {
            return m_tasbotLoadClickConfig();
        }
        return false;
    }

    bool ModManager::saveTASbotConfig() {
        if (m_tasbotSaveClickConfig) {
            return m_tasbotSaveClickConfig();
        }
        return false;
    }

    void ModManager::loadSettings() {
        // Default values are already set in struct initialization
        if (!std::filesystem::exists(m_configPath)) {
            return;
        }

        try {
            std::ifstream file(m_configPath);
            if (!file.is_open()) return;

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty() || line[0] == '#') continue;

                size_t eqPos = line.find('=');
                if (eqPos == std::string::npos) continue;

                std::string key = line.substr(0, eqPos);
                std::string value = line.substr(eqPos + 1);

                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                value.erase(0, value.find_first_not_of(" \t"));
                value.erase(value.find_last_not_of(" \t") + 1);

                if (key == "noclip") m_settings.noclip = (value == "1" || value == "true");
                else if (key == "showHitboxes") m_settings.showHitboxes = (value == "1" || value == "true");
                else if (key == "autoClicker") m_settings.autoClicker = (value == "1" || value == "true");
                else if (key == "autoClickerSpeed") m_settings.autoClickerSpeed = std::stof(value);
                else if (key == "practiceMode") m_settings.practiceMode = (value == "1" || value == "true");
                else if (key == "showFPS") m_settings.showFPS = (value == "1" || value == "true");
                else if (key == "tasbotCompatible") m_settings.tasbotCompatible = (value == "1" || value == "true");
                else if (key == "tasbotSyncMode") m_settings.tasbotSyncMode = std::stoi(value);
            }
        } catch (...) {
            // Ignore parse errors, use defaults
        }
    }

    void ModManager::saveSettings() {
        try {
            std::ofstream file(m_configPath);
            if (!file.is_open()) return;

            file << "# Mod Menu Settings\n";
            file << "noclip=" << (m_settings.noclip ? "1" : "0") << "\n";
            file << "showHitboxes=" << (m_settings.showHitboxes ? "1" : "0") << "\n";
            file << "autoClicker=" << (m_settings.autoClicker ? "1" : "0") << "\n";
            file << "autoClickerSpeed=" << m_settings.autoClickerSpeed << "\n";
            file << "practiceMode=" << (m_settings.practiceMode ? "1" : "0") << "\n";
            file << "showFPS=" << (m_settings.showFPS ? "1" : "0") << "\n";
            file << "tasbotCompatible=" << (m_settings.tasbotCompatible ? "1" : "0") << "\n";
            file << "tasbotSyncMode=" << m_settings.tasbotSyncMode << "\n";
        } catch (...) {
            // Ignore write errors
        }
    }

    void ModManager::toggleSetting(const char* key) {
        if (strcmp(key, "noclip") == 0) m_settings.noclip = !m_settings.noclip;
        else if (strcmp(key, "showHitboxes") == 0) m_settings.showHitboxes = !m_settings.showHitboxes;
        else if (strcmp(key, "autoClicker") == 0) m_settings.autoClicker = !m_settings.autoClicker;
        else if (strcmp(key, "practiceMode") == 0) m_settings.practiceMode = !m_settings.practiceMode;
        else if (strcmp(key, "showFPS") == 0) m_settings.showFPS = !m_settings.showFPS;
        else if (strcmp(key, "tasbotCompatible") == 0) m_settings.tasbotCompatible = !m_settings.tasbotCompatible;
        saveSettings();
    }

    void ModManager::setSetting(const char* key, bool value) {
        if (strcmp(key, "noclip") == 0) m_settings.noclip = value;
        else if (strcmp(key, "showHitboxes") == 0) m_settings.showHitboxes = value;
        else if (strcmp(key, "autoClicker") == 0) m_settings.autoClicker = value;
        else if (strcmp(key, "practiceMode") == 0) m_settings.practiceMode = value;
        else if (strcmp(key, "showFPS") == 0) m_settings.showFPS = value;
        else if (strcmp(key, "tasbotCompatible") == 0) m_settings.tasbotCompatible = value;
        saveSettings();
    }

    void ModManager::setSetting(const char* key, float value) {
        if (strcmp(key, "autoClickerSpeed") == 0) m_settings.autoClickerSpeed = value;
        saveSettings();
    }

    void ModManager::setSetting(const char* key, int value) {
        if (strcmp(key, "tasbotSyncMode") == 0) m_settings.tasbotSyncMode = value;
        saveSettings();
    }

    bool ModManager::getSettingBool(const char* key) const {
        if (strcmp(key, "noclip") == 0) return m_settings.noclip;
        if (strcmp(key, "showHitboxes") == 0) return m_settings.showHitboxes;
        if (strcmp(key, "autoClicker") == 0) return m_settings.autoClicker;
        if (strcmp(key, "practiceMode") == 0) return m_settings.practiceMode;
        if (strcmp(key, "showFPS") == 0) return m_settings.showFPS;
        if (strcmp(key, "tasbotCompatible") == 0) return m_settings.tasbotCompatible;
        return false;
    }

    float ModManager::getSettingFloat(const char* key) const {
        if (strcmp(key, "autoClickerSpeed") == 0) return m_settings.autoClickerSpeed;
        return 0.0f;
    }

    int ModManager::getSettingInt(const char* key) const {
        if (strcmp(key, "tasbotSyncMode") == 0) return m_settings.tasbotSyncMode;
        return 0;
    }

    void ModManager::save() {
        saveSettings();
    }

    void ModManager::load() {
        loadSettings();
    }

} // namespace modmenu