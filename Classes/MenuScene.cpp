//
//  MenuScene.cpp
//  Dusk
//
//  Created by lilil on 08.01.15.
//
//

#include "MenuScene.h"
#include "Portals.h"

USING_NS_CC;

MenuScene::MenuScene()
{
}

/**
 * Call this method to create the MenuScene.
 */
Scene* MenuScene::createScene()
{
    // 'pScene' is an autorelease object
    auto pScene = Scene::create();
    
    // 'pLayer' is an autorelease object
    auto pLayer = MenuScene::create();
    
    // add the layer as a child to the scene
    pScene->addChild(pLayer);
    return pScene;
}

bool MenuScene::init()
{
    // Init super first.
    if ( !Layer::init() )
    {
        return false;
    }
    
    // Ask director the window size.
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // Create start button.
    MenuItemFont* pStartButton = MenuItemFont::create("Start", CC_CALLBACK_1(MenuScene::menuStartCallback, this));
    
    pStartButton->setFontSizeObj(50);
    
    pStartButton->setPosition(Vec2(origin.x+ visibleSize.width/2,
                                   origin.y + visibleSize.height/2 + pStartButton->getContentSize().height/2));
    
    MenuItemFont* pRestartButton = MenuItemFont::create("Restart", CC_CALLBACK_1(MenuScene::menuRestartCallback, this));
    
    pRestartButton->setFontSizeObj(50);
    
    pRestartButton->setPosition(Vec2(origin.x+ visibleSize.width/2,
                                   origin.y + visibleSize.height/2 - pStartButton->getContentSize().height + pRestartButton->getContentSize().height/2));
    
    
    MenuItemFont* pCloseButton = MenuItemFont::create("Close", CC_CALLBACK_1(MenuScene::menuCloseCallback, this));
    
    pCloseButton->setFontSizeObj(50);
    
    pCloseButton->setPosition(Vec2(origin.x + visibleSize.width/2,
                                   origin.y + visibleSize.height/2 - pRestartButton->getContentSize().height - pStartButton->getContentSize().height + pCloseButton->getContentSize().height/2));
    
    // Create menu, it's an autorelease object
    Menu* pMenu = Menu::create(pStartButton, pRestartButton, pCloseButton, NULL);
    pMenu->setPosition(Vec2::ZERO);
    this->addChild(pMenu, 1);
    
    return true;
}

void MenuScene::menuStartCallback(cocos2d::Ref* pSender)
{
    Scene *pScene = Portals::createScene();
    cocos2d::Director::getInstance()->replaceScene(pScene);
}

void MenuScene::menuRestartCallback(cocos2d::Ref* pSender)
{
    Scene *pScene = Portals::createScene();
    cocos2d::Director::getInstance()->replaceScene(pScene);
}


void MenuScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif
    
    Director::getInstance()->end();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}