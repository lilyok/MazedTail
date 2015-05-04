//
//  Holes.cpp
//  MazedTail
//
//  Created by lilil on 25.04.15.
//
//

#include "Holes.h"
#include "MenuScene.h"
USING_NS_CC;

#define HOLE_TAG 110
#define RABBIT_TAG 600
#define HOLER_TAG 666
#define JUMP_TAG 777
#define STOPSTART_TAG 888

Scene *Holes::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Holes::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Holes::init() {
    //////////////////////////////
    // 1. super init first
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("hole.plist");
    if (!AbstractLabirint::init("hole.tmx", "back5.png")) {
        return false;
    }
    
    TMXObjectGroup *holetmx = map->getObjectGroup("holes");
    this->holes = makeObject(HOLE_TAG, holetmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0);
    
    TMXObjectGroup *rabbittmx = map->getObjectGroup("rabbit");
    this->rabbits = makeObject(RABBIT_TAG, rabbittmx, scale_map, xZero, yZero, BRICK, true, 0, 0);
    auto rsize = rabbits.size();
    for (auto i = 1; i <= rsize; i++) {
        isA.push_back(false);
        if (i % rsize == 0)
            rabbits.at(i-1)->runAction(TintTo::create(0.5, 255, 255, 255));
        else if (i % rsize == 1)
            rabbits.at(i-1)->runAction(TintTo::create(0.5, 0, 255, 255));
        else if (i % rsize == 2)
            rabbits.at(i-1)->runAction(TintTo::create(0.5, 0, 0, 255));
        else if (i % rsize == 3)
            rabbits.at(i-1)->runAction(TintTo::create(0.5, 255, 0, 255));
        else if (i % rsize == 4)
            rabbits.at(i-1)->runAction(TintTo::create(0.5, 255, 255, 0));
        else if (i % rsize == 5)
            rabbits.at(i-1)->runAction(TintTo::create(0.5, 255, 0, 0));
        else if (i % rsize == 6)
            rabbits.at(i-1)->runAction(TintTo::create(0.5, 0, 255, 0));
        rabbits.at(i-1)->getPhysicsBody()->setMass(0.5);
      //  rabbits.at(i-1)->getPhysicsBody()->setRotationEnable(true);
    }
    
    
    this->m_emitter = ParticleFire::create();
    this->m_emitter->setScale(scale_map / 2);
   // m_emitter->setStartColor(Color4F(0, 0, 255, 255));
    m_emitter->setEndColor(Color4F(0, 0, 255, 255));
    this->m_emitter->stopSystem();
    addChild(m_emitter, 3);

    
    this->g_emitter = ParticleGalaxy::create();
    this->g_emitter->setScale(scale_map / 2);
    this->g_emitter->stopSystem();
    addChild(g_emitter, 3);

    
    this->scheduleUpdate();
    return true;
}


void Holes::ownEvent(){
    auto i = 0;
    if (mysprite->getOpacity() == 253) {
        mysprite->setOpacity(255);
        mysprite->getPhysicsBody()->resetForces();
        
        g_emitter->stopSystem();
    }
    
    for (auto r : rabbits){
        if (r->getOpacity() != 255) {
            r->getPhysicsBody()->resetForces();
            r->getPhysicsBody()->setGravityEnable(false);
            r->getPhysicsBody()->setVelocity(Vec2(0,0));
        }
        if (r->getOpacity() == 0) {
            auto name = r->getName();
            auto new_hole_name = hole_name;
            if (isA.at(i)) {
                name += "b";
                new_hole_name += "b";
            }
            else {
                name += "a";
                new_hole_name += "a";
            }
            
            isA[i] = !isA.at(i);

            for (auto h: holes) {
                if (h->getName() == name) {
                    r->setPosition(h->getPosition());
                    r->setOpacity(1);
                    if (name == new_hole_name) {
                        mysprite->setPosition(h->getPosition());
                        mysprite->runAction(FadeTo::create(5.0f, 253));
                        g_emitter->setPosition(h->getPosition());
                        g_emitter->setScale(scale_map);
                        g_emitter->resetSystem();
                        hole_name = "";
                    }
                }
            }
            
        }
        else if(r->getOpacity() == 254) {
            r->getPhysicsBody()->resetForces();
            r->getPhysicsBody()->setGravityEnable(true);
            r->getPhysicsBody()->setMass(0.001);
            r->getPhysicsBody()->setVelocityLimit(MY_VELOCITY*scale_map);
            float vx = 0.2*(rand()%3 - 2);
            float vy = 0.2;

            r->getPhysicsBody()->applyImpulse(Vec2(vx, vy));
            r->setOpacity(255);
        }

        i++;

    }
}

Sprite *Holes::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    Sprite * sprite;
    auto w = size.width;
    auto h = size.height;
    
    if (tag == RABBIT_TAG) {
        Vector<SpriteFrame *> animStartRH;
        Vector<SpriteFrame *> animJumpRH;
        Vector<SpriteFrame *> animStopRH;
        Vector<SpriteFrame *> animHole;
        Vector<SpriteFrame *> animStartStop;
    
        animStartRH.reserve(5);
        animJumpRH.reserve(4);
        animStopRH.reserve(4);
        animHole.reserve(2);
        animStartStop.reserve(1);
        
        animStartRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole01.png"));
        animStartRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole02.png"));
        animStartRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole03.png"));
        animStartRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole04.png"));
        animStartRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole05.png"));

        animJumpRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole06.png"));
        animJumpRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole07.png"));
        animJumpRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole08.png"));
        animJumpRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole07.png"));
        
        animStopRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole09.png"));
        animStopRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole10.png"));
        animStopRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole11.png"));
        animStopRH.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole12.png"));
        
        animHole.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole13.png"));
        animHole.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole14.png"));
        
        animStartStop.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole01.png"));
        
        
        // create the animation out of the frames
        double randtime = (rand() % 5) * 0.1f + 0.3f;
        Animation *animationStart = Animation::createWithSpriteFrames(animStartRH, 0.1f);
        Animation *animationJump = Animation::createWithSpriteFrames(animJumpRH, randtime);
        Animation *animationStop = Animation::createWithSpriteFrames(animStopRH, 0.1f);
        Animation *animationHole = Animation::createWithSpriteFrames(animHole, 0.2f);
        
        auto stopstart = Animate::create(Animation::createWithSpriteFrames(animStartStop, 0.0001f));
        stopstart->setTag(STOPSTART_TAG);
        auto jumping = Animate::create(animationJump);
        jumping->setTag(JUMP_TAG);
        auto holer = Animate::create(animationHole);
        holer->setTag(HOLER_TAG);
        
        sprite = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hole01.png"));
        sprite->setScale(w/sprite->getContentSize().width, h/sprite->getContentSize().height);
        
        sprite->setOpacity(0);
        sprite->runAction(RepeatForever::create(Sequence::create(
                                                                 RotateTo::create(0.0f, 0),
                                                                 RotateTo::create(2.1f, 0),
                                                                 RotateBy::create(6.0f, 40*(randtime - 0.5)),NULL)));
        sprite->runAction(RepeatForever::create(
                                               Sequence::create(
                                               RotateTo::create(0.0f, 0),
                                               FadeTo::create(1.0f, 253),
                                               Animate::create(animationStart),
                                               FadeTo::create(0.0f, 254),
                                               Repeat::create(Animate::create(animationJump), 3),
                                               FadeTo::create(0.0f, 253),
                                               Animate::create(animationStop),
                                               FadeTo::create(0.0f, 252),
                                               Repeat::create(holer, 10),
                                               FadeTo::create(1.0f, 0),
                                               stopstart,
                                               RotateBy::create(2.0f, 0),
                                               NULL)));
    } else {
        return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
    }
    return sprite;
}


bool Holes::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Holes::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
            else if (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG)
                m_emitter->stopSystem();
        }
    }
}

bool Holes::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == RABBIT_TAG or nodeB->getTag() == RABBIT_TAG) {
            collisionWithEnemy(nodeA, nodeB);
            if (nodeA->getOpacity() < 254 or nodeB->getOpacity() < 254) return false;
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG) {
            m_emitter->setPosition(contact.getContactData()->points[0]);
            m_emitter->setScale(scale_map);
            m_emitter->resetSystem();
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            isNewLevel = true;
            return false;
        }
        
    }
//    if (nodeA->getTag() == HOLE_TAG or nodeB->getTag() == HOLE_TAG) {
//        return false;
//    }
    return true;
}


void Holes::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (nodeA->getOpacity() > 253 and nodeB->getOpacity() > 253) {
        if (life_num > 0) {
            life_num--;
            
            char *res = new char[50];
            sprintf(res, "life%i.png", life_num);
            SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
            mylife->setSpriteFrame(sp);
     
            if (life_num == 0) {
                mysprite->runAction(TintTo::create(1.0f, 243, 44, 239));
                isRestart = true;
                stopTakingPoints();
                isPaused = false;
                pauseScene();
                pause();
            } else
                mysprite->runAction(Sequence::create(TintTo::create(0.5f, 243, 44, 239), TintTo::create(0.5, 255, 255, 255), NULL));
        }
    } else if (nodeA->getOpacity() == 252 or nodeB->getOpacity() == 252) {
       // mysprite->pause();
        hole_name = nodeA->getName();
        auto pos = nodeA->getPosition();
        if (nodeA->getTag() == HERO_SPRITE_TAG) {
            pos = nodeB->getPosition();
            hole_name = nodeB->getName();
        }
        stopTakingPoints();
        mysprite->setPosition(pos);
        mysprite->runAction(FadeTo::create(1.0f, 0));
        
        g_emitter->setPosition(pos);
        g_emitter->setScale(scale_map);
        g_emitter->resetSystem();
        
        
    }
}

void Holes::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: rabbits) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
        sprite->getPhysicsBody()->setGravityEnable(true);
    }
    resumeAllObjectLayer(rabbits);
    resumeAllObjectLayer(pluses);
}

void Holes::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: rabbits) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    pauseAllObjectLayer(rabbits);
    pauseAllObjectLayer(pluses);
}

void Holes::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: rabbits) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    stopAllObjectLayer(rabbits);
    stopAllObjectLayer(pluses);
}

Scene* Holes::returnRestartedScene(){
    return Holes::createScene();
}

Scene* Holes::returnNewScene(){
    return Holes::createScene();
}

bool Holes::onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (hole_name == "")
        return AbstractLabirint::onTouchBegan(touch, event);
    return false;
}

void Holes::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (hole_name == "")
        AbstractLabirint::onTouchMoved(touch, event);
}

void Holes::onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (hole_name == "")
        AbstractLabirint::onTouchEnded(touch, event);
}

void Holes::onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (hole_name == "")
        AbstractLabirint::onTouchCancelled(touch, event);
}






