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
#define VELOCITY 200

Bot::Bot(float xZero, float yZero, float obj_width, float obj_height, float x, float y, std::string plist_name, std::string sprite_name, float scale_map, int tag, std::string name) {
    this->scale_map = scale_map;
    auto spritecache = SpriteFrameCache::getInstance();
    spritecache->addSpriteFramesWithFile(plist_name);
    Vector<SpriteFrame *> animGoingFrames;
    animGoingFrames.reserve(8);
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left01.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left02.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left03.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left02.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left01.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left06.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left07.png"));
    animGoingFrames.pushBack(spritecache->getSpriteFrameByName(sprite_name+"left02.png"));
    
    // create the animation out of the frames
    Animation *animationGoing = Animation::createWithSpriteFrames(animGoingFrames, ANIMATION_DELAY);// / scale_map);
    botAnimateGoing = Animate::create(animationGoing);
    botAnimateGoing->retain();
    
    bot =Sprite::createWithSpriteFrameName(sprite_name+"left01.png");
    auto w = bot->getContentSize().width;
    auto h = bot->getContentSize().height;
    bot->setTag(tag);
    bot->setPosition(Vec2(xZero + scale_map*(x+obj_width/2.0f), yZero + scale_map*(y+obj_height/2.0f)));
    scale_bot = fmax(obj_width/w, obj_height/h)*scale_map;
    bot->setScale(scale_bot);
    direction = LEFT;
    auto action = RepeatForever::create(botAnimateGoing);
    bot->runAction(action);
    
    auto physicsBody = PhysicsBody::createBox(Size(fmin(w,h) * scale_bot, fmin(w,h) * scale_bot),
                                              PhysicsMaterial(1.0f, 0.5f, 0.0f), Vec2(0, 0));
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(true);
    physicsBody->setVelocityLimit(VELOCITY*scale_map);
    physicsBody->setMass(0.001);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    //set the body isn't affected by the physics world's gravitational force
    physicsBody->setGravityEnable(false);
    
    physicsBody->applyForce(Vec2(-VELOCITY * scale_map, 0));
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
    auto last_direction = direction;
    direction = rand() % DIRECTION_LIMIT;
    if (direction == last_direction)
        direction = (direction + 1) % DIRECTION_LIMIT;
    if (direction == LEFT) {
        bot->setRotation(0);
        bot->getPhysicsBody()->resetForces();
        bot->getPhysicsBody()->applyForce(Vec2(-VELOCITY * scale_map, 0));
    } else if (direction == RIGHT) {
        bot->setRotation(180);
        bot->getPhysicsBody()->resetForces();
        bot->getPhysicsBody()->applyForce(Vec2(VELOCITY * scale_map, 0));
    } else if (direction == TOP) {
        bot->setRotation(90);
        bot->getPhysicsBody()->resetForces();
        bot->getPhysicsBody()->applyForce(Vec2(0, VELOCITY * scale_map));
    } else if (direction == BOTTOM) {
        bot->setRotation(-90);
        bot->getPhysicsBody()->resetForces();
        bot->getPhysicsBody()->applyForce(Vec2(0, -VELOCITY * scale_map));
    }
}
