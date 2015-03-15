//
//  Bot.cpp
//  MazedTail
//
//  Created by lilil on 14.03.15.
//
//

#include "Bot.h"

USING_NS_CC;
#define ANIMATION_DELAY 0.02f
#define LEFT 0
#define RIGHT 1
#define TOP 2
#define BOTTOM 3
#define DIRECTION_LIMIT 4
#define VELOCITY 100

Bot::Bot(float xZero, float yZero, float obj_width, float obj_height, float x, float y, std::string plist_name, std::string sprite_name, float scale_map, int tag, std::string name) {
    this->scale_map = scale_map;
    auto spritecache = SpriteFrameCache::getInstance();
    spritecache->addSpriteFramesWithFile(plist_name);
    Vector<SpriteFrame *> animLeftFrames;
    animLeftFrames.reserve(8);
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left01.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left02.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left03.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left02.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left01.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left06.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left07.png"));
    animLeftFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left02.png"));
    
    // create the animation out of the frames
    Animation *animationLeft = Animation::createWithSpriteFrames(animLeftFrames, ANIMATION_DELAY / scale_map);
    botAnimateLeft = Animate::create(animationLeft);
    botAnimateLeft->retain();
    
    Vector<SpriteFrame *> animRightFrames;
    animRightFrames.reserve(8);
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right01.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right02.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right03.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right02.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right01.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right06.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right07.png"));
    animRightFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"right02.png"));
    
    // create the animation out of the frames
    Animation *animationRight = Animation::createWithSpriteFrames(animRightFrames, ANIMATION_DELAY / scale_map);
    botAnimateRight = Animate::create(animationRight);
    botAnimateRight->retain();
    
    Vector<SpriteFrame *> animTopFrames;
    animTopFrames.reserve(8);
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top01.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top02.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top03.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top02.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top01.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top06.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top07.png"));
    animTopFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"top02.png"));
    
    // create the animation out of the frames
    Animation *animationTop = Animation::createWithSpriteFrames(animTopFrames, ANIMATION_DELAY / scale_map);
    botAnimateTop = Animate::create(animationTop);
    botAnimateTop->retain();
    
    Vector<SpriteFrame *> animBottomFrames;
    animBottomFrames.reserve(8);
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom01.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom02.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom03.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom02.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom01.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom06.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom07.png"));
    animBottomFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"bottom02.png"));
    
    // create the animation out of the frames
    Animation *animationBottom = Animation::createWithSpriteFrames(animBottomFrames, ANIMATION_DELAY / scale_map);
    botAnimateBottom = Animate::create(animationBottom);
    botAnimateBottom->retain();
    
    bot =Sprite::createWithSpriteFrameName(sprite_name+"bottom01.png");
    auto w = bot->getContentSize().width;
    auto h = bot->getContentSize().height;
    bot->setTag(tag);
    bot->setPosition(Vec2(xZero + scale_map*(x+obj_width/2.0f), yZero + scale_map*(y+obj_height/2.0f)));
    scale_bot = fmax(obj_width/w, obj_height/h)*scale_map;
    bot->setScale(scale_bot);
    direction = BOTTOM;
    auto action = RepeatForever::create(botAnimateBottom);
    action->setTag(BOTTOM);
    bot->runAction(action);
    
    auto physicsBody = PhysicsBody::createBox(Size(fmin(w,h) * scale_bot, fmin(w,h) * scale_bot),
                                              PhysicsMaterial(1.0f, 1.0f, 0.0f), Vec2(0, 0));
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(true);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    //set the body isn't affected by the physics world's gravitational force
    physicsBody->setGravityEnable(false);
    
    physicsBody->setVelocity(Vec2(0, -VELOCITY * scale_map));
    bot->setPhysicsBody(physicsBody);
    
    bot->setName(name);
    
    int num = stoi(name);
    if (stoi(name) % 3 == 0)
        bot->runAction(TintTo::create(0.5, 250 - num*20, 255, 255));
    else if (stoi(name) % 3 == 1)
        bot->runAction(TintTo::create(0.5, 255, 250 - num*20, 255));
    else
        bot->runAction(TintTo::create(0.5, 255, 255, 250 - num*20));
}

void Bot::changeDirection(){
    bot->stopActionByTag(direction);
    direction = (direction + 1) % DIRECTION_LIMIT;
    cocos2d::Action* action;
    if (direction == LEFT) {
        action = RepeatForever::create(botAnimateLeft);
        bot->getPhysicsBody()->setVelocity(Vec2(-VELOCITY * scale_map, 0));
    } else if (direction == RIGHT) {
        action = RepeatForever::create(botAnimateRight);
        bot->getPhysicsBody()->setVelocity(Vec2(VELOCITY * scale_map, 0));
    } else if (direction == TOP) {
        action = RepeatForever::create(botAnimateTop);
        bot->getPhysicsBody()->setVelocity(Vec2(0, VELOCITY * scale_map));
    } else if (direction == BOTTOM) {
        action = RepeatForever::create(botAnimateBottom);
        bot->getPhysicsBody()->setVelocity(Vec2(0, -VELOCITY * scale_map));

    }
    action->setTag(direction);
    bot->runAction(action);
    
}
