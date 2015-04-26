//
//  Lifts.cpp
//  MazedTail
//
//  Created by lilil on 25.04.15.
//
//

#include "Lifts.h"
#include "MenuScene.h"
USING_NS_CC;

#define LIFT_TAG 100
#define BUTTON_TAG 105
#define FIRE_TAG 666
#define SNOWMAN_TAG 600

Scene *Lifts::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Lifts::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Lifts::init() {
    //////////////////////////////
    // 1. super init first
    
    if (!AbstractLabirint::init("lifts.tmx", "back4.png")) {
        return false;
    }
    
    TMXObjectGroup *btntmx = map->getObjectGroup("buttons");
    this->buttons = makeObject(BUTTON_TAG, btntmx, scale_map, xZero, yZero, BRICK, true, 0, 0);
    
    m_emitters = initFires();
    
    this->m_emitter = ParticleFire::create();
    this->m_emitter->setScale(scale_map / 2);
    m_emitter->setStartColor(Color4F(5, 5, 5, 155));
    m_emitter->setEndColor(Color4F(0, 0, 100, 255));
    this->m_emitter->stopSystem();
    addChild(m_emitter, 3);
    
    
    botsManager = new BotsManager(xZero, yZero, map, "snowman", "sm.plist", "snowman", scale_map, SNOWMAN_TAG);
    snowman = botsManager->getBotsSprites();
    for(auto s:snowman){
        s->getPhysicsBody()->setCategoryBitmask(0x0000000F);
        s->getPhysicsBody()->setContactTestBitmask(0x0000000F);
        s->getPhysicsBody()->setCollisionBitmask(0x0000000F);
        AbstractLabirint::addChild(s, 2);
    }
    
    TMXObjectGroup *lifttmx = map->getObjectGroup("lifts");
    this->lifts = makeObject(LIFT_TAG, lifttmx, scale_map, xZero, yZero, BRICK, true, 0, 0);
    setLiftStatus();
    
    for (auto l:lifts)
        l->setGlobalZOrder(4);
    
    this->scheduleUpdate();
    return true;
}

void Lifts::changeFireColor(ParticleSystemQuad* p, std::string name) {
    if (name == "bluered" or name == "thisbluered") {
        if (isBlue) {
            p->setStartColor(Color4F(5, 5, 255, 155));
            p->setEndColor(Color4F(0, 0, 255, 255));
        }
        else {
            p->setStartColor(Color4F(255, 5, 5, 155));
            p->setEndColor(Color4F(255, 0, 0, 255));
        }
    } else {
        if (isGreen) {
            p->setStartColor(Color4F(5, 255, 5, 155));
            p->setEndColor(Color4F(0, 255, 0, 255));
        }
        else {
            p->setStartColor(Color4F(255, 5, 255, 155));
            p->setEndColor(Color4F(255, 0, 255, 255));
        }
    }
}

void Lifts::changeFiresColor(std::string name) {
    for (auto p : m_emitters) {
        auto b_name = p->getName();
        if ((name == "bluered" or name == "thisbluered") and
            (b_name == "bluered" or b_name == "thisbluered")) {
            changeFireColor(p, name);
        } else if ((name == "pinkgreen" or name == "thispinkgreen") and
                  (b_name == "pinkgreen" or b_name == "thispinkgreen")) {
            changeFireColor(p, name);
        }
    }
}


Vector<ParticleSystemQuad*> Lifts::initFires() {
    Vector<ParticleSystemQuad*> m;
    for (auto button: buttons) {
        auto h = button->getContentSize().height;
        auto w = button->getContentSize().width;
        ParticleSun *p_emitter = ParticleSun::createWithTotalParticles(100);
        p_emitter->setEmitterMode(ParticleSystem::Mode::RADIUS);
        p_emitter->setStartRadius(0);
        p_emitter->setStartRadiusVar(fmax(h,w)*2);
        p_emitter->setEndRadiusVar(fmax(h,w)*2);
        p_emitter->setScale(scale_map);
        
      //  m_emitter->stopSystem();
        
        auto name = button->getName();
        changeFireColor(p_emitter, name);
        p_emitter->setPosition(w/2, h/2);
        p_emitter->setName(name);
        button->addChild(p_emitter, 3);
        
        m.pushBack(p_emitter);
    }
    return m;
}

void Lifts::ownEvent(){
    if (num_bot_delta < BOT_DELTA) {
        num_bot_delta++;
    } else {
        botsManager->changeDirectionAll();
        num_bot_delta = 0;
    }
}

void Lifts::setLiftStatus(){
    for (auto l : lifts){
        auto name = l->getName();
        if (name == "green") {
            if (!isGreen)
                l->setOpacity(200);
            else
                l->setOpacity(0);
            if ((isGreen && isHeroTop) || (!isGreen && !isHeroTop)) {
                l->getPhysicsBody()->setCollisionBitmask(0);
                l->getPhysicsBody()->setContactTestBitmask(0);
            } else {
                l->getPhysicsBody()->setCollisionBitmask(0xFFFFFFF0);
                l->getPhysicsBody()->setContactTestBitmask(0xFFFFFFF0);
            }
        } else if (name == "blue") {
            if (!isBlue)
                l->setOpacity(200);
            else
                l->setOpacity(0);
            if ((isBlue && isHeroTop) || (!isBlue && !isHeroTop)) {
                l->getPhysicsBody()->setCollisionBitmask(0);
                l->getPhysicsBody()->setContactTestBitmask(0);
            } else {
                l->getPhysicsBody()->setCollisionBitmask(0xFFFFFFF0);
                l->getPhysicsBody()->setContactTestBitmask(0xFFFFFFF0);
            }
        } else if (name == "red") {
            if (isBlue)
                l->setOpacity(200);
            else
                l->setOpacity(0);
            if ((!isBlue && isHeroTop) || (isBlue && !isHeroTop)) {
                l->getPhysicsBody()->setCollisionBitmask(0);
                l->getPhysicsBody()->setContactTestBitmask(0);
            } else {
                l->getPhysicsBody()->setCollisionBitmask(0xFFFFFFF0);
                l->getPhysicsBody()->setContactTestBitmask(0xFFFFFFF0);
            }
        } else if (name == "pink") {
            if (isGreen)
                l->setOpacity(200);
            else
                l->setOpacity(0);
            if ((!isGreen && isHeroTop) || (isGreen && !isHeroTop)) {
                l->getPhysicsBody()->setCollisionBitmask(0);
                l->getPhysicsBody()->setContactTestBitmask(0);
            } else {
                l->getPhysicsBody()->setCollisionBitmask(0xFFFFFFF0);
                l->getPhysicsBody()->setContactTestBitmask(0xFFFFFFF0);
            }
        }
        //l->setOpacity(200);
    }
}

Sprite *Lifts::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    Sprite * sprite;
    auto w = size.width;
    auto h = size.height;
    
    if (tag == LIFT_TAG || tag == BUTTON_TAG) {
        sprite = Sprite::create("black_pixel.png");
        sprite->setOpacity(0);
    } else {
        sprite = Sprite::create();
        sprite->setOpacity(0);
    }
    sprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
    
    sprite->setTextureRect(Rect(p.x, p.y, size.width, size.height));
    
    
    auto current_scaleX = w / sprite->getContentSize().width;
    auto current_scaleY = h / sprite->getContentSize().height;
    sprite->setScale(current_scaleX, current_scaleY);
    return sprite;
}


bool Lifts::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Lifts::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
            else if (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG or
                     nodeA->getTag() == LIFT_TAG or nodeB->getTag() == LIFT_TAG) {
                m_emitter->stopSystem();
            }
        }
    }
}

bool Lifts::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == SNOWMAN_TAG or nodeB->getTag() == SNOWMAN_TAG) {
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == BUTTON_TAG or nodeB->getTag() == BUTTON_TAG) {
            auto name = nodeB->getName();
            if (nodeA->getTag() == BUTTON_TAG)
                name = nodeA->getName();
            if (name == "bluered" or name == "thisbluered") {
                isBlue = !isBlue;
            } else {
                isGreen = !isGreen;
            }
            if (name == "thisbluered" or name == "thispinkgreen")
                isHeroTop = !isHeroTop;
            
            setLiftStatus();
            changeFiresColor(name);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG or
                 nodeA->getTag() == LIFT_TAG or nodeB->getTag() == LIFT_TAG) {
            
            m_emitter->setPosition(contact.getContactData()->points[0]);
            m_emitter->setScale(scale_map);
            m_emitter->resetSystem();
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            isNewLevel = true;
            return false;
        }
        
    }
    return true;
}


void Lifts::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);
        Sprite *sm;
        if (nodeA->getTag() == SNOWMAN_TAG)
            sm = snowman.at(stoi(nodeA->getName()));
        else
            sm = snowman.at(stoi(nodeB->getName()));
        
        int num = stoi(sm->getName());
        if (stoi(sm->getName()) % 3 == 0)
            sm->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 250 - num*20, 255, 255), NULL));
        else if (stoi(sm->getName()) % 3 == 1)
            sm->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 250 - num*20, 255), NULL));
        else
            sm->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 255, 250 - num*20), NULL));
        
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

void Lifts::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: snowman) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    
    resumeAllObjectLayer(snowman);
    resumeAllObjectLayer(pluses);
}

void Lifts::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: snowman) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(snowman);
    pauseAllObjectLayer(pluses);
}

void Lifts::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: snowman) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    stopAllObjectLayer(snowman);
    stopAllObjectLayer(pluses);
}

Scene* Lifts::returnRestartedScene(){
    return Lifts::createScene();
}

Scene* Lifts::returnNewScene(){
    return Lifts::createScene();
}
