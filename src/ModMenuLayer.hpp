#pragma once

#include "includes.h"
#include "ModManager.hpp"
// FLAlertLayer is included via gd.h

namespace modmenu {

    class ModMenuLayer : public CCLayer {
    protected:
        CCMenu* m_pButtonMenu;
        CCMenu* m_pToggleMenu;
        CCLabelBMFont* m_pTitleLabel;
        CCSprite* m_pBackgroundSprite;

        // Tabs
        enum TabType {
            TAB_MAIN = 0,
            TAB_GAMEPLAY = 1,
            TAB_TASBOT = 2,
            TAB_VISUAL = 3,
            TAB_SETTINGS = 4,
        };
        int m_currentTab = TAB_MAIN;

        // Menu items for tabs
        gd::CCMenuItemSpriteExtra* m_pTabButtons[5];

        // Toggle buttons
        struct ToggleItem {
            gd::CCMenuItemToggler* toggler;
            const char* settingKey;
            const char* label;
        };
        std::vector<ToggleItem> m_toggleItems;

        // Slider items (using button-based approach since CCControlSlider may not be available)
        struct SliderItem {
            const char* settingKey;
            const char* label;
            float minVal;
            float maxVal;
        };
        std::vector<SliderItem> m_sliderItems;

        virtual bool init();
        virtual void keyBackClicked();

        void createBackground();
        void createTitle();
        void createTabs();
        void createTabContent(int tab);
        void switchTab(int tab);
        void clearTabContent();

        // Callbacks
        void onTabButton(CCObject* sender);
        void onToggle(CCObject* sender);
        void onSliderChanged(CCObject* sender);
        void onClose(CCObject* sender);
        void onTASbotSync(CCObject* sender);
        void onReloadMods(CCObject* sender);
        void onSaveConfig(CCObject* sender);
        void onLoadConfig(CCObject* sender);

        // Helper to create toggle with label
        void addToggleItem(CCMenu* menu, const char* label, const char* key, float yPos);
        void addSliderItem(CCMenu* menu, const char* label, const char* key, float min, float max, float yPos);

        // Update toggle states from settings
        void updateToggleStates();
        void updateSliderStates();

    public:
        static ModMenuLayer* create();

        // Button callback
        void onModMenuButton(CCObject* sender);
    };

} // namespace modmenu