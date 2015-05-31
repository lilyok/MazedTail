//
//  Ears.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Ears.h"
#include "Balance.h"
USING_NS_CC;

#define DING_TAG 20

#define EAR_TAG 105
#define EARBF_TAG 100
#define EARFLOOR_TAG 110
#define STOPFLOOR_TAG 115


Scene *Ears::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Ears::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Ears::init() {
    //////////////////////////////
    // 1. super init first
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("earbf.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("earbfred.plist");
    
    
    if (!AbstractLabirint::init("ears.tmx", "back10.png")) {
        return false;
    }
    
    auto earfloortmx = map->getObjectGroup("earfloor");
    this->earfloor = makeObject(EARFLOOR_TAG, earfloortmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0xFFFFFFFF);
    auto stopfloortmx = map->getObjectGroup("stopfloor");
    this->stopfloor = makeObject(STOPFLOOR_TAG, stopfloortmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0xFFFFFFFF);
    
    auto dingtmx = map->getObjectGroup("ding");
    this->dings = makeObject(DING_TAG, dingtmx, scale_map, xZero, yZero, BALL, false, 0, 0, 0xFFFFF000);
    auto eartmx = map->getObjectGroup("ears");
    this->ears = makeObject(EAR_TAG, eartmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0xFFFFF000);

    auto earbfredtmx = map->getObjectGroup("earbfred");
    this->earbfred = makeObject(EARBF_TAG, earbfredtmx, SpriteFrameCache::getInstance(), "earbfred", 1, 3, scale_map, xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);
    
    auto earbftmx = map->getObjectGroup("earbf");
    this->earbf = makeObject(EARBF_TAG, earbftmx, SpriteFrameCache::getInstance(), "earbf", 1, 3, scale_map, xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);
    for (auto sprite: earbf) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
        sprite->setOpacity(0);
    }
    this->scheduleUpdate();
    return true;
}

void Ears::ownEvent(){

}

Sprite *Ears::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    auto w = size.width;
    auto h = size.height;
    if (tag == DING_TAG) {
        auto sprite = Sprite::create("ding.png");
        auto current_scaleX = w / sprite->getContentSize().width;
        auto current_scaleY = h / sprite->getContentSize().height;
        sprite->setScale(current_scaleX, current_scaleY);
        return sprite;
    } else if (tag == EAR_TAG) {
        auto sprite = Sprite::create("ear.png");
        auto current_scaleX = w / sprite->getContentSize().width;
        auto current_scaleY = h / sprite->getContentSize().height;
        sprite->setScale(current_scaleX, current_scaleY);
        return sprite;
    }
    return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Ears::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Ears::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
        }
    }
}

bool Ears::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == EARBF_TAG or nodeB->getTag() == EARBF_TAG) {
            if (nodeA->getOpacity()== 255 and nodeB->getOpacity() == 255) {
                audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
                collisionWithEnemy(nodeA, nodeB);
            }
            else {
                return false;
            }
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() == EARFLOOR_TAG or nodeB->getTag() == EARFLOOR_TAG) {
            auto nm = nodeB->getName();
            if (nodeA->getTag() == EARFLOOR_TAG)
                nm = nodeA->getName();
            for (auto sprite : earbf) {
                if (sprite->getName() == nm) {
                    sprite->runAction(FadeTo::create(1.0f, 255));
                    sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
                    sprite->getPhysicsBody()->setGravityEnable(true);
                }
            }
            
            ears.at(stoi(nm) - 1)->runAction(Sequence::create(TintTo::create(0.5f, 243, 44, 239), TintTo::create(0.5, 255, 255, 255), NULL));
            
            return false;
        }
        else if (nodeA->getTag() == STOPFLOOR_TAG or nodeB->getTag() == STOPFLOOR_TAG) {
            auto nm = nodeB->getName();
            if (nodeA->getTag() == STOPFLOOR_TAG)
                nm = nodeA->getName();
            for (auto sprite : earbf) {
                if (sprite->getName() == nm) {
                    sprite->runAction(FadeTo::create(1.0f, 0));
                    sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
                    sprite->getPhysicsBody()->resetForces();
                    sprite->getPhysicsBody()->setGravityEnable(false);
                }
            }
            
            return false;
        }
        else if (nodeA->getTag() == DING_TAG or nodeB->getTag() == DING_TAG) {
            audio->playEffect("chime.wav", false, 2.0f, 0.0f, 1.0f);
            for (auto sprite : ears) {
                sprite->runAction(Sequence::create(TintTo::create(0.5f, 44, 255, 44), TintTo::create(0.5, 255, 255, 255), NULL));
            }
            for (auto sprite : earbf) {
                sprite->runAction(FadeTo::create(1.0f, 0));
                sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
                sprite->getPhysicsBody()->resetForces();
                sprite->getPhysicsBody()->setGravityEnable(false);
            }
            return false;
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);
            setNextLevelNum(11);
            isNewLevel = true;
            return false;
        }
        
    } /* else if ((nodeA->getTag() == TORT_TAG or nodeB->getTag() == TORT_TAG)  and
                (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG or
                 nodeA->getTag() == DOOR_TAG or nodeB->getTag() == DOOR_TAG or
                 nodeA->getTag() == BUTTON_TAG or nodeB->getTag() == BUTTON_TAG)){
                    if (nodeA->getTag() == TORT_TAG)
                        botsManager->changeDirection(nodeA->getName());
                    else
                        botsManager->changeDirection(nodeB->getName());
                    
                }*/
    
    return true;
}


void Ears::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);
/*        Sprite *tort;
        if (nodeA->getTag() == TORT_TAG)
            tort = torts.at(stoi(nodeA->getName()));
        else
            tort = torts.at(stoi(nodeB->getName()));
        
        int num = stoi(tort->getName());
        if (num % 3 == 0)
            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 250 - num*20, 255, 255), NULL));
        else if (stoi(tort->getName()) % 3 == 1)
            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 250 - num*20, 255), NULL));
        else
            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 255, 250 - num*20), NULL));
        
        
        */
        if (life_num == 0) {
            audio->playEffect("twang.wav", false, 2.0f, 0.0f, 1.0f);
            mysprite->runAction(TintTo::create(1.0f, 243, 44, 239));
            isRestart = true;
            stopTakingPoints();
            isPaused = false;
            pauseScene();
            pause();
        } else
            mysprite->runAction(Sequence::create(TintTo::create(0.5f, 243, 44, 239), TintTo::create(0.5, 255, 255, 255), NULL));
    }
}

void Ears::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: earbfred) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
        sprite->getPhysicsBody()->setGravityEnable(true);
    }
    for (auto sprite: earbf) {
        if (sprite->getOpacity() == 255) {
            sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
            sprite->getPhysicsBody()->setGravityEnable(true);
        }
    }
    
    resumeAllObjectLayer(earbf);
    resumeAllObjectLayer(earbfred);
    
    resumeAllObjectLayer(pluses);
}

void Ears::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: earbf) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    for (auto sprite: earbfred) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    pauseAllObjectLayer(earbf);
    pauseAllObjectLayer(earbfred);
    
    pauseAllObjectLayer(pluses);
}

void Ears::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: earbf) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    for (auto sprite: earbfred) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    stopAllObjectLayer(earbf);
    stopAllObjectLayer(earbfred);
    
    stopAllObjectLayer(pluses);
}

Scene* Ears::returnRestartedScene(){
    return Ears::createScene();
}

Scene* Ears::returnNewScene(){
    return Balance::createScene();
}


