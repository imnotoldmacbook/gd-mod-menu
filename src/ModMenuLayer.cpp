#include "ModMenuLayer.hpp"

namespace modmenu {

    ModMenuLayer* ModMenuLayer::create() {
        auto ret = new ModMenuLayer();
        if (ret && ret->init()) {
            ret->autorelease();
        } else {
            delete ret;
            ret = nullptr;
        }
        return ret;
    }

    bool ModMenuLayer::init() {
        if (!CCLayer::init()) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        createBackground();
        createTitle();
        createTabs();
        createTabContent(m_currentTab);

        // Close button
        auto closeBtn = gd::CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_closeBtn_001.png"),
            this,
            menu_selector(ModMenuLayer::onClose)
        );
        auto closeMenu = CCMenu::create();
        closeMenu->addChild(closeBtn);
        closeMenu->setPosition({winSize.width - 25, winSize.height - 25});
        addChild(closeMenu, 10);

        setKeypadEnabled(true);
        setTouchEnabled(true);

        return true;
    }

    void ModMenuLayer::createBackground() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        m_pBackgroundSprite = CCSprite::create("GJ_gradientBG.png");
        auto bgSize = m_pBackgroundSprite->getContentSize();

        m_pBackgroundSprite->setScaleX(winSize.width / bgSize.width);
        m_pBackgroundSprite->setScaleY(winSize.height / bgSize.height);
        m_pBackgroundSprite->setAnchorPoint({0, 0});
        m_pBackgroundSprite->setColor({20, 20, 40});
        m_pBackgroundSprite->setOpacity(230);
        m_pBackgroundSprite->setZOrder(-1);

        addChild(m_pBackgroundSprite);

        // Add a semi-transparent overlay panel
        auto panel = CCSprite::create("GJ_square01.png");
        panel->setScale(3.5f);
        panel->setColor({0, 0, 0});
        panel->setOpacity(200);
        panel->setPosition(winSize / 2);
        panel->setZOrder(0);
        addChild(panel);
    }

    void ModMenuLayer::createTitle() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        m_pTitleLabel = CCLabelBMFont::create("Mod Menu v1.0", "bigFont.fnt");
        m_pTitleLabel->setPosition({winSize.width / 2, winSize.height - 40});
        m_pTitleLabel->setScale(0.8f);
        addChild(m_pTitleLabel, 1);
    }

    void ModMenuLayer::createTabs() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        m_pButtonMenu = CCMenu::create();
        m_pButtonMenu->setPosition({0, 0});

        const char* tabNames[] = {"Main", "Gameplay", "TASbot", "Visual", "Settings"};
        const float tabWidth = 80.0f;
        const float startX = (winSize.width - tabWidth * 5) / 2 + tabWidth / 2;

        for (int i = 0; i < 5; i++) {
            auto btnSprite = CCSprite::createWithSpriteFrameName(i == m_currentTab ? "GJ_button_01.png" : "GJ_button_04.png");
            btnSprite->setScale(0.7f);

            auto label = CCLabelBMFont::create(tabNames[i], "goldFont.fnt");
            label->setPosition(btnSprite->getContentSize() / 2);
            label->setScale(0.5f);
            btnSprite->addChild(label);

            auto btn = gd::CCMenuItemSpriteExtra::create(
                btnSprite,
                this,
                menu_selector(ModMenuLayer::onTabButton)
            );
            btn->setTag(i);
            btn->setPosition({startX + i * tabWidth, winSize.height - 85});

            m_pButtonMenu->addChild(btn);
            m_pTabButtons[i] = btn;
        }

        addChild(m_pButtonMenu, 1);
    }

    void ModMenuLayer::createTabContent(int tab) {
        clearTabContent();

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        m_pToggleMenu = CCMenu::create();
        m_pToggleMenu->setPosition({0, 0});

        ModManager* manager = ModManager::getInstance();

        float startY = winSize.height - 140;
        float spacing = 50.0f;

        switch (tab) {
            case TAB_MAIN: {
                addToggleItem(m_pToggleMenu, "Noclip", "noclip", startY);
                addToggleItem(m_pToggleMenu, "Show Hitboxes", "showHitboxes", startY - spacing);
                addToggleItem(m_pToggleMenu, "Auto Clicker", "autoClicker", startY - spacing * 2);
                addSliderItem(m_pToggleMenu, "Click Speed", "autoClickerSpeed", 1.0f, 60.0f, startY - spacing * 3);
                addToggleItem(m_pToggleMenu, "Practice Mode Helper", "practiceMode", startY - spacing * 4);
                addToggleItem(m_pToggleMenu, "Show FPS", "showFPS", startY - spacing * 5);
                break;
            }

            case TAB_GAMEPLAY: {
                auto label = CCLabelBMFont::create("Gameplay Modifications", "bigFont.fnt");
                label->setPosition({winSize.width / 2, startY + 30});
                label->setScale(0.6f);
                addChild(label, 1);

                addToggleItem(m_pToggleMenu, "Auto Practice Mode", "practiceMode", startY);
                addToggleItem(m_pToggleMenu, "Disable Death Effect", "noclip", startY - spacing);
                addToggleItem(m_pToggleMenu, "Show Object Info", "showHitboxes", startY - spacing * 2);

                auto btnSprite = gd::ButtonSprite::create("Test Noclip", 120, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f);
                auto testBtn = gd::CCMenuItemSpriteExtra::create(btnSprite, this, menu_selector(ModMenuLayer::onTASbotSync));
                testBtn->setPosition({winSize.width / 2, startY - spacing * 3});
                m_pToggleMenu->addChild(testBtn);
                break;
            }

            case TAB_TASBOT: {
                auto label = CCLabelBMFont::create("TASbot Integration", "bigFont.fnt");
                label->setPosition({winSize.width / 2, startY + 30});
                label->setScale(0.6f);
                addChild(label, 1);

                // TASbot status
                char statusBuf[64];
                if (manager->isTASbotLoaded()) {
                    strcpy(statusBuf, "TASbot: <cg>Loaded</c>");
                } else {
                    strcpy(statusBuf, "TASbot: <cr>Not Loaded</c>");
                }
                auto statusLabel = CCLabelBMFont::create(statusBuf, "chatFont.fnt");
                statusLabel->setPosition({winSize.width / 2, startY});
                statusLabel->setScale(0.7f);
                addChild(statusLabel, 1);

                addToggleItem(m_pToggleMenu, "TASbot Compatible Mode", "tasbotCompatible", startY - spacing);
                addSliderItem(m_pToggleMenu, "Sync Mode", "tasbotSyncMode", 0.0f, 2.0f, startY - spacing * 2);

                auto reloadBtn = gd::CCMenuItemSpriteExtra::create(
                    gd::ButtonSprite::create("Reload TASbot", 130, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f),
                    this,
                    menu_selector(ModMenuLayer::onReloadMods)
                );
                reloadBtn->setPosition({winSize.width / 2 - 80, startY - spacing * 3.5f});
                m_pToggleMenu->addChild(reloadBtn);

                auto saveBtn = gd::CCMenuItemSpriteExtra::create(
                    gd::ButtonSprite::create("Save TASbot Config", 150, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f),
                    this,
                    menu_selector(ModMenuLayer::onSaveConfig)
                );
                saveBtn->setPosition({winSize.width / 2 + 80, startY - spacing * 3.5f});
                m_pToggleMenu->addChild(saveBtn);

                auto loadBtn = gd::CCMenuItemSpriteExtra::create(
                    gd::ButtonSprite::create("Load TASbot Config", 150, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f),
                    this,
                    menu_selector(ModMenuLayer::onLoadConfig)
                );
                loadBtn->setPosition({winSize.width / 2, startY - spacing * 4.5f});
                m_pToggleMenu->addChild(loadBtn);

                auto syncLabel = CCLabelBMFont::create("Sync Mode: 0=None 1=Frame 2=Click", "chatFont.fnt");
                syncLabel->setPosition({winSize.width / 2, startY - spacing * 5.5f});
                syncLabel->setScale(0.6f);
                addChild(syncLabel, 1);
                break;
            }

            case TAB_VISUAL: {
                auto label = CCLabelBMFont::create("Visual Settings", "bigFont.fnt");
                label->setPosition({winSize.width / 2, startY + 30});
                label->setScale(0.6f);
                addChild(label, 1);

                addToggleItem(m_pToggleMenu, "Show Hitboxes", "showHitboxes", startY);
                addToggleItem(m_pToggleMenu, "Show Object IDs", "noclip", startY - spacing);
                addToggleItem(m_pToggleMenu, "Wireframe Mode", "practiceMode", startY - spacing * 2);
                addToggleItem(m_pToggleMenu, "Show FPS Counter", "showFPS", startY - spacing * 3);
                break;
            }

            case TAB_SETTINGS: {
                auto label = CCLabelBMFont::create("Mod Menu Settings", "bigFont.fnt");
                label->setPosition({winSize.width / 2, startY + 30});
                label->setScale(0.6f);
                addChild(label, 1);

                auto saveBtn = gd::CCMenuItemSpriteExtra::create(
                    gd::ButtonSprite::create("Save Settings", 130, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f),
                    this,
                    menu_selector(ModMenuLayer::onSaveConfig)
                );
                saveBtn->setPosition({winSize.width / 2 - 80, startY});
                m_pToggleMenu->addChild(saveBtn);

                auto loadBtn = gd::CCMenuItemSpriteExtra::create(
                    gd::ButtonSprite::create("Load Settings", 130, true, "bigFont.fnt", "GJ_button_01.png", 30, 0.7f),
                    this,
                    menu_selector(ModMenuLayer::onLoadConfig)
                );
                loadBtn->setPosition({winSize.width / 2 + 80, startY});
                m_pToggleMenu->addChild(loadBtn);

                auto resetBtn = gd::CCMenuItemSpriteExtra::create(
                    gd::ButtonSprite::create("Reset to Defaults", 150, true, "bigFont.fnt", "GJ_button_02.png", 30, 0.7f),
                    this,
                    menu_selector(ModMenuLayer::onReloadMods)
                );
                resetBtn->setPosition({winSize.width / 2, startY - spacing});
                m_pToggleMenu->addChild(resetBtn);

                auto infoLabel = CCLabelBMFont::create("Settings saved to modmenu_settings.json\nin Geometry Dash folder", "chatFont.fnt");
                infoLabel->setPosition({winSize.width / 2, startY - spacing * 2.5f});
                infoLabel->setScale(0.6f);
                addChild(infoLabel, 1);
                break;
            }
        }

        addChild(m_pToggleMenu, 1);
        updateToggleStates();
        updateSliderStates();
    }

    void ModMenuLayer::clearTabContent() {
        if (m_pToggleMenu) {
            m_pToggleMenu->removeFromParentAndCleanup(true);
            m_pToggleMenu = nullptr;
        }
        // Remove slider value labels (tagged 1000+)
        CCArray* children = getChildren();
        CCObject* obj;
        CCARRAY_FOREACH(children, obj) {
            auto node = dynamic_cast<CCNode*>(obj);
            if (node && node->getTag() >= 1000) {
                node->removeFromParentAndCleanup(true);
            }
        }
    }

    void ModMenuLayer::switchTab(int tab) {
        if (tab < 0 || tab >= 5) return;
        m_currentTab = tab;

        // Update tab button sprites
        for (int i = 0; i < 5; i++) {
            if (m_pTabButtons[i]) {
                auto sprite = dynamic_cast<CCSprite*>(m_pTabButtons[i]->getNormalImage());
                if (sprite) {
                    sprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(
                        i == tab ? "GJ_button_01.png" : "GJ_button_04.png"
                    ));
                }
            }
        }

        createTabContent(tab);
    }

    void ModMenuLayer::onTabButton(CCObject* sender) {
        auto btn = static_cast<gd::CCMenuItemSpriteExtra*>(sender);
        switchTab(btn->getTag());
    }

    void ModMenuLayer::onToggle(CCObject* sender) {
        auto toggler = static_cast<gd::CCMenuItemToggler*>(sender);
        const char* key = reinterpret_cast<const char*>(toggler->getUserData());
        if (key) {
            ModManager::getInstance()->setSetting(key, toggler->isOn());
        }
    }

    void ModMenuLayer::onSliderChanged(CCObject* sender) {
        auto btn = static_cast<gd::CCMenuItemSpriteExtra*>(sender);
        const char* key = reinterpret_cast<const char*>(btn->getUserData());
        int delta = btn->getTag(); // -1 for minus, 1 for plus

        if (key) {
            float currentValue = ModManager::getInstance()->getSettingFloat(key);
            float newValue = currentValue + delta * 0.5f;

            if (strcmp(key, "autoClickerSpeed") == 0) {
                if (newValue < 1.0f) newValue = 1.0f;
                if (newValue > 60.0f) newValue = 60.0f;
            } else if (strcmp(key, "tasbotSyncMode") == 0) {
                if (newValue < 0.0f) newValue = 0.0f;
                if (newValue > 2.0f) newValue = 2.0f;
            }

            ModManager::getInstance()->setSetting(key, newValue);

            // Update the value label
            char valueStr[32];
            sprintf(valueStr, "%.1f", newValue);

            CCArray* children = getChildren();
            CCObject* obj;
            CCARRAY_FOREACH(children, obj) {
                auto node = dynamic_cast<CCNode*>(obj);
                if (node && node->getTag() == 1000 + reinterpret_cast<int>(key)) {
                    auto label = dynamic_cast<CCLabelBMFont*>(node);
                    if (label) {
                        label->setString(valueStr);
                    }
                    break;
                }
            }
        }
    }

    void ModMenuLayer::onClose(CCObject* sender) {
        CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
    }

    void ModMenuLayer::onTASbotSync(CCObject* sender) {
        ModManager::getInstance()->callTASbotInitHook();
    }

    void ModMenuLayer::onReloadMods(CCObject* sender) {
        ModManager* manager = ModManager::getInstance();
        ModManager::Settings defaults;
        manager->getSettings() = defaults;
        manager->save();
        updateToggleStates();
        updateSliderStates();
        createTabContent(m_currentTab);
    }

    void ModMenuLayer::onSaveConfig(CCObject* sender) {
        ModManager::getInstance()->save();
        if (ModManager::getInstance()->isTASbotLoaded()) {
            ModManager::getInstance()->saveTASbotConfig();
        }

        // Show confirmation using FLAlertLayer from gd namespace
        auto alert = gd::FLAlertLayer::create(nullptr, "Saved", "OK", nullptr, "Configuration saved successfully!");
        alert->show();
    }

    void ModMenuLayer::onLoadConfig(CCObject* sender) {
        ModManager::getInstance()->load();
        if (ModManager::getInstance()->isTASbotLoaded()) {
            ModManager::getInstance()->loadTASbotConfig();
        }
        updateToggleStates();
        updateSliderStates();
        createTabContent(m_currentTab);

        auto alert = gd::FLAlertLayer::create(nullptr, "Loaded", "OK", nullptr, "Configuration loaded successfully!");
        alert->show();
    }

    void ModMenuLayer::addToggleItem(CCMenu* menu, const char* label, const char* key, float yPos) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Label
        auto lbl = CCLabelBMFont::create(label, "goldFont.fnt");
        lbl->setAnchorPoint({1, 0.5f});
        lbl->setPosition({winSize.width / 2 - 100, yPos});
        lbl->setScale(0.6f);
        addChild(lbl, 1);

        // Toggle
        auto offSprite = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto onSprite = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");
        offSprite->setScale(0.8f);
        onSprite->setScale(0.8f);

        auto toggler = gd::CCMenuItemToggler::create(
            offSprite,
            onSprite,
            this,
            menu_selector(ModMenuLayer::onToggle)
        );
        toggler->setUserData(const_cast<char*>(key));
        toggler->setPosition({winSize.width / 2 + 80, yPos});

        menu->addChild(toggler);

        ToggleItem item;
        item.toggler = toggler;
        item.settingKey = key;
        item.label = label;
        m_toggleItems.push_back(item);
    }

    void ModMenuLayer::addSliderItem(CCMenu* menu, const char* label, const char* key, float min, float max, float yPos) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Label
        auto lbl = CCLabelBMFont::create(label, "goldFont.fnt");
        lbl->setAnchorPoint({1, 0.5f});
        lbl->setPosition({winSize.width / 2 - 100, yPos});
        lbl->setScale(0.6f);
        addChild(lbl, 1);

        // Value label
        auto value = ModManager::getInstance()->getSettingFloat(key);
        char valueStr[32];
        sprintf(valueStr, "%.1f", value);

        auto valueLabel = CCLabelBMFont::create(valueStr, "goldFont.fnt");
        valueLabel->setPosition({winSize.width / 2 + 80, yPos + 25});
        valueLabel->setScale(0.5f);
        valueLabel->setTag(1000 + reinterpret_cast<int>(key));
        addChild(valueLabel, 1);

        // Minus button
        auto minusBtn = gd::CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_minusBtn_001.png"),
            this,
            menu_selector(ModMenuLayer::onSliderChanged)
        );
        minusBtn->setUserData(const_cast<char*>(key));
        minusBtn->setTag(-1);
        minusBtn->setPosition({winSize.width / 2 - 20, yPos});
        minusBtn->setScale(0.7f);
        menu->addChild(minusBtn);

        // Plus button
        auto plusBtn = gd::CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_plusBtn_001.png"),
            this,
            menu_selector(ModMenuLayer::onSliderChanged)
        );
        plusBtn->setUserData(const_cast<char*>(key));
        plusBtn->setTag(1);
        plusBtn->setPosition({winSize.width / 2 + 60, yPos});
        plusBtn->setScale(0.7f);
        menu->addChild(plusBtn);

        SliderItem item;
        item.settingKey = key;
        item.label = label;
        item.minVal = min;
        item.maxVal = max;
        m_sliderItems.push_back(item);
    }

    void ModMenuLayer::updateToggleStates() {
        for (auto& item : m_toggleItems) {
            if (item.toggler) {
                bool value = ModManager::getInstance()->getSettingBool(item.settingKey);
                item.toggler->toggle(value);
            }
        }
    }

    void ModMenuLayer::updateSliderStates() {
        for (auto& item : m_sliderItems) {
            float value = ModManager::getInstance()->getSettingFloat(item.settingKey);
            char valueStr[32];
            sprintf(valueStr, "%.1f", value);

            CCArray* children = getChildren();
            CCObject* obj;
            CCARRAY_FOREACH(children, obj) {
                auto node = dynamic_cast<CCNode*>(obj);
                if (node && node->getTag() == 1000 + reinterpret_cast<int>(item.settingKey)) {
                    auto label = dynamic_cast<CCLabelBMFont*>(node);
                    if (label) {
                        label->setString(valueStr);
                    }
                    break;
                }
            }
        }
    }

    void ModMenuLayer::keyBackClicked() {
        onClose(nullptr);
    }

    void ModMenuLayer::onModMenuButton(CCObject* sender) {
        auto scene = CCScene::create();
        auto layer = ModMenuLayer::create();
        scene->addChild(layer);
        auto transition = CCTransitionFade::create(0.5f, scene);
        CCDirector::sharedDirector()->pushScene(transition);
    }

} // namespace modmenu