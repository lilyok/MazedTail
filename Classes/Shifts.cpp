//
//  Shifts.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Shifts.h"
#include "MenuScene.h"
#include "Lifts.h"

USING_NS_CC;

#define FALLING_TAG 20

#define SHIFT_TAG 100


Scene *Shifts::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Shifts::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Shifts::init() {
    //////////////////////////////
    // 1. super init first

    if (!AbstractLabirint::init("shifts.tmx", "back3.png")) {
        return false;
    }
    

    
    TMXObjectGroup *shifttmx = map->getObjectGroup("shifts");
    this->shifts = makeObject(SHIFT_TAG, shifttmx, scale_map, xZero, yZero, BRICK, false, 0, 0);

    auto spidercache = SpriteFrameCache::getInstance();
    spidercache->addSpriteFramesWithFile("bf.plist");
    auto bfCount = 3;
    auto bfAnimSize = 2;
    
    TMXObjectGroup *fallings = map->getObjectGroup("butterfly");
    this->butterfly = makeObject(FALLING_TAG, fallings, spidercache, "bf", bfCount, bfAnimSize, scale_map,
                                xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);
    
    m_emitters = initFires();
    last_m_emitters = initFires();
    
    this->scheduleUpdate();
    return true;
}

Vector<ParticleSystemQuad*> Shifts::initFires() {
    Vector<ParticleSystemQuad*> m;
    for (auto i = 0; i < 14; i++) {
        auto m_emitter = ParticleFire::create();
        m_emitter->setScale(scale_map);
        
        m_emitter->stopSystem();
        m_emitter->setEndColor(Color4F(0, 120, 225, 120));
        if (i >= 7) {
            m_emitter->setGravity(Vec2(0, -150));
            m_emitter->setSpeed(-200);
        }
        else {
            m_emitter->setGravity(Vec2(0, 150));
            m_emitter->setSpeed(200);
        }
        addChild(m_emitter, 3);
        m.pushBack(m_emitter);
    }
    return m;
}

void Shifts::startFires(int i, Vector<ParticleSystemQuad*> m) {
    float w = visibleSize.width / 8;
    float h = 0;
    auto start = (7 - i) * w;
    int k = 0;
    long m_size = m.size() / 2;
    for (auto m_emitter : m){
        if (k >= m_size) {
            k = 0;
            h = visibleSize.height - h;
        }
        m_emitter->setPosition(Vec2(start + k*w/5, h)); //
        m_emitter->setScale(scale_map);
        m_emitter->resetSystem();
        k++;
    }
}

int Shifts::getShiftNum(Point p){
    float w = visibleSize.width / 8;
    for (auto i = 1; i <= 8; i++) {
        if (p.x < w*i)
            return 8 - i;
    }
    return 7;
}

void Shifts::ownEvent(){
    if (num_shift_delta < SHIFT_DELTA*scale_map) {
        num_shift_delta++;
    } else {
        auto h = map->getContentSize().height*scale_map;//visibleSize.height;
        int currentShift = getShiftNum(mysprite->getPosition());
        
        for (auto shift : shifts) {
            if (shift->getName() == std::__1::to_string((currentShift + 2)%8) or
                shift->getName() == std::__1::to_string((currentShift + 6)%8)){
                auto p = shift->getPosition();
                auto sh = shift->getContentSize().height*scale_map;
                
                    
                if (p.y - sh/2 >= h) {
                    p.y -= 2*h;
                    shift->setPosition(p);
                }
                shift->runAction(MoveTo::create(0.5, Vec2(p.x, p.y + h/5)));
                
                if (num_shift != currentShift) {
                    startFires((currentShift + 2)%8, m_emitters);
                    startFires((currentShift + 6)%8, last_m_emitters);
                    num_shift = currentShift;
                }
         
            }
        }
        num_shift_delta = 0;
    }
    if (num_spider_delta < BUTTERFLY_DELTA) {
        num_spider_delta++;
    } else {
        for (auto b : butterfly){
            b->getPhysicsBody()->resetForces();
            b->getPhysicsBody()->setVelocity(Vec2(0,0));
            float vx = 0.5 * (1 - rand() % 3);
            float vy = 1;
            b->getPhysicsBody()->applyImpulse(Vec2(Vec2(vx*scale_map, vy*scale_map)));
            
        }
        num_spider_delta = 0;
    }
}

Sprite *Shifts::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    Sprite * sprite;
    auto w = size.width;
    auto h = size.height;
    
    if (tag == SHIFT_TAG) {
        sprite = Sprite::create("greenwall.png");
        sprite->setOpacity(255);
        
        sprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
        
        sprite->setTextureRect(Rect(p.x, p.y, size.width, size.height));
        
        
        auto current_scaleX = w / sprite->getContentSize().width;
        auto current_scaleY = h / sprite->getContentSize().height;
        sprite->setScale(current_scaleX, current_scaleY);
        return sprite;
    } else
        return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Shifts::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Shifts::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
        }
    }
}

bool Shifts::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == FALLING_TAG or nodeB->getTag() == FALLING_TAG) {
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            isNewLevel = true;
            return false;
        }
        
    }
    return true;
}


void Shifts::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);
        Sprite *bf;
        if (nodeA->getTag() == FALLING_TAG)
            bf = butterfly.at(stoi(nodeA->getName()));
        else
            bf = butterfly.at(stoi(nodeB->getName()));
        
        int num = stoi(bf->getName());
        if (num % 3 == 0)
            bf->runAction(Sequence::create(TintTo::create(0.5f, 0, 255, 255), TintTo::create(0.5, 255, 255, 255), NULL));
        else if (stoi(bf->getName()) % 3 == 1)
            bf->runAction(Sequence::create(TintTo::create(0.5f, 255, 255, 0), TintTo::create(0.5, 255, 255, 255), NULL));
        else
            bf->runAction(Sequence::create(TintTo::create(0.5f, 0, 0, 255), TintTo::create(0.5, 255, 255, 255), NULL));
        
        
        
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
}

void Shifts::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: butterfly) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    
    resumeAllObjectLayer(butterfly);
    resumeAllObjectLayer(pluses);
}

void Shifts::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: butterfly) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(butterfly);
    pauseAllObjectLayer(pluses);
}

void Shifts::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: butterfly) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();        
    }
    stopAllObjectLayer(butterfly);
    stopAllObjectLayer(pluses);
}

Scene* Shifts::returnRestartedScene(){
    return Shifts::createScene();
}

Scene* Shifts::returnNewScene(){
    return Lifts::createScene();
}


