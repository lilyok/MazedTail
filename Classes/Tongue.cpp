//
//  Tongue.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Tongue.h"
#include "Ears.h"
USING_NS_CC;

#define TORT_TAG 20

#define TONGUE_TAG 110
#define BULLET_TAG 105
#define CANNON_TAG 100


Scene *Tongue::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Tongue::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Tongue::init() {
    //////////////////////////////
    // 1. super init first
   // SpriteFrameCache::getInstance()->addSpriteFramesWithFile("flower.plist");
    
    
    if (!AbstractLabirint::init("lips.tmx", "back9.png")) {
        return false;
    }
    TMXObjectGroup *eyestmx = map->getObjectGroup("cannon");
    this->cannons = makeObject(CANNON_TAG, eyestmx, scale_map, xZero, yZero, BRICK, true, 0, 0, 0xFFFFF000);
    for (auto c : cannons) {
        c->getPhysicsBody()->setGravityEnable(false);
        c->getPhysicsBody()->setRotationEnable(true);
        c->setRotation(180);
        c->getPhysicsBody()->setMoment(100);
    }
    
    botsManager = new BotsManager(xZero, yZero, map, "tongue", "tonguelist.plist", "tonguelist", scale_map, TONGUE_TAG);
    tongue = botsManager->getBotsSprites();
    for(auto t:tongue){
        AbstractLabirint::addChild(t, 2);
    }
    
    
    this->scheduleUpdate();
    return true;
}

void Tongue::ownEvent(){
    for (auto c : cannons) {
        auto nm = c->getName();
        auto dv = 70*scale_map;
        Vec2 v = Vec2(0, dv);
        auto r = c->getPhysicsBody()->getRotation();
        while (r < 0) r = 360 + r;
        v.rotate(Vec2(0,0), 0 + r);
        bulletv[stoi(nm)] = v;
        
    }
}

Sprite *Tongue::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    auto w = size.width;
    auto h = size.height;
    if (tag == CANNON_TAG) {
        auto sprite = Sprite::create("cannon.png");
        auto current_scaleX = w / sprite->getContentSize().width;
        auto current_scaleY = h / sprite->getContentSize().height;
        sprite->setScale(current_scaleX, current_scaleY);
        
        auto dv = 70*scale_map;
        Vec2 v = Vec2(0, dv);
        bulletv.push_back(v);
        
        auto bullet = Sprite::create("lemon.png");
        char *bname = new char[50];
        sprintf(bname, "%zi", bulletpositions.size());
        bullet->setName(bname);
        bulletpositions.push_back(p);
        bullet->setScale(w/2/bullet->getContentSize().width);
        bullet->setPosition(p);
        bullet->setTag(BULLET_TAG);
        auto body = PhysicsBody::createCircle(w / 4.0,
                                              PhysicsMaterial(0.1f, 0.0f, 0.0f));
        body->setGravityEnable(false);
        body->setDynamic(true);
        body->setMass(0.1);
        body->setContactTestBitmask(0x0000FFFF);
        body->setCollisionBitmask(0x000000FF);
        body->setRotationEnable(false);
        bullet->setPhysicsBody(body);
        bullet->getPhysicsBody()->applyForce(v);
        bullet->setOpacity(0);
        bullet->runAction(FadeTo::create(1, 255));
 
        bullets.pushBack(bullet);
        
        addChild(bullet, 3);

        
        return sprite;
    } else
    return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Tongue::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Tongue::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
        }
    }
}

bool Tongue::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == TONGUE_TAG or nodeB->getTag() == TONGUE_TAG) {
            audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);
            setNextLevelNum(10);
            isNewLevel = true;
            return false;
        }
        
    } else if (nodeA->getTag() == BULLET_TAG or nodeB->getTag()== BULLET_TAG)  {
        if (nodeA->getTag() == CANNON_TAG or nodeB->getTag()== CANNON_TAG) {
            return false;
        } else {
            auto node = nodeB;
            if (nodeA->getTag() == BULLET_TAG)
                node = nodeA;
                
            node->setOpacity(0);
            node->runAction(FadeTo::create(1, 255));
            node->getPhysicsBody()->resetForces();
            node->getPhysicsBody()->setVelocity(Vec2(0, 0));
            node->getPhysicsBody()->applyForce(bulletv.at(stoi(node->getName())));
            node->setPosition(bulletpositions.at(stoi(node->getName())));
            
            if (nodeA->getTag() == TONGUE_TAG) {
                nodeA->runAction(Sequence::create(TintTo::create(0.5f, 100, 0, 100), TintTo::create(0.5, 255, 255, 255), NULL));
                botsManager->changeDirection(nodeA->getName());
            }
            else if (nodeB->getTag() == TONGUE_TAG) {
                nodeB->runAction(Sequence::create(TintTo::create(0.5f, 100, 0, 100), TintTo::create(0.5, 255, 255, 255), NULL));
                botsManager->changeDirection(nodeB->getName());
            }
        }
    } else if ((nodeA->getTag() == CANNON_TAG or nodeB->getTag()== CANNON_TAG) and
               (nodeA->getTag() == COLLISION_TAG or nodeB->getTag()== COLLISION_TAG)) {
        return false;
    } else if (nodeA->getTag() == TONGUE_TAG or nodeB->getTag()== TONGUE_TAG) {
        if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            return false;
        }
        else if (nodeA->getTag() == TONGUE_TAG)
            botsManager->changeDirection(nodeA->getName());
        else if (nodeB->getTag() == TONGUE_TAG)
            botsManager->changeDirection(nodeB->getName());
    }

    return true;
}


void Tongue::collisionWithEnemy(Node *nodeA, Node *nodeB) {
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

void Tongue::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: tongue) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    auto i = 0;
    for (auto sprite: bullets) {
        sprite->getPhysicsBody()->applyForce(bulletv.at(i));
        i++;
    }
    resumeAllObjectLayer(tongue);
    resumeAllObjectLayer(pluses);
}

void Tongue::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: tongue) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    for (auto sprite: bullets) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    for (auto sprite: cannons) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(tongue);
    pauseAllObjectLayer(pluses);
}

void Tongue::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: tongue) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    for (auto sprite: bullets) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    for (auto sprite: cannons) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    stopAllObjectLayer(tongue);
    stopAllObjectLayer(pluses);
}

Scene* Tongue::returnRestartedScene(){
    return Tongue::createScene();
}

Scene* Tongue::returnNewScene(){
    return Ears::createScene();
}


