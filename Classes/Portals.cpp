//
//  Portals.cpp
//
//  Created by lilil on 08.01.15.
//
//
#include "Portals.h"
#include "Doors.h"

USING_NS_CC;
#define PORTAL_TAG 100
#define FALLING_TAG 20


Scene *Portals::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Portals::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Portals::init() {
    //////////////////////////////
    // 1. super init first
    
    
    if (!AbstractLabirint::init("portals.tmx", "back0.png")) {
        return false;
    }
    
    
    
    TMXObjectGroup *holes = map->getObjectGroup("portals");
    
    this->portals = makeObject(PORTAL_TAG, holes, scale_map, xZero, yZero, BRICK, false, 0, 0);
    
    
    auto spidercache = SpriteFrameCache::getInstance();
    spidercache->addSpriteFramesWithFile("spider.plist");
    auto spidersCount = 3;
    auto spidersAnimSize = 2;
    
    TMXObjectGroup *fallings = map->getObjectGroup("fallings");
    this->fallings = makeObject(FALLING_TAG, fallings, spidercache, "spider", spidersCount, spidersAnimSize, scale_map,
                                xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);
    
    
    //////////////////////////////////////////////////
    
    int i = 0;
    for (auto spider: this->fallings){
        auto h = spider->getContentSize().height;
        auto w = spider->getContentSize().width;
        spider->getPhysicsBody()->setPositionOffset(Vec2(0, h*scale_map/8));
        
        ParticleFire *p_emitter = ParticleFire::createWithTotalParticles(10);
        p_emitter->setEmitterMode(ParticleSystem::Mode::RADIUS);
        p_emitter->setStartRadius(0);
        p_emitter->setStartRadiusVar(fmin(h,w)/2);
        p_emitter->setEndRadiusVar(fmin(h,w)/2);
        
        auto j = i % 3;
        if (j == 0)
            p_emitter->setEndColor(Color4F(255, 0, 0, 255));
        else if (j == 1)
            p_emitter->setEndColor(Color4F(0, 0, 255, 255));
        else
            p_emitter->setEndColor(Color4F(255, 0, 255, 255));
        
        
        p_emitter->setScale(scale_map*3);
        p_emitter->setPosition(w/2, h/2);
        spider->addChild(p_emitter, 3);
        i++;
    }
    
    
    this->m_emitter = ParticleFire::create();
    this->m_emitter->setScale(scale_map / 2);
    this->m_emitter->stopSystem();
    addChild(m_emitter, 3);
    
    
    
    for (auto portal : this->portals) {
        ParticleSystemQuad *p_emitter = ParticleGalaxy::create();
        p_emitter->setScale(scale_map);
        p_emitter->setPosition(0, 0);
        portal->addChild(p_emitter, 3);
        reorderChild(portal, 3);
    }
    
    
    this->scheduleUpdate();
    return true;
}

void Portals::ownEvent(){
    if (num_spider_delta < SPIDER_DELTA) {
        num_spider_delta++;
    } else {
        for (auto spider : fallings){
            spider->getPhysicsBody()->resetForces();
            spider->getPhysicsBody()->setVelocity(Vec2(0,0));
            float vx = 0.5 * (1 - rand() % 3);
            float vy = 1;
            spider->getPhysicsBody()->applyImpulse(Vec2(Vec2(vx*scale_map, vy*scale_map)));
            
        }
        num_spider_delta = 0;
    }
}

Scene* Portals::returnRestartedScene(){
    return Portals::createScene();
}

Scene* Portals::returnNewScene(){
    return Doors::createScene();
}

void Portals::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        if ((touchX != NOTOUCH) && (touchY != NOTOUCH)) {
            goHero();
        }
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
            else if (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG) {
                m_emitter->stopSystem();
            } else if (nodeA->getTag() == PORTAL_TAG or nodeB->getTag() == PORTAL_TAG) {
                isPortal = true;
                stopTakingPoints();
            }
        }
        
        if (!isSpiderPortal) {
            if ((nodeA->getTag() == PORTAL_TAG || nodeB->getTag() == PORTAL_TAG) &&
                (nodeA->getTag() == FALLING_TAG || nodeB->getTag() == FALLING_TAG)) {
                isSpiderPortal = true;
                PhysicsBody *body;
                if (nodeA->getTag() == PORTAL_TAG)
                    body = contact.getShapeB()->getBody();
                else
                    body = contact.getShapeA()->getBody();
                if (body->getVelocity().x <= 0)
                    body->setVelocity(Vec2(100, 100));
                else
                    body->setVelocity(Vec2(-100, -100));
            }
        }
    }
}


bool Portals::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            if (nodeA->getTag() != PORTAL_TAG && nodeB->getTag() != PORTAL_TAG) {
                return checkCollision(contact, nodeA, nodeB);
            }
            else if (isPortal or isSpiderPortal) {
                entranceIntoPortal(nodeA, nodeB);
            }
        }
    }
    return true;
}

void Portals::entranceIntoPortal(Node *nodeA, Node *nodeB) {
    std::__1::string portal_name;
    Node *node;
    Node *current_portal;
    if (nodeA->getTag() == PORTAL_TAG) {
        portal_name = nodeA->getName();
        node = nodeB;
        current_portal = nodeA;
    }
    else {
        portal_name = nodeB->getName();
        node = nodeA;
        current_portal = nodeB;
    }
    
    node->runAction(Sequence::create(TintTo::create(0.0f, 0, 255, 144), TintTo::create(0.5, 255, 255, 255), NULL));
    long pos = portal_name.length() - 1;
    int num = 1;
    if (portal_name.at(pos) == '1') num = 2;
    std::__1::string res = portal_name.substr(0, pos).append(std::__1::to_string(num));
    Vec2 nextPortalPos = Vec2(0, 0);
    Vec2 dPos = Vec2(0, 0);
    for (auto portal : portals) {
        if (portal->getName() == res) {
            nextPortalPos = portal->getPosition();
            auto dPos = Vec2(nextPortalPos.x - node->getPosition().x, 0);
            
            if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
                audio->playEffect("bruitmetallique.wav", false, 3.0f, 0.0f, 1.0f);
                auto xDes = 0.0;
                auto yDes = 0.0;
                if (mysprite->getPositionX() - current_portal->getPositionX() != 0)
                    xDes = current_portal->getPositionX() - mysprite->getPositionX();
                if (mysprite->getPositionY() - current_portal->getPositionY() != 0)
                    yDes = current_portal->getPositionY() - mysprite->getPositionY();
                
                stopTakingPoints();
                isPortal = false;
                
                mysprite->setPosition(Vec2(nextPortalPos.x + 1.5*xDes,
                                           nextPortalPos.y + 1.5*yDes));
            }
            else if (isSpiderPortal) {
                isSpiderPortal = false;
                auto xDes = 0.0;
                auto yDes = 0.0;
                if (node->getPositionX() - current_portal->getPositionX() != 0)
                    xDes = current_portal->getPositionX() - node->getPositionX();
                if (node->getPositionY() - current_portal->getPositionY() != 0)
                    yDes = current_portal->getPositionY() - node->getPositionY();
                node->setPosition(Vec2(nextPortalPos.x + 1.5*xDes,
                                           nextPortalPos.y + 1.5*yDes));
            }
            break;
        }
    }
}

bool Portals::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == FALLING_TAG or nodeB->getTag() == FALLING_TAG) {
            audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() != NEWLEVEL_TAG and nodeB->getTag() != NEWLEVEL_TAG) {
            
            m_emitter->setPosition(contact.getContactData()->points[0]);
            m_emitter->setScale(scale_map);
            m_emitter->resetSystem();
        } else {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);

            isNewLevel = true;
            return false;
        }
        
    }
    
    return true;
}

void Portals::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);
        int colorint = 0;
        Sprite *falling;
        if (nodeA->getTag() == FALLING_TAG) {
            colorint = stoi(nodeA->getName()) % 3;
            falling = fallings.at(stoi(nodeA->getName()));
        }
        else {
            colorint = stoi(nodeB->getName()) % 3;
            falling = fallings.at(stoi(nodeB->getName()));
            
        }
        
        falling->runAction(Sequence::create(TintTo::create(0.5f, 44, 215, 243), TintTo::create(0.5, 255, 255, 255), NULL));
        
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


void Portals::resumeScene() {
    AbstractLabirint::resumeScene();
    
    for (auto sprite: fallings) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
        sprite->getPhysicsBody()->setGravityEnable(true);
    }
    
    resumeAllObjectLayer(fallings);
    resumeAllObjectLayer(pluses);
}

void Portals::pauseScene() {
    AbstractLabirint::pauseScene();
    
    for (auto sprite: fallings) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    pauseAllObjectLayer(fallings);
    pauseAllObjectLayer(pluses);
}

void Portals::stopScene() {
    AbstractLabirint::stopScene();
    
    for (auto sprite: fallings) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    stopAllObjectLayer(fallings);
    stopAllObjectLayer(pluses);
}

void Portals::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (!isRestart && !isNewLevel && !isRestarted && !isNewLeveled && isPortal && touchX != NOTOUCH && touchY != NOTOUCH) {
        touchX = touch->getLocation().x;
        touchY = touch->getLocation().y;
        goHero();
    } else {
        stopTakingPoints();
    }
}

