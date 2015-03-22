//
//  Created by lilil on 08.01.15.
//
//
#include "Doors.h"

#include "MenuScene.h"

USING_NS_CC;
#define BUNNY_TAG 20
#define DOOR_TAG 100
#define BUTTON_TAG 105

#define FIRE_TAG 15




Scene *Doors::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Doors::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Doors::init() {
    //////////////////////////////
    // 1. super init first
    
    
    if (!AbstractLabirint::init("doors.tmx", "back1.png")) {
        return false;
    }
  
    TMXObjectGroup *buttontmx = map->getObjectGroup("buttons");
    
    this->buttons = makeObject(BUTTON_TAG, buttontmx, scale_map, xZero, yZero, BRICK, 0, 0);
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("color_buttons.plist");
    
    auto btn_scale = buttons.at(0)->getContentSize().width /
    SpriteFrameCache::getInstance()->getSpriteFrameByName("blue_normal.png")->getOriginalSize().width;
    for (auto button: buttons){
        auto name = button->getName() + "_normal.png";
        button->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(name));
        button->setOpacity(255);
        button->setScale(btn_scale);

    }

    TMXObjectGroup *doortmx = map->getObjectGroup("doors");
    
    this->doors = makeObject(DOOR_TAG, doortmx, scale_map, xZero, yZero, BRICK, 0, 0);
    for (auto door : this->doors) {
        auto w = door->getContentSize().width;
        //auto h = door->getContentSize().height;

        auto p_emitter = ParticleFire::createWithTotalParticles(60);
        p_emitter->setTag(FIRE_TAG);
        auto name = door->getName();
        if (name == "red")
            p_emitter->setEndColor(Color4F(220, 0, 0, 255));
        else if (name == "orange")
            p_emitter->setEndColor(Color4F(255, 220, 0, 255));
        else if (name == "green")
            p_emitter->setEndColor(Color4F(0, 255, 0, 255));
        else if (name == "blue")
            p_emitter->setEndColor(Color4F(0, 255, 255, 255));
        else if (name == "pink")
            p_emitter->setEndColor(Color4F(255, 0, 255, 255));



        p_emitter->setScale(scale_map*2);
        p_emitter->setPosition(w/2, 0);//h/2);
       // p_emitter->setGravity(Vec2(0, -30));
        p_emitter->setTag(FIRE_TAG);
        door->addChild(p_emitter, 3);
        reorderChild(door, 3);
    }
    
    
    botsManager = new BotsManager(xZero, yZero, map, "bunny", "bunny.plist", "bunny_", scale_map, BUNNY_TAG);
    bunnies = botsManager->getBotsSprites();
    for(auto bunny:bunnies){
        AbstractLabirint::addChild(bunny, 2);
    }
    
    this->m_emitter = ParticleFire::create();
    this->m_emitter->setScale(scale_map / 2);
    this->m_emitter->stopSystem();
    addChild(m_emitter, 3);
 
    
    this->scheduleUpdate();
    return true;
}

Scene* Doors::returnRestartedScene(){
    return Doors::createScene();
}

Scene* Doors::returnNewScene(){
    return Doors::createScene();
}

void Doors::ownEvent(){
    if (num_bot_delta < BOT_DELTA) {
        num_bot_delta++;
    } else {
        botsManager->changeDirectionAll();
        num_bot_delta = 0;
    }
}

bool Doors::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Doors::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {

        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
            else if (nodeA->getTag() != NEWLEVEL_TAG and nodeB->getTag() != NEWLEVEL_TAG) {
                m_emitter->stopSystem();
            }
        }
    }
}

bool Doors::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == BUNNY_TAG or nodeB->getTag() == BUNNY_TAG) {
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        } else if (nodeA->getTag() == BUTTON_TAG or nodeB->getTag() == BUTTON_TAG) {
            auto btn_name = nodeB->getName();
            if (nodeA->getTag() == BUTTON_TAG)
                btn_name = nodeA->getName();
            
            for (auto btn : buttons) {
                if (btn->getName() == btn_name)
                    btn->setSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(btn_name+"_selected.png"));
            }

            for (auto door : doors) {
                if (door->getName() == btn_name) {
                    door->removeChildByTag(FIRE_TAG);
                    door->getPhysicsBody()->setCollisionBitmask(0);
                    door->getPhysicsBody()->setContactTestBitmask(0);
                }
            }
            
        }
        else if (nodeA->getTag() != NEWLEVEL_TAG and nodeB->getTag() != NEWLEVEL_TAG) {
            m_emitter->setPosition(contact.getContactData()->points[0]);
            m_emitter->setScale(scale_map);
            m_emitter->resetSystem();
        } else {
            isNewLevel = true;
            return false;
        }
        
    } else if ((nodeA->getTag() == BUNNY_TAG or nodeB->getTag() == BUNNY_TAG)  and
               (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG or
                nodeA->getTag() == DOOR_TAG or nodeB->getTag() == DOOR_TAG or
                nodeA->getTag() == BUTTON_TAG or nodeB->getTag() == BUTTON_TAG)){
        if (nodeA->getTag() == BUNNY_TAG)
            botsManager->changeDirection(nodeA->getName());
        else
            botsManager->changeDirection(nodeB->getName());

    }
    
    
    return true;
}

void Doors::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);
        Sprite *bunny;
        if (nodeA->getTag() == BUNNY_TAG)
            bunny = bunnies.at(stoi(nodeA->getName()));
        else
            bunny = bunnies.at(stoi(nodeB->getName()));

        int num = stoi(bunny->getName());
        if (stoi(bunny->getName()) % 3 == 0)
            bunny->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 250 - num*20, 255, 255), NULL));
        else if (stoi(bunny->getName()) % 3 == 1)
            bunny->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 250 - num*20, 255), NULL));
        else
            bunny->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 255, 250 - num*20), NULL));
        
        
        
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


void Doors::resumeScene() {
    AbstractLabirint::resumeScene();
    
    for (auto sprite: bunnies) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    
    resumeAllObjectLayer(bunnies);
    resumeAllObjectLayer(pluses);
}

void Doors::pauseScene() {
    AbstractLabirint::pauseScene();
    
    for (auto sprite: bunnies) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
    }
    pauseAllObjectLayer(bunnies);
    pauseAllObjectLayer(pluses);
}

void Doors::stopScene() {
    AbstractLabirint::stopScene();
    
    for (auto sprite: bunnies) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
    }
    stopAllObjectLayer(bunnies);
    stopAllObjectLayer(pluses);
}


