//
//  Hands.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Hands.h"
#include "Noses.h"
USING_NS_CC;

#define TORT_TAG 20

#define PUSHPIN_TAG 105
#define HAND_TAG 100
#define INSHAND_TAG 110

Scene *Hands::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Hands::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Hands::init() {
    //////////////////////////////
    // 1. super init first
    if (!AbstractLabirint::init("hands.tmx", "back7.png")) {
        return false;
    }
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("inshand.plist");
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("handlist.plist");
    Vector<SpriteFrame *> animHand;
    animHand.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hand01.png"));
    animHand.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hand02.png"));
    animHand.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hand03.png"));
    animHand.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hand04.png"));
    
    
    
    // create the animation out of the frames
    auto animationHand = Animation::createWithSpriteFrames(animHand, 0.05);
    animateHand = Animate::create(animationHand);
    animateHand->retain();
    
    TMXObjectGroup *pushpintmx = map->getObjectGroup("pushpin");
    this->pushpin = makeObject(PUSHPIN_TAG, pushpintmx, scale_map, xZero, yZero, BALL, true, 0, 0, 0xFFFFF000);
    for (auto pin : pushpin) {
        pin->getPhysicsBody()->resetForces();
        pin->getPhysicsBody()->setGravityEnable(false);
        pin->getPhysicsBody()->setMass(0.001);
        pin->getPhysicsBody()->setLinearDamping(10.0f);
    }
    
    TMXObjectGroup *handtmx = map->getObjectGroup("hands");
    this->hands = makeObject(HAND_TAG, handtmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0xFFFFF000);
    for (auto hand : hands) {
        reorderChild(hand, 3);
    }
    
    TMXObjectGroup *inshandtmx = map->getObjectGroup("inshand");
    this->inshand = makeObject(INSHAND_TAG, inshandtmx, SpriteFrameCache::getInstance(), "inshand", 1, 3, scale_map, xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);
    this->scheduleUpdate();
    return true;
}

void Hands::ownEvent(){
    if (cur_hand && cur_hand->getOpacity() == 253) {
        audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
        collisionWithEnemy(cur_hand, mysprite);
        cur_hand = NULL;
    }
    if (num_ins_delta < INS_DELTA) {
        num_ins_delta++;
    } else {
        for (auto ins : inshand){
            ins->getPhysicsBody()->resetForces();
            ins->getPhysicsBody()->setVelocity(Vec2(0,0));
            float vx = 0.5 * (1 - rand() % 3);
            float vy = 1;
            ins->getPhysicsBody()->applyImpulse(Vec2(Vec2(vx*scale_map, vy*scale_map)));
        }
        num_ins_delta = 0;
    }
}

Sprite *Hands::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    auto w = size.width;
    auto h = size.height;
    if (tag == PUSHPIN_TAG) {
        auto sprite = Sprite::create("pushpin.png");
        auto current_scaleX = w / sprite->getContentSize().width;
        auto current_scaleY = h / sprite->getContentSize().height;
        sprite->setScale(current_scaleX, current_scaleY);
        return sprite;
    }else if (tag == HAND_TAG) {
        auto sprite = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hand01.png"));
        sprite->setScale(2*w/sprite->getContentSize().width, h/sprite->getContentSize().height);
        
        if (sprite_name == "top") {
            sprite->setRotation(180);
        }
        
        sprite->setPosition(p);
        return sprite;
    } else
    return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Hands::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Hands::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
        }
        if (nodeA->getTag() == HAND_TAG or nodeB->getTag() == HAND_TAG) {
            auto node = nodeA;
            if(nodeB->getTag() == HAND_TAG)
                node = nodeB;
            node->stopAllActions();
            node->runAction(Sequence::create(TintTo::create(0.5, 255, 255, 255), FadeTo::create(0, 255), NULL));
        }
    }
}

bool Hands::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == HAND_TAG or nodeB->getTag() == HAND_TAG) {
            auto node = nodeA;
            if (nodeB->getTag() == HAND_TAG)
                node = nodeB;
            
            if (node->getOpacity() == 255 && node->getNumberOfRunningActions() <= 0) {
                node->runAction(Sequence::create(animateHand, FadeTo::create(0, 253), animateHand->reverse(), FadeTo::create(0, 254), NULL));
                cur_hand = node;
            }
            return false;
        } else if (nodeA->getTag() == INSHAND_TAG or nodeB->getTag() == INSHAND_TAG) {
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);
            setNextLevelNum(8);
            isNewLevel = true;
            return false;
        }
        
    } else if (nodeA->getTag() == HAND_TAG or nodeB->getTag() == HAND_TAG) {
        auto node = nodeA;
        if(nodeB->getTag() == HAND_TAG)
            node = nodeB;
        if (node->getOpacity() == 255 && node->getNumberOfRunningActions() <= 0) {
            node->runAction(Sequence::create(animateHand, animateHand->reverse(), NULL));
            node->runAction(Sequence::create(TintTo::create(0.5f, 243, 44, 239), FadeTo::create(0, 254), NULL));
            audio->playEffect("pain.wav", false, 0.5f, 0.0f, 1.0f);
        }
        return false;
    }
    
    return true;
}


void Hands::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);

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

void Hands::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: inshand) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
        sprite->getPhysicsBody()->setGravityEnable(true);
    }
    resumeAllObjectLayer(inshand);
    resumeAllObjectLayer(hands);
    resumeAllObjectLayer(pluses);
}

void Hands::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: pushpin) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    for (auto sprite: inshand) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    pauseAllObjectLayer(inshand);
    pauseAllObjectLayer(hands);
    pauseAllObjectLayer(pluses);
}

void Hands::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: pushpin) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    for (auto sprite: inshand) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    pauseAllObjectLayer(inshand);
    stopAllObjectLayer(hands);
    stopAllObjectLayer(pluses);
}

Scene* Hands::returnRestartedScene(){
    return Hands::createScene();
}

Scene* Hands::returnNewScene(){
    return Noses::createScene();
}


