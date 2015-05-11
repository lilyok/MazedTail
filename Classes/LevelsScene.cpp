//
//  LevelsScene.cpp
//  Dusk
//
//  Created by lilil on 08.01.15.
//
//

#include "LevelsScene.h"
#include "Portals.h"
#include "Doors.h"
#include "Rotators.h"
#include "Shifts.h"
#include "Lifts.h"
#include "Holes.h"
#include "Eyes.h"
#include "Hands.h"
#include "Noses.h"
#include "Tongue.h"
#include "Ears.h"
#include "Balance.h"

USING_NS_CC;


LevelsScene::LevelsScene()
{
}

/**
 * Call this method to create the LevelsScene.
 */
Scene* LevelsScene::createScene()
{
    // 'pScene' is an autorelease object
    auto pScene = Scene::create();
    
    // 'pLayer' is an autorelease object
    auto pLayer = LevelsScene::create();
    
    // add the layer as a child to the scene
    pScene->addChild(pLayer);
    return pScene;
}

bool LevelsScene::init()
{
    // Init super first.
    if ( !Layer::init() )
    {
        return false;
    }
    std::vector<std::string> searchPaths;
    searchPaths.push_back("TileMaps");
    searchPaths.push_back("fonts");
    FileUtils::getInstance()->setSearchPaths(searchPaths);
    
    this->visibleSize = Director::getInstance()->getVisibleSize();
    this->origin = Director::getInstance()->getVisibleOrigin();
    
    
    icon_scale = (this->visibleSize.height/16.0) / 64.0;
    if (icon_scale > 1)
        icon_scale = 1.0;
    
    auto back_sprite = Sprite::create("backall.png");
    
    back_sprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
    back_sprite->setTextureRect(Rect(origin.x, origin.y, visibleSize.width, visibleSize.height));
    
    back_sprite->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
    addChild(back_sprite, 0);
    
    prepareMap("");
    
    this->scheduleUpdate();

    return true;
}

void LevelsScene::clearMap() {
    for (auto level: levels)
        if (getChildByName(level->getName())) removeChildByName(level->getName());
    for (auto btn: buttons)
        if (getChildByName(btn->getName())) removeChildByName(btn->getName());
    if (getChildByName("map")) removeChildByName("map");    
    levels.clear();
    buttons.clear();
    map = NULL;
}

void LevelsScene::prepareMap(std::string name) {
    clearMap();

    const char *HIGH_SCORE="l";
    auto high_score = 7;//UserDefault::getInstance()->getIntegerForKey(HIGH_SCORE);
    
    if (name == "") {
        char *res = new char[50];
        cur_menu = 1 + int(high_score / MAX_LEVELS);
        std::sprintf(res, "menu%i.tmx", cur_menu);
        name = res;
    }
    
    this->map = TMXTiledMap::create(name);
    Size s = map->getContentSize();
    

    //////////////////////////////////////////////////
    if (visibleSize.height / s.height < visibleSize.width / s.width)
        this->scale_map = visibleSize.height / s.height;
    else
        this->scale_map = visibleSize.width / s.width;
    xZero = origin.x + visibleSize.width / 2 - s.width * scale_map / 2;
    yZero = origin.y + visibleSize.height / 2 - s.height * scale_map / 2;
    
    map->setScale(scale_map);
    map->setPosition(Vec2(xZero, yZero));
    map->setName("map");
    addChild(map, 0);
    
    TMXObjectGroup *lev = map->getObjectGroup("levels");
    if (lev != nullptr) {
        float x, y, w, h;
        ValueVector objectsPoint = lev->getObjects();
        auto r = -30;
        auto i = 0;
        for (auto objPointMap : objectsPoint) {
            ValueMap objPoint = objPointMap.asValueMap();
            x = objPoint.at("x").asFloat();
            y = objPoint.at("y").asFloat();
            w = objPoint.at("width").asFloat();
            h = objPoint.at("height").asFloat();
            auto name = objPoint.at("name").asString();
            Point _point = Point(xZero + scale_map * (x + w / 2.0f), yZero + scale_map * (y + h / 2.0f));
            Size _size = Size(scale_map * w, scale_map * h);
            
            auto sprite = Sprite::create(name +".png");
            auto sprite_scale_w = _size.width / sprite->getContentSize().width;
            auto sprite_scale_h = _size.width / sprite->getContentSize().width;
            sprite->setScale(sprite_scale_w, sprite_scale_h);
            sprite->setName(name);
            sprite->setPosition(_point);
            sprite->setRotation(r);
            if (i  + MAX_LEVELS * (cur_menu - 1) > high_score)
                sprite->setOpacity(100);
            r += 30;
            if (r > 30) r = -30;
            
            sprite->setName(name);
            addChild(sprite, 1);
            levels.pushBack(sprite);
            i++;
        }
    }
    
    
    TMXObjectGroup *but = map->getObjectGroup("buttons");
    if (but != nullptr) {
        float x, y, w, h;
        ValueVector objectsPoint = but->getObjects();
        for (auto objPointMap : objectsPoint) {
            ValueMap objPoint = objPointMap.asValueMap();
            x = objPoint.at("x").asFloat();
            y = objPoint.at("y").asFloat();
            w = objPoint.at("width").asFloat();
            h = objPoint.at("height").asFloat();
            auto name = objPoint.at("name").asString();
            Point _point = Point(xZero + scale_map * (x + w / 2.0f), yZero + scale_map * (y + h / 2.0f));
            
            Sprite* btnsprite;
            if (name == "start")
                btnsprite = Sprite::create("PlayNormal.png");
            else if (name == "close")
                btnsprite = Sprite::create("CloseNormal.png");
            else if (name == "next")
                btnsprite = Sprite::create("next.png");
            else if (name == "back")
                btnsprite = Sprite::create("back.png");
            
            btnsprite->setName(name);
            btnsprite->setPosition(_point);
            btnsprite->setScale(icon_scale);
            
            btnsprite->setName(name);
            addChild(btnsprite, 1);
            buttons.pushBack(btnsprite);
        }
    }
}

void LevelsScene::finishSplash(float dt){
    getChildByName("black")->runAction(FadeTo::create(2, 0));

    getChildByName("splash")->runAction(FadeTo::create(2, 0));
}

void LevelsScene::onEnter() {
    Layer::onEnter();
    
    // Register Touch Event
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    
    listener->onTouchBegan = CC_CALLBACK_2(LevelsScene::onTouchBegan, this);
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    // add "HelloWorld" splash screen"
    auto menuSprite = Sprite::create("clearblack.png");
    menuSprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
    menuSprite->setTextureRect(Rect(origin.x, origin.y, visibleSize.width, visibleSize.height));
    
    menuSprite->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
    menuSprite->setName("black");
    menuSprite->setOpacity(255);
    addChild(menuSprite, 4);
    
    
    auto sprite = Sprite::create("HelloWorld.png");
    
    // position the sprite on the center of the screen
    
    sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));

    sprite->setName("splash");
    //sprite->setOpacity(0);
    // add the sprite as a child to this layer
    this->addChild(sprite, 5);
    this->scheduleOnce(schedule_selector(LevelsScene::finishSplash),0.0f);
}


bool LevelsScene::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event) {
    auto p = touch->getLocation();
    auto i = 0;
    for (auto level : levels) {
        auto w = level->getContentSize().width * scale_map;
        auto h = level->getContentSize().height * scale_map;
        auto x = level->getPosition().x;
        auto y = level->getPosition().y;
        
        if (level->getOpacity() == 255 && (x - w/2 < p.x) && (x + w/2 > p.x) && (y - h/2 < p.y) && (y + h/2 > p.y)) {
            level->runAction(Sequence::create(ScaleTo::create(0.1, 1.5*scale_map),
                                               TintTo::create(0.2, 255, 200, 255),
                                               TintTo::create(0.2, 255, 255, 255),
                                               ScaleTo::create(0.1, scale_map), NULL));
            level_i = i + MAX_LEVELS * (cur_menu - 1);
        }
        i++;
    }
    
    for (auto button : buttons) {
        auto w = button->getContentSize().width * icon_scale;
        auto h = button->getContentSize().height * icon_scale;
        auto x = button->getPosition().x;
        auto y = button->getPosition().y;
        
        if ((x - w/2 < p.x) && (x + w/2 > p.x) && (y - h/2 < p.y) && (y + h/2 > p.y)) {
            button->runAction(Sequence::create(ScaleTo::create(0.1, 1.5*icon_scale),
                                              TintTo::create(0.2, 255, 200, 255),
                                              TintTo::create(0.2, 255, 255, 255),
                                              ScaleTo::create(0.1, icon_scale), NULL));
            if (button->getName() == "close")
                isClose = true;
            else if (button->getName() == "start")
                isStart = true;
            else if (button->getName() == "next")
                isNext = true;
            else if (button->getName() == "back")
                isBack = true;
        }
    }
    return true;
}

void LevelsScene::update(float delta) {
    Sprite* start;
    Sprite* close;
    Sprite* next;
    Sprite* back;
    char *res = new char[50];
    
    for(auto b : buttons)
        if (b->getName() == "start")
            start = b;
        else if (b->getName() == "close")
            close = b;
        else if (b->getName() == "next")
            next = b;
        else if (b->getName() == "back")
            back = b;
    
    if (isClose && close->getNumberOfRunningActions() <= 0) {
        isClose = false;
        LevelsScene::close();
    } else if (isStart && start->getNumberOfRunningActions() <= 0) {
        isStart = false;
        LevelsScene::start();
    } else if (isNext && next->getNumberOfRunningActions() <= 0) {
        isNext = false;
        cur_menu++;
        if (cur_menu <= MAX_MENU) {
            std::sprintf(res, "menu%i.tmx", cur_menu);
            prepareMap(res);
        } else {
            cur_menu = MAX_MENU;
        }
    } else if (isBack && back->getNumberOfRunningActions() <= 0) {
        isBack = false;
        cur_menu--;
    
        std::sprintf(res, "menu%i.tmx", cur_menu);
        prepareMap(res);
    } else if (level_i > (cur_menu - 1) * MAX_LEVELS - 1 &&
               levels.at(level_i - (cur_menu - 1) * MAX_LEVELS)->getNumberOfRunningActions() <= 0) {
        auto i = level_i - (cur_menu - 1) * MAX_LEVELS;
        level_i = -1;
        changeLevel(levels.at(i)->getName());
    }
    
}

void LevelsScene::changeLevel(std::string name) {
    Scene *newScene;
    if (name == "portals")
        newScene = Portals::createScene();
    else if (name == "doors")
        newScene = Doors::createScene();
    else if (name == "rotators")
        newScene = Rotators::createScene();
    else if (name == "shifts")
        newScene = Shifts::createScene();
    else if (name == "mists")
        newScene = Lifts::createScene();
    else if (name == "holes")
        newScene = Holes::createScene();
    else if (name == "eyes")
        newScene = Eyes::createScene();
    else if (name == "touch")
        newScene = Hands::createScene();
    else if (name == "noses")
        newScene = Noses::createScene();
    else if (name == "tongue")
        newScene = Tongue::createScene();
    else if (name == "ears")
        newScene = Ears::createScene();
    else if (name == "balance")
        newScene = Balance::createScene();
    
    cocos2d::Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, newScene));
}

void LevelsScene::start()
{
    auto size = levels.size()-1;
    for (auto i = size; i >= 0; i--) {
        if (levels.at(i)->getOpacity() == 255) {
            changeLevel(levels.at(i)->getName());
            break;
        }
            
    }
}


void LevelsScene::close()
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