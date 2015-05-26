//
// Created by lilil on 27.02.15.
//

#include "AbstractLabirint.h"
#include "LevelsScene.h"

USING_NS_CC;
const char *HIGH_SCORE="l";

Scene *AbstractLabirint::createScene(std::string map_name, std::string back_name) {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = AbstractLabirint::create(map_name, back_name);
    //layer->setPhyWorld(scene->getPhysicsWorld());
    //layer->setPhyWorld(scene->getPhysicsWorld()); //lilil
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool AbstractLabirint::init(std::string map_name, std::string back_name) {
    //////////////////////////////
    // 1. super init first
    if (!Layer::init()) {
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
    
    
    audio = CocosDenshion::SimpleAudioEngine::getInstance();
    
    audio->playBackgroundMusic("allegro.mp3", true);
    
    menuSprite = Sprite::create("black_pixel.png");
    menuSprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
    menuSprite->setTextureRect(Rect(origin.x, origin.y, visibleSize.width, visibleSize.height));
    
    menuSprite->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
    menuSprite->setOpacity(0);
    addChild(menuSprite, 3);

    
    auto back_sprite = Sprite::create(back_name);
    
    back_sprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
    back_sprite->setTextureRect(Rect(origin.x, origin.y, visibleSize.width, visibleSize.height));
    
    back_sprite->setPosition(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2);
    addChild(back_sprite, 0);
    
    
    this->map = TMXTiledMap::create(map_name);
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
    map->setName(map_name);
    addChild(map, 0);
    
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.
    
    // add a "close" icon to exit the progress. it's an autorelease object
    
    
    
    this->pauseItem = MenuItemImage::create(
                                            "PauseNormal.png",
                                            "PauseSelected.png",
                                            CC_CALLBACK_1(AbstractLabirint::menuPauseCallback, this));
    
    pauseItem->setPosition(Vec2(origin.x + visibleSize.width - pauseItem->getContentSize().width*icon_scale / 2,
                                origin.y + pauseItem->getContentSize().height*icon_scale / 2));
    
    pauseItem->setScale(icon_scale);
    pauseItem->setVisible(true);
    pauseItem->setGlobalZOrder(3);
    pauseItem->getNormalImage()->setGlobalZOrder(3);
    pauseItem->getSelectedImage()->setGlobalZOrder(3);
    ///////////////////////////////////////////////////////////////////////////
    this->closeItem = MenuItemImage::create(
                                            "CloseNormal.png",
                                            "CloseSelected.png",
                                            CC_CALLBACK_1(AbstractLabirint::menuCloseCallback, this));
    
    closeItem->setOpacity(0);
    closeItem->setPosition(Vec2(origin.x + visibleSize.width/2
                                , origin.y + visibleSize.height + closeItem->getContentSize().height));
    closeItem->setScale(icon_scale);
    closeItem->setGlobalZOrder(3);
    closeItem->getNormalImage()->setGlobalZOrder(3);
    closeItem->getSelectedImage()->setGlobalZOrder(3);
    ///////////////////////////////////////////////////////////////////////////
    this->restartItem = MenuItemImage::create(
                                              "RestartNormal.png",
                                              "RestartSelected.png",
                                              CC_CALLBACK_1(AbstractLabirint::menuRestartCallback, this));
    restartItem->setOpacity(0);
    restartItem->setPosition(Vec2(origin.x + visibleSize.width/2,
                                  origin.y + visibleSize.height + restartItem->getContentSize().height));
    restartItem->setScale(icon_scale);
    restartItem->setGlobalZOrder(3);
    restartItem->getNormalImage()->setGlobalZOrder(3);
    restartItem->getSelectedImage()->setGlobalZOrder(3);
    ///////////////////////////////////////////////////////////////////////////
    this->continueItem = MenuItemImage::create(
                                               "PlayNormal.png",
                                               "PlaySelected.png",
                                               CC_CALLBACK_1(AbstractLabirint::menuContinueCallback, this));
    continueItem->setOpacity(0);
    continueItem->setPosition(Vec2(origin.x + visibleSize.width/2,
                                   origin.y + visibleSize.height + continueItem->getContentSize().height));
    continueItem->setScale(icon_scale);
    continueItem->setGlobalZOrder(3);
    continueItem->getNormalImage()->setGlobalZOrder(3);
    continueItem->getSelectedImage()->setGlobalZOrder(3);
    
    auto menu = Menu::create(pauseItem, closeItem, restartItem, continueItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    
    //////////////////////////////////////////////////
    
    TMXObjectGroup *walls = map->getObjectGroup("collisions");
    
    this->collisions = makeObject(COLLISION_TAG, walls, scale_map, xZero, yZero, BRICK, false, 0, 0);
    
    auto healthcache = SpriteFrameCache::getInstance();
    healthcache->addSpriteFramesWithFile("plus.plist");
    TMXObjectGroup *health = map->getObjectGroup("health");
    auto plusCount = 1;
    auto plusAnimSize = 4;
    this->pluses = makeObject(PLUS_TAG, health, healthcache, "plus", plusCount, plusAnimSize, scale_map, xZero, yZero,
                              BALL, 0.1f, false, 0.0f, 0.0f, 0.0f);
    
    //////////////////////////////////////////////////
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("life.plist");
    this->mylife = Sprite::createWithSpriteFrameName("life3.png");
    this->mylife->setScale(icon_scale);
    mylife->setOpacity(230);
    mylife->setBlendFunc((BlendFunc) {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
    mylife->setPosition(origin.x + visibleSize.width / 2, origin.y + mylife->getContentSize().height*icon_scale/2);
    addChild(mylife, 3);
    mylife->setLocalZOrder(5);
    
    // load the Sprite Sheet
    auto spritecache = SpriteFrameCache::getInstance();
    
    // the .plist file can be generated with any of the tools mentioned below
    spritecache->addSpriteFramesWithFile("tgirl.plist");
    
    
    
    TMXObjectGroup *heroObject = map->getObjectGroup("hero");
    float x, y, obj_width, obj_height;
    if (heroObject != nullptr){
        ValueVector objectsPoint = heroObject->getObjects();
        for (auto objPointMap : objectsPoint) {
            ValueMap objPoint = objPointMap.asValueMap();
            x = objPoint.at("x").asFloat();
            y = objPoint.at("y").asFloat();
            obj_width = objPoint.at("width").asFloat();
            obj_height = objPoint.at("height").asFloat();
            
            break;
        }
    }
    
    
    this->mysprite = Sprite::createWithSpriteFrameName("topgoing01.png");
    auto w = mysprite->getContentSize().width;
    auto h = mysprite->getContentSize().height;
    // position the sprite on the center of the screen
    mysprite->setTag(HERO_SPRITE_TAG);
    mysprite->setPosition(Vec2(xZero + scale_map*(x+obj_width/2.0f), yZero + scale_map*(y+obj_height/2.0f)));
    scale_hero = fmax(obj_width/w, obj_height/h)*scale_map;
    mysprite->setScale(scale_hero);
    
    
    Vector<SpriteFrame *> animGoingFrames;
    animGoingFrames.reserve(8);
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing01.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing02.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing03.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing02.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing01.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing06.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing07.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName("topgoing02.png"));
    
    // create the animation out of the frames
    Animation *animationGoing = Animation::createWithSpriteFrames(animGoingFrames, ANIMATION_DELAY);// / scale_map);
    animateGoing = Animate::create(animationGoing);
    animateGoing->retain();
    
    
    auto physicsBody = PhysicsBody::createBox(Size(h * scale_hero, h * scale_hero),
                                              PhysicsMaterial(1.0f, 0.0f, 0.0f), Vec2(0, 0));
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(true);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    //set the body isn't affected by the physics world's gravitational force
    physicsBody->setGravityEnable(false);
    physicsBody->setVelocityLimit(MY_VELOCITY*scale_map);
    physicsBody->setMass(0.001);
    addChild(this->mysprite, 2);
    
    mysprite->setPhysicsBody(physicsBody);
    //    this->scheduleUpdate();
    return true;
}


void AbstractLabirint::onEnter() {
    Layer::onEnter();
    
    // Register Touch Event
    auto dispatcher = Director::getInstance()->getEventDispatcher();
    auto listener = EventListenerTouchOneByOne::create();
    
    listener->onTouchBegan = CC_CALLBACK_2(AbstractLabirint::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(AbstractLabirint::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(AbstractLabirint::onTouchEnded, this);
    listener->onTouchCancelled = CC_CALLBACK_2(AbstractLabirint::onTouchCancelled, this);
    
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(AbstractLabirint::onContactBegin,
                                                    this);
    
    contactListener->onContactSeperate = CC_CALLBACK_1(AbstractLabirint::onContactSeperate,
                                                       this);
    
    dispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    dispatcher->addEventListenerWithSceneGraphPriority(contactListener,
                                                       this);
}


void AbstractLabirint::onExit() {
    // You don't need to unregister listeners here as new API
    // removes all linked listeners automatically in CCNode's destructor
    // which is the base class for all cocos2d DRAWING classes
    animateGoing->release();

    Layer::onExit();
}

void AbstractLabirint::setNextLevelNum(int next_level) {
    cocos2d::UserDefault *def=UserDefault::getInstance();
    auto high_score=def->getIntegerForKey(HIGH_SCORE);
    if (high_score < next_level) {
        def->setIntegerForKey(HIGH_SCORE, next_level);
        def->flush();
    }
}

void AbstractLabirint::update(float delta) {
    //life_num = 3;
    if (!isNewLeveled && !isNewLevel && !isAll && isOnlyStart){
        audio->resumeBackgroundMusic();
        isOnlyStart = false;
    }
    if (getChildByName("continued") && getChildByName("continued")->getNumberOfRunningActions() <= 0) {
        removeChildByName("continued");
        cocos2d::Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, LevelsScene::createScene()));
    }
    if ((closeItem->getNumberOfRunningActions() <= 0) && (closeItem->getOpacity() == 0)) {
        if (isClose) {
            isClose = false;
            auto newScene = LevelsScene::createScene();
            cocos2d::Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, newScene));
        }
    }
    if ((isRestarted) && (menuSprite->getNumberOfRunningActions() <= 0) && (menuSprite->getOpacity() == 0)) {
        isRestarted = false;
        isRestart = false;
        auto newScene = returnRestartedScene();
        cocos2d::Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, newScene));
    }
    else if ((isNewLeveled) && (menuSprite->getNumberOfRunningActions() <= 0) && (menuSprite->getOpacity() == 0))
    {
        isNewLeveled = false;
        isNewLevel = false;
        auto newScene = returnNewScene();
        audio->pauseBackgroundMusic();
        pauseAllObjects();
        
        if (newScene) {
            cocos2d::Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, newScene));
        }
        else {
            auto sprite = Sprite::create("ByeWorld.png");
            sprite->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
            sprite->setOpacity(0);
            sprite->setName("continued");
            addChild(sprite, 5);
            
            sprite->runAction(Sequence::create(FadeTo::create(1.0, 255), FadeTo::create(1.0, 0), NULL));
        }
    }
    else if (isNewLevel) {
        isAll = true;
        isPaused = false;
        isNewLevel = false;
        isNewLeveled = true;
        pauseScene();
        pause();
        
    }
    else if (!isPaused&&!isRestart && !isNewLevel&&!isNewLeveled) {
        for (auto plus: pluses) {
            if (plus->getOpacity() < 255) {
                plus->setOpacity(plus->getOpacity() + 1);
                
                if (plus->getOpacity() == 255) {
                    if (isPlus)
                        collisionWithHealth(plus, mysprite);
                }
            }
        }
        if ((touchX != NOTOUCH) && (touchY != NOTOUCH)) {
            audio->playEffect("step.wav", false, 0.2f, 0.0f, 0.1f);
            goHero();
        }

        ownEvent();
    }
    
}


bool AbstractLabirint::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (!isRestart && !isNewLevel && !isRestarted && !isNewLeveled) {
        touchX = touch->getLocation().x;
        touchY = touch->getLocation().y;
        startGoingAnimation();
        goHero(true);
    }
    
    return true;
}

void AbstractLabirint::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (!isRestart && !isNewLevel && !isRestarted && !isNewLeveled && touchX != NOTOUCH && touchY != NOTOUCH) {
        touchX = touch->getLocation().x;
        touchY = touch->getLocation().y;
        goHero(true);
    } else {
        stopTakingPoints();
    }
    
}

void AbstractLabirint::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event) {
    stopTakingPoints();
}

void AbstractLabirint::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event) {
    onTouchEnded(touch, event);
}


void AbstractLabirint::stopTakingPoints(){
    touchX = NOTOUCH;
    touchY = NOTOUCH;
    stopAllObjects();
}

void AbstractLabirint::goHero(bool isStarted) {
    auto dx = touchX - mysprite->getPositionX();
    auto dy = touchY - mysprite->getPositionY() + mysprite->getContentSize().height * scale_hero / 2;
    goToPoint(dx, dy, isStarted);
}

void AbstractLabirint::goToPoint(float dx, float dy, bool isStarted) {
    if ((touchX != NOTOUCH) && (touchY != NOTOUCH)) {
        float vx_old = mysprite->getPhysicsBody()->getVelocity().x;
        float vy_old = mysprite->getPhysicsBody()->getVelocity().y;
        bool isChangedDirection = false;
        float vx = 0;
        float vy = 0;
        if (dx > 0) vx = MY_VELOCITY * scale_map;
        else if (dx < 0) vx = -MY_VELOCITY * scale_map;
        
        if (dy > 0) vy = MY_VELOCITY * scale_map;
        else if (dy < 0) vy = -MY_VELOCITY * scale_map;
        
        auto body = mysprite->getPhysicsBody();
        
        if (isStarted or (vx_old > 0 and dx < 0) or (vx_old < 0 and dx > 0) or (vy_old > 0 and dy < 0) or (vy_old < 0 and dy > 0) or
            ((vx_old == 0 or vy_old == 0) and (dx != 0 or dy != 0))){
            isChangedDirection = true;
            
            if (fabs(dx) > fabs(dy)){
                vy = vy * fabs(dy / dx);
                if (dx > 0)
                    mysprite->setRotation(180);
                else if (dx < 0)
                    mysprite->setRotation(0);
            } else if (fabs(dx) < fabs(dy)){
                vx = vx * fabs(dx / dy);
                if (dy > 0)
                    mysprite->setRotation(90);
                else if (dy < 0)
                    mysprite->setRotation(-90);
            }

        }

        if (isChangedDirection) {
            body->resetForces();
            body->setGravityEnable(false);
            body->applyForce(Vec2(vx, vy));
        }
    }
    else {
        stopAllObjects();
    }
}

void AbstractLabirint::startGoingAnimation() {
    if (!mysprite->getActionByTag(GOING)) {
        auto action = RepeatForever::create(animateGoing);
        action->setTag(GOING);
        mysprite->runAction(action);
    }
}

void AbstractLabirint::stopAllObjects() {
    mysprite->stopActionByTag(GOING);
    mysprite->getPhysicsBody()->resetForces();
    mysprite->getPhysicsBody()->setGravityEnable(false);
    mysprite->getPhysicsBody()->setVelocity(Vec2(0, 0));

}

void AbstractLabirint::pauseAllObjects() {
    if (mysprite->getNumberOfRunningActions() > 0) {
        mysprite->pause();
    }
}

void AbstractLabirint::resumeAllObjects() {
    if (mysprite->getNumberOfRunningActions() > 0) {
        mysprite->resume();
    }
}

Vector<Sprite *> AbstractLabirint::makeObject(int tag, TMXObjectGroup *objects, SpriteFrameCache *spritecache,
                                              std::string name, int spritecount, int animsize, float scale_map, float xZero, float yZero, int form, float time,
                                              bool isDynamic, float rest, float fric, float dens, int mask) {
    Vector<Sprite *> sprites;
    if (objects != nullptr) {
        Vector<Animate *> anim;
        
        Vector<SpriteFrame *> spiderFrames;
        for (int i = 0; i < spritecount; i++) {
            Vector<SpriteFrame *> spiderFrames;
            spiderFrames.reserve(animsize);
            for (int j = 1; j <= animsize; j++) {
                char *res = new char[50];
                std::sprintf(res, "%s%02i.png", name.c_str(), i * animsize + j);
                spiderFrames.pushBack(spritecache->getSpriteFrameByName(res));
            }
            Animate *animate = Animate::create(Animation::createWithSpriteFrames(spiderFrames, time));
            anim.pushBack(animate);
        }
        
        float x, y, w, h;
        ValueVector objectsPoint = objects->getObjects();
        int i = 0;
        for (auto objPointMap : objectsPoint) {
            ValueMap objPoint = objPointMap.asValueMap();
            x = objPoint.at("x").asFloat();
            y = objPoint.at("y").asFloat();
            w = objPoint.at("width").asFloat();
            h = objPoint.at("height").asFloat();
            
            Point _point = Point(xZero + scale_map * (x + w / 2.0f), yZero + scale_map * (y + h / 2.0f));
            Size _size = Size(scale_map * w, scale_map * h);
            char *res = new char[50];
            std::sprintf(res, "%s%02i.png", name.c_str(), (i % spritecount) * animsize + 1);
            auto sprite = this->makePhysicsObjAt(tag, _point, _size, isDynamic, rest, fric, dens, form,
                                                 anim.at(i % spritecount)->clone(), res, mask);
            
            auto sprite_name = objPoint.at("name").asString();
            if (sprite_name == "") sprite_name = std::to_string(i);
            i++;
            
            sprite->setName(sprite_name);
            sprites.pushBack(sprite);
        }
    }
    return sprites;
}


Vector<Sprite *> AbstractLabirint::makeObject(int tag, TMXObjectGroup *objects, float scale_map, float xZero, float yZero,
                                              int form, bool isDynamic, int v, int n, int mask) {
    Vector<Sprite *> sprites;
    if (objects != nullptr) {
        float x, y, w, h;
        ValueVector objectsPoint = objects->getObjects();
        
        int i = 0;
        for (auto objPointMap : objectsPoint) {
            ValueMap objPoint = objPointMap.asValueMap();
            x = objPoint.at("x").asFloat();
            y = objPoint.at("y").asFloat();
            w = objPoint.at("width").asFloat();
            h = objPoint.at("height").asFloat();
            auto name = objPoint.at("name").asString();
            Point _point = Point(xZero + scale_map * (x + w / 2.0f), yZero + scale_map * (y + h / 2.0f));
            Size _size = Size(scale_map * w, scale_map * h);
            auto tagname = tag;
            if (name == "dummy") tagname = 0;
            else if (name == "newlevel") tagname = NEWLEVEL_TAG;
            auto sprite = this->makePhysicsObjAt(name, tagname, _point, _size, form, isDynamic, v, n, mask);
            
            if (name == "") name = std::to_string(i);
            
            i++;
            sprite->setName(name);
            sprites.pushBack(sprite);
            
        }
    }
    return sprites;
}

Sprite *AbstractLabirint::makeTexturedSprite(std::string name, int tag, Point p, Size size) {
    Sprite * sprite;
    auto w = size.width;
    auto h = size.height;
    if (name == "dummy") {
        sprite = Sprite::create("start.png");
        sprite->setScale(w/sprite->getContentSize().width, h/sprite->getContentSize().height);
    } else if (name == "newlevel") {
        sprite = Sprite::create("finish.png");
        sprite->setScale(w/sprite->getContentSize().width, h/sprite->getContentSize().height);
    } else {
        sprite = Sprite::create();
        sprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
        sprite->setTextureRect(Rect(p.x - size.width / 2, p.y - size.height / 2, size.width, size.height));
        sprite->setOpacity(0);
    }
    return sprite;
}

Sprite *AbstractLabirint::makePhysicsObjAt(std::string name, int tag, Point p, Size size, int form, bool isDynamic, int v, int n, int mask) {
    auto sprite = makeTexturedSprite(name, tag, p, size);
    
    PhysicsBody *body;
    
   // size = sprite->getContentSize();
    
    if (form == BRICK) {
        body = PhysicsBody::createBox(size,
                                      PhysicsMaterial(1.0f, 1.0f, 0.0f));
    } else if (form == BALL) {
        body = PhysicsBody::createCircle(MAX(size.width, size.height) / 2.0,
                                         PhysicsMaterial(0.1f, 1.0f, 0.0f));
    }
    
    sprite->setPhysicsBody(body);
    sprite->setTag(tag);
    body->setDynamic(isDynamic);
    body->setContactTestBitmask(mask);
    body->setCollisionBitmask(mask);
    body->setCategoryBitmask(mask);
    sprite->setPosition(p);
    addChild(sprite, 1);
    
    
    auto moveBy2 = MoveBy::create(1, Vec2(0, v));
    if (n < 0)
        sprite->runAction(RepeatForever::create(moveBy2));
    else
        sprite->runAction(Repeat::create(moveBy2, n));
    return sprite;
}

Sprite *AbstractLabirint::makePhysicsObjAt(int tag, Point p, Size size, bool isDynamic, float rest, float fric, float dens,
                                           int form, Animate *anim, std::string name, int mask) {
    auto sprite = Sprite::createWithSpriteFrameName(name);
    auto sprite_scale = size.width / sprite->getContentSize().width;
    sprite->setScale(sprite_scale * 1.5);
    sprite->runAction(RepeatForever::create(anim));
    PhysicsBody *body;
    if (form == BRICK) {
        body = PhysicsBody::createBox(size,
                                      PhysicsMaterial(1.0f, 1.0f, 0.0f));
    } else if (form == BALL) {
        body = PhysicsBody::createCircle(MAX(size.width, size.height) / 2.0,
                                         PhysicsMaterial(1.0f, 1.0f, 0.0f));
    }
    sprite->setTag(tag);
    if (isDynamic) {
        body->getShape(0)->setRestitution(rest);
        body->getShape(0)->setFriction(fric);
        body->getShape(0)->setDensity(dens);
        body->setMass(0.001);
        body->setVelocityLimit(MY_VELOCITY*scale_map);
        body->applyImpulse(Vec2(0.5*scale_map, 0.5*scale_map));
        body->setRotationEnable(false);
    }
    body->setDynamic(isDynamic);
    
    body->setContactTestBitmask(mask); //(0xFFFFFFFF);
    sprite->setPhysicsBody(body);
    sprite->setPosition(p);
    
    addChild(sprite, 1);
    return sprite;
}


void AbstractLabirint::stopAllObjectLayer(Vector<Sprite *> sprites) {
    for (auto sprite : sprites) {
        if (sprite->getNumberOfRunningActions() > 0) {
            sprite->stopAllActions();
        }
    }
}


void AbstractLabirint::pauseAllObjectLayer(Vector<Sprite *> sprites) {
    for (auto sprite : sprites) {
        if (sprite->getNumberOfRunningActions() > 0) {
            sprite->pause();
        }
    }
}


void AbstractLabirint::resumeAllObjectLayer(Vector<Sprite *> sprites) {
    for (auto sprite : sprites) {
        if (sprite->getNumberOfRunningActions() > 0) {
            sprite->resume();
        }
    }
}

void AbstractLabirint::resumeScene() {
    resumeAllObjects();
}

void AbstractLabirint::pauseScene() {
    pauseAllObjects();
}

void AbstractLabirint::stopScene() {
    stopAllObjects();
}

void AbstractLabirint::collisionWithHealth(Node *nodeA, Node *nodeB) {
    SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName("plussplash.png");
    Node *node;
    if (nodeA->getTag() == PLUS_TAG) node = nodeA;
    else node = nodeB;
    isPlus = true;
    if (node->getOpacity() == 255) {
        auto plus = pluses.at(stoi(node->getName()));
        
        if (life_num < 3) {
            audio->playEffect("chime.wav", false, 3.0f, 0.0f, 1.0f);

            life_num++;
            char *res = new char[50];
            sprintf(res, "life%i.png", life_num);
            SpriteFrame *splife = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
            mylife->setSpriteFrame(splife);
            
            plus->setSpriteFrame(sp);
            mysprite->runAction(Sequence::create(TintTo::create(0.5f, 252, 255, 0), TintTo::create(0.5, 255, 255, 255), NULL));
            
            plus->setOpacity(10);
        }
    }
}

void AbstractLabirint::finishMenuAction(){
    if (continueItem->getNumberOfRunningActions() > 0){
        continueItem->stopAllActions();
    }
    if (restartItem->getNumberOfRunningActions() > 0){
        restartItem->stopAllActions();
    }
    if (closeItem->getNumberOfRunningActions() > 0){
        closeItem->stopAllActions();
    }
    
    if (isPaused) {
        continueItem->setScale(icon_scale);
        continueItem->setOpacity(0);
        
        restartItem->setScale(icon_scale);
        restartItem->setOpacity(0);
        
        closeItem->setScale(icon_scale);
        closeItem->setOpacity(0);
    } else {
        if (isNewLevel){
            continueItem->setScale(1.5*icon_scale);
            restartItem->setScale(icon_scale);
            closeItem->setScale(icon_scale);
        }
        continueItem->setOpacity(255);
        restartItem->setOpacity(255);
        closeItem->setOpacity(255);
    }
    
}
void AbstractLabirint::prettyShowItem(MenuItemImage* item, Vec2 endPosition){
    if (item->getOpacity() == 0)
        item->runAction(cocos2d::FadeTo::create(1.0, 255));
    else
        item->runAction(cocos2d::FadeTo::create(1.0, 0));
    
    item->runAction(cocos2d::MoveTo::create(1.0, endPosition));
}

void AbstractLabirint::pause(){
    audio->pauseBackgroundMusic();
    if(!isPaused) {
        isPaused = true;
        finishMenuAction();
        pauseScene();
        
        menuSprite->runAction(cocos2d::FadeTo::create(1.0, 150));
        
        if (!isRestart && !isNewLevel && !isNewLeveled) {
            prettyShowItem(continueItem, Vec2(origin.x + visibleSize.width/2,
                                              origin.y + visibleSize.height/2 + continueItem->getContentSize().height*icon_scale*1.5));
            prettyShowItem(restartItem, Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2));
            prettyShowItem(closeItem, Vec2(origin.x + visibleSize.width/2,
                                           origin.y + visibleSize.height/2 - closeItem->getContentSize().height*icon_scale*1.5));
        }
        else if (isNewLeveled) {
            prettyShowItem(continueItem, Vec2(origin.x + visibleSize.width/2,
                                              origin.y + visibleSize.height/2));
            
            continueItem->runAction(cocos2d::ScaleTo::create(1.0, icon_scale*1.5));
            prettyShowItem(restartItem, Vec2(origin.x + visibleSize.width/2 - continueItem->getContentSize().width*icon_scale*1.5,
                                             origin.y + visibleSize.height/2));
            prettyShowItem(closeItem, Vec2(origin.x + visibleSize.width/2 + continueItem->getContentSize().width*icon_scale*1.5,
                                           origin.y + visibleSize.height/2));
        } else if (isRestart) {
            prettyShowItem(restartItem, Vec2(origin.x + visibleSize.width/2, origin.y + visibleSize.height/2 + continueItem->getContentSize().height*icon_scale));
            prettyShowItem(closeItem, Vec2(origin.x + visibleSize.width/2,
                                           origin.y + visibleSize.height/2 - continueItem->getContentSize().height*icon_scale));
        }
    } else {
        if ((!isRestart) && (!isNewLeveled))
            resume();
    }
}


void AbstractLabirint::resume(bool isResumeScene){
    isPaused = false;
    finishMenuAction();
    prettyShowItem(continueItem, Vec2(origin.x + visibleSize.width/2,
                                      origin.y + visibleSize.height + continueItem->getContentSize().height));
    prettyShowItem(restartItem, Vec2(origin.x + visibleSize.width/2,
                                     origin.y + visibleSize.height + restartItem->getContentSize().height));
    prettyShowItem(closeItem, Vec2(origin.x + visibleSize.width/2,
                                   origin.y + visibleSize.height + closeItem->getContentSize().height));
    if (isNewLevel) {
        continueItem->runAction(cocos2d::ScaleTo::create(1.0, icon_scale));
    }
    menuSprite->runAction(cocos2d::FadeTo::create(1.0, 0));
    
    if (isResumeScene && !isAll) {
        audio->resumeBackgroundMusic();
        resumeScene();
    } else {
        isAll = false;
    }
}

void AbstractLabirint::hideBottom(){
    if (!isRestart)
        prettyShowItem(continueItem, Vec2(origin.x + visibleSize.width/2, origin.y - continueItem->getContentSize().height));
    prettyShowItem(restartItem, Vec2(origin.x + visibleSize.width/2, origin.y - continueItem->getContentSize().height));
    prettyShowItem(closeItem, Vec2(origin.x + visibleSize.width/2, origin.y - continueItem->getContentSize().height));
    
}

void AbstractLabirint::menuPauseCallback(Ref * pSender) {
    pause();
}

void AbstractLabirint::menuContinueCallback(Ref * pSender) {
    resume();
}

void AbstractLabirint::menuRestartCallback(Ref * pSender) {
    resume(false);
    isRestarted = true;
}

void AbstractLabirint::menuCloseCallback(Ref * pSender) {
    hideBottom();
    audio->stopBackgroundMusic();
    isClose = true;
    
}