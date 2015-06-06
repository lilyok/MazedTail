//
//  Balance.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Balance.h"
USING_NS_CC;

#define BALANCER_TAG 20

#define UNKNOWN_TAG 105
#define UNLINE_TAG 100
#define MV_TAG 95


Scene *Balance::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Balance::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Balance::init() {
    //////////////////////////////
    // 1. super init first
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("balancer.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("unknown.plist");
    
    
    if (!AbstractLabirint::init("balance.tmx", "back11.png")) {
        return false;
    }
    
    botsManager = new BotsManager(xZero, yZero, map, "balancers", "balancer.plist", "balancer", scale_map, BALANCER_TAG);
    balancers = botsManager->getBotsSprites();
    for(auto balancer:balancers){
        AbstractLabirint::addChild(balancer, 2);
    }
    
    TMXObjectGroup *untmx = map->getObjectGroup("unknown");
    this->unknown = makeObject(UNKNOWN_TAG, untmx, scale_map, xZero, yZero, BRICK, false, 0, 0);
    for (auto u:unknown)
        u->setLocalZOrder(4);
    
    TMXObjectGroup *unlntmx = map->getObjectGroup("unlines");
    this->unlines = makeObject(UNLINE_TAG, unlntmx, scale_map, xZero, yZero, BRICK, false, 0, 0);
    for (auto u : this->unlines) {
        ParticleSystemQuad *p_emitter = ParticleGalaxy::create();
        p_emitter->setStartColor(Color4F(215, 255, 0, 255));
        p_emitter->setEndColor(Color4F(255, 0, 0, 255));
        p_emitter->setScale(scale_map);
        p_emitter->setPosition(0, 0);
        u->addChild(p_emitter, 3);
    }
    
    
    TMXObjectGroup *mvtmx = map->getObjectGroup("moving");
    this->movings = makeObject(MV_TAG, mvtmx, scale_map, xZero, yZero, BRICK, false, 0, 0);
    
    this->scheduleUpdate();
    return true;
}

void Balance::ownEvent(){
    mysprite->getPhysicsBody()->applyForce(myforce);
    if (num_bot_delta < BOT_DELTA) {
        num_bot_delta++;
    } else {
        botsManager->changeDirectionAll();
        num_bot_delta = 0;
    }
}

Sprite *Balance::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    Sprite * sprite;
    auto w = size.width;
    auto h = size.height;
    
    if (tag == UNKNOWN_TAG) {
        Vector<SpriteFrame *> animFogFrames;
        animFogFrames.reserve(2);
        animFogFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("unknown01.png"));
        animFogFrames.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("unknown02.png"));
        
        // create the animation out of the frames
        double randtime = (rand() % 5) * 0.1f + 0.3f;
        Animation *animationFog = Animation::createWithSpriteFrames(animFogFrames, randtime);// / scale_map);
        sprite = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("unknown01.png"));
        sprite->setScale(w/sprite->getContentSize().width, h/sprite->getContentSize().height);
        
        sprite->runAction(RepeatForever::create(Animate::create(animationFog)));
        sprite->setOpacity(255);
        return sprite;
    }
    return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Balance::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Balance::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
                isPlus = false;
            }  else if (nodeA->getTag() == MV_TAG or nodeB->getTag() == MV_TAG) {
                auto nm = nodeB->getName();
                if (nodeA->getTag() == MV_TAG)
                    nm = nodeA->getName();
                
                if (nm == "up")
                    myforce.y = 0;
                else if (nm == "down")
                    myforce.y = 0;
                else if (nm == "left")
                    myforce.x = 0;
                else if (nm == "right")
                    myforce.x = 0;
            }
        }
    }
}

bool Balance::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == BALANCER_TAG or nodeB->getTag() == BALANCER_TAG) {
            audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        } else if (nodeA->getTag() == MV_TAG or nodeB->getTag() == MV_TAG) {
            auto nm = nodeB->getName();
            if (nodeA->getTag() == MV_TAG)
                nm = nodeA->getName();
                
            if (nm == "up")
                myforce.y = MY_VELOCITY*scale_map;
            else if (nm == "down")
                myforce.y = -MY_VELOCITY*scale_map;
            else if (nm == "left")
                myforce.x = -MY_VELOCITY*scale_map;
            else if (nm == "right")
                myforce.x = MY_VELOCITY*scale_map;
            
            return false;
        } else if (nodeA->getTag() == UNLINE_TAG or nodeB->getTag() == UNLINE_TAG) {
            auto nm = nodeA->getName();
            auto node = nodeA;
            if (nodeB->getTag() == UNLINE_TAG) {
                nm = nodeB->getName();
                node = nodeB;
            }
            
            long size = unlines.size();
            if (num_of_portals < unlines.size() - 1)
                size -= 1;
            int randunknown = rand() % size;
            if (randunknown == stoi(nm))
                randunknown = (randunknown + 1) % size;
            
                
            auto mx = node->getPositionX() - mysprite->getPositionX();
            auto my = node->getPositionY() - mysprite->getPositionY();
            audio->playEffect("bruitmetallique.wav", false, 3.0f, 0.0f, 1.0f);
            mysprite->setPosition(Vec2(unlines.at(randunknown)->getPositionX() + mx, unlines.at(randunknown)->getPositionY() + my));
            
            num_of_portals++;
            return false;
        } else if (nodeA->getTag() == UNKNOWN_TAG or nodeB->getTag() == UNKNOWN_TAG) {
            return false;
        }

        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);
            setNextLevelNum(11);
            isNewLevel = true;
            return false;
        }
        
    } else if (nodeA->getTag() == UNKNOWN_TAG or nodeB->getTag() == UNKNOWN_TAG or
                 nodeA->getTag() == UNLINE_TAG or nodeB->getTag() == UNLINE_TAG or
                 nodeA->getTag() == MV_TAG or nodeB->getTag() == MV_TAG){
                    return false;
        
    }
    
    return true;
}


void Balance::collisionWithEnemy(Node *nodeA, Node *nodeB) {
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

void Balance::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: balancers) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    
    resumeAllObjectLayer(balancers);
    resumeAllObjectLayer(unknown);
    resumeAllObjectLayer(pluses);
}

void Balance::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: balancers) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(balancers);
    pauseAllObjectLayer(unknown);
    pauseAllObjectLayer(pluses);
}

void Balance::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: balancers) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    stopAllObjectLayer(balancers);
    stopAllObjectLayer(unknown);
    stopAllObjectLayer(pluses);
}

Scene* Balance::returnRestartedScene(){
    return Balance::createScene();
}

Scene* Balance::returnNewScene(){
    return NULL;//Shifts::createScene();
}


