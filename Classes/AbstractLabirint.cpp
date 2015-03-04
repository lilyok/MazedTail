//
// Created by lilil on 27.02.15.
//

#include "AbstractLabirint.h"
#include "MenuScene.h"

USING_NS_CC;


Scene *AbstractLabirint::createScene(std::string map_name, std::string back_name) {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = AbstractLabirint::create(map_name, back_name);
    layer->setPhyWorld(scene->getPhysicsWorld());
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

    this->collisions = makeObject(COLLISION_TAG, walls, scale_map, xZero, yZero, BRICK, 0, 0);

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

    // load the Sprite Sheet
    auto spritecache = SpriteFrameCache::getInstance();

    // the .plist file can be generated with any of the tools mentioned below
    spritecache->addSpriteFramesWithFile("tgirlgo.plist");


    
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

    
    this->mysprite = Sprite::createWithSpriteFrameName("topleft01.png");
    auto w = mysprite->getContentSize().width;
    auto h = mysprite->getContentSize().height;
    // position the sprite on the center of the screen
    mysprite->setTag(HERO_SPRITE_TAG);
    mysprite->setPosition(Vec2(xZero + scale_map*(x+obj_width/2.0f), yZero + scale_map*(y+obj_height/2.0f)));
    scale_hero = fmax(obj_width/w, obj_height/h)*scale_map;
    mysprite->setScale(scale_hero);

 
    Vector<SpriteFrame *> animLeftFrames;
    animLeftFrames.reserve(8);
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft01.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft02.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft03.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft02.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft01.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft06.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft07.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName("topleft02.png"));

    // create the animation out of the frames
    Animation *animationLeft = Animation::createWithSpriteFrames(animLeftFrames, ANIMATION_DELAY / scale_map);
    animateLeft = Animate::create(animationLeft);
    animateLeft->retain();

    Vector<SpriteFrame *> animRightFrames;
    animRightFrames.reserve(8);
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright01.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright02.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright03.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright02.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright01.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright06.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright07.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName("topright02.png"));

    // create the animation out of the frames
    Animation *animationRight = Animation::createWithSpriteFrames(animRightFrames, ANIMATION_DELAY / scale_map);
    animateRight = Animate::create(animationRight);
    animateRight->retain();

    Vector<SpriteFrame *> animTopFrames;
    animTopFrames.reserve(8);
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop01.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop02.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop03.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop02.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop01.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop06.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop07.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName("toptop02.png"));


    // create the animation out of the frames
    Animation *animationTop = Animation::createWithSpriteFrames(animTopFrames, ANIMATION_DELAY / scale_map);
    animateTop = Animate::create(animationTop);
    animateTop->retain();

    Vector<SpriteFrame *> animBottomFrames;
    animBottomFrames.reserve(8);
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom01.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom02.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom03.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom02.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom01.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom06.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom07.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName("topbottom02.png"));

    // create the animation out of the frames
    Animation *animationBottom = Animation::createWithSpriteFrames(animBottomFrames, ANIMATION_DELAY / scale_map);
    animateBottom = Animate::create(animationBottom);
    animateBottom->retain();


    auto physicsBody = PhysicsBody::createBox(Size(w * scale_hero / 3, h * scale_hero/2),
            PhysicsMaterial(1.0f, 0.0f, 0.0f), Vec2(0, 0));
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(true);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    //set the body isn't affected by the physics world's gravitational force
    physicsBody->setGravityEnable(false);

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
    animateLeft->release();
    animateBottom->release();
    animateRight->release();
    animateTop->release();
    Layer::onExit();
}

void AbstractLabirint::update(float delta) {
    if ((closeItem->getNumberOfRunningActions() <= 0) && (closeItem->getOpacity() == 0)) {
        if (isClose) {
            isClose = false;
            auto newScene = MenuScene::createScene();
            cocos2d::Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, newScene));
        }
    }
}


bool AbstractLabirint::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (!isRestart && !isNewLevel && !isRestarted && !isNewLeveled) {
        touchX = touch->getLocation().x;
        touchY = touch->getLocation().y;

        auto dx = touch->getLocation().x - mysprite->getPositionX();
        auto dy = touch->getLocation().y - mysprite->getPositionY() + mysprite->getContentSize().height * scale_map / 2;
        direction = NODIRECTION;
        goToPoint(dx, dy);
    }


    return true;
}

void AbstractLabirint::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (!isRestart && !isNewLevel && !isRestarted && !isNewLeveled) {
        touchX = touch->getLocation().x;
        touchY = touch->getLocation().y;
    }
}

void AbstractLabirint::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event) {
    stopTakingPoints();
}

void AbstractLabirint::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event) {
    onTouchEnded(touch, event);
}


void AbstractLabirint::stopTakingPoints(){
    direction = NODIRECTION;
    stopAllObjects();
    touchX = -500000;
    touchY = -500000;
}

void AbstractLabirint::goToPoint(float dx, float dy) {
    if ((touchX != -500000) && (touchY != -500000)) {
        float vx_old = mysprite->getPhysicsBody()->getVelocity().x;
        float vy_old = mysprite->getPhysicsBody()->getVelocity().y;
        bool isChangedDirection = false;
        float vx;
        float vy;
        if (dx > 0) vx = MY_VELOCITY * scale_map;
        else vx = -MY_VELOCITY * scale_map;

        if (dy > 0) vy = MY_VELOCITY * scale_map;
        else vy = -MY_VELOCITY * scale_map;

        auto body = mysprite->getPhysicsBody();
        auto pos = body->getPositionOffset();

        if (std::abs(dx) > std::abs(dy)) {
            isChangedDirection = goToPointX(dx, dy, vx_old, vy_old, vx, body, pos, vy);
        }
        else {
            isChangedDirection = goToPointY(dx, dy, vx_old, vy_old, vy, body, pos, vx);
        }
        if (isChangedDirection) {
            mysprite->getPhysicsBody()->setVelocity(Vec2(vx, vy));
        }
    }
    else {
        stopAllObjects();
        mysprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
    }
}

bool AbstractLabirint::goToPointY(float dx, float dy, float vx_old, float vy_old, float vy, PhysicsBody *body,
        Vec2 &pos, float &vx) {
    bool isChangedDirection = false;
    if (dy == 0) vx = 0.0;
    else vx = vx * std::__1::abs(dx / dy);
    if ((std::__1::abs(vy_old - vy) >= MIN_FOR_DIRECTION) || (std::__1::abs(vx_old - vx) >= MIN_FOR_DIRECTION)) {
        if (dy < 0) {
            if (direction != BOTTOM) {
                stopAllObjects();
                isChangedDirection = true;
                direction = BOTTOM;
                auto action = RepeatForever::create(animateBottom);
                action->setTag(BOTTOM);
                mysprite->runAction(action);
                body->setRotationOffset(90);
                if (pos.y >= 0) {
                    pos.y = -mysprite->getContentSize().height * scale_hero * 5 / 12;
                    pos.x = 0;
                }
                body->setPositionOffset(pos);
            }
        }
        else {
            if (direction != TOP) {
                stopAllObjects();
                isChangedDirection = true;
                direction = TOP;
                auto action = RepeatForever::create(animateTop);
                action->setTag(TOP);
                mysprite->runAction(action);
                body->setRotationOffset(90);
                if (pos.y <= 0) {
                    pos.y = mysprite->getContentSize().height * scale_hero * 5 / 12;
                    pos.x = 0;
                }
                body->setPositionOffset(pos);
            }
        }

    }
    return isChangedDirection;
}

bool AbstractLabirint::goToPointX(float dx, float dy, float vx_old, float vy_old, float vx, PhysicsBody *body,
        Vec2 &pos, float &vy) {
    bool isChangedDirection = false;

    if (dx == 0) vy = 0.0;
    else vy = vy * std::__1::abs(dy / dx);


    if ((fabs(vy_old - vy) >= MIN_FOR_DIRECTION) || (fabs(vx_old - vx) >= MIN_FOR_DIRECTION)) {
        if (dx > 0) {
            if (direction != RIGHT) {
                stopAllObjects();
                auto action = RepeatForever::create(animateRight);
                action->setTag(RIGHT);
                isChangedDirection = true;
                direction = RIGHT;
                mysprite->runAction(action);
                body->setRotationOffset(0);
                if (pos.x <= 0) {
                    pos.y = 0;
                    pos.x = mysprite->getContentSize().height * scale_hero * 5 / 12;
                }
                body->setPositionOffset(pos);

            }
        }
        else {
            if (direction != LEFT) {
                stopAllObjects();
                auto action = RepeatForever::create(animateLeft);
                action->setTag(LEFT);
                isChangedDirection = true;
                direction = LEFT;
                mysprite->runAction(action);
                mysprite->getPhysicsBody()->setRotationOffset(0);
                if (pos.x >= 0) {
                    pos.y = 0;
                    pos.x = -mysprite->getContentSize().height * scale_hero * 5 / 12;
                }
                body->setPositionOffset(pos);


            }
        }
    }
    return isChangedDirection;
}

void AbstractLabirint::stopAllObjects() {
    if (mysprite->getNumberOfRunningActions() > 0) {
        mysprite->stopActionByTag(TOP);
        mysprite->stopActionByTag(BOTTOM);
        mysprite->stopActionByTag(LEFT);
        mysprite->stopActionByTag(RIGHT);

        mysprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
    }
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
        int form, int v, int n, int mask) {
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
            auto sprite = this->makePhysicsObjAt(tagname, _point, _size, form, v, n, mask);

            if (name == "") name = std::to_string(i);
            i++;
            sprite->setName(name);
            sprites.pushBack(sprite);

        }
    }
    return sprites;
}

Sprite *AbstractLabirint::makePhysicsObjAt(int tag, Point p, Size size, int form, int v, int n, int mask) {
    auto sprite = Sprite::create();

    sprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
    sprite->setTextureRect(Rect(p.x - size.width / 2, p.y - size.height / 2, size.width, size.height));
    sprite->setOpacity(0);
    PhysicsBody *body;


    if (form == BRICK) {
        body = PhysicsBody::createBox(size,
                PhysicsMaterial(1.0f, 1.0f, 0.0f));
    } else if (form == BALL) {
        body = PhysicsBody::createCircle(MAX(size.width, size.height) / 2.0,
                PhysicsMaterial(0.1f, 1.0f, 0.0f));
    }

    sprite->setPhysicsBody(body);
    sprite->setTag(tag);
    body->setDynamic(false);
    body->setContactTestBitmask(mask);
    sprite->setPosition(p);
    addChild(sprite, 3);


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
                PhysicsMaterial(1.0f, 1.0f, 0.2f));
    }
    sprite->setTag(tag);
    if (isDynamic) {
        body->getShape(0)->setRestitution(rest);
        body->getShape(0)->setFriction(fric);
        body->getShape(0)->setDensity(dens);
        body->setVelocity(Vec2(100, 0));
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
    if (node->getOpacity() == 255) {
        auto plus = pluses.at(stoi(node->getName()));

        if (life_num < 3) {
            life_num++;
            char *res = new char[50];
            sprintf(res, "life%i.png", life_num);
            SpriteFrame *splife = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
            mylife->setSpriteFrame(splife);
        
            plus->setSpriteFrame(sp);
            mysprite->runAction(Sequence::create(TintTo::create(0.5f, 252, 255, 0), TintTo::create(0.5, 255, 255, 255), NULL));
            isPlus = true;
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
    
    if (isResumeScene)
        resumeScene();
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
    if (!isNewLeveled)
        resume();
}

void AbstractLabirint::menuRestartCallback(Ref * pSender) {
    resume(false);
    isRestarted = true;
}

void AbstractLabirint::menuCloseCallback(Ref * pSender) {
    hideBottom();
    isClose = true;
    
}