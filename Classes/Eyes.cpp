//
//  Eyes.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Eyes.h"
#include "Hands.h"
USING_NS_CC;

#define TORT_TAG 20

#define BULLET_TAG 105
#define EYES_TAG 100
#define INSEYES_TAG 110

Scene *Eyes::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Eyes::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Eyes::init() {
    //////////////////////////////
    // 1. super init first
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("eye.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("inseye.plist");
    
    if (!AbstractLabirint::init("eyes.tmx", "back6.png")) {
        return false;
    }
    
    TMXObjectGroup *eyestmx = map->getObjectGroup("eyes");
    this->eyes = makeObject(EYES_TAG, eyestmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0xFFFFF000);

    auto inseyeCount = 3;
    auto inseyeAnimSize = 2;
    
    TMXObjectGroup *inseyetmx = map->getObjectGroup("inseye");
    this->inseye = makeObject(INSEYES_TAG, inseyetmx, SpriteFrameCache::getInstance(), "inseye", inseyeCount, inseyeAnimSize, scale_map, xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);


    this->scheduleUpdate();
    return true;
}

void Eyes::ownEvent(){
    if (num_ins_delta < INS_DELTA) {
        num_ins_delta++;
    } else {
        for (auto ins : inseye){
            ins->getPhysicsBody()->resetForces();
            ins->getPhysicsBody()->setVelocity(Vec2(0,0));
            float vx = 0.5 * (1 - rand() % 3);
            float vy = 1;
            ins->getPhysicsBody()->applyImpulse(Vec2(Vec2(vx*scale_map, vy*scale_map)));
        }
        num_ins_delta = 0;
    }

}

Sprite *Eyes::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    if (tag == EYES_TAG) {
        Sprite * sprite;
        auto w = size.width;
        auto h = size.height;
        
        Vector<SpriteFrame *> animEyes;
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye01.png"));
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye02.png"));
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye03.png"));
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye04.png"));
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye05.png"));
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye04.png"));
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye03.png"));
        animEyes.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye02.png"));
        
        
        
        // create the animation out of the frames
        Animation *animationEyes = Animation::createWithSpriteFrames(animEyes, 0.1);
        auto dv = 70*scale_map;
        
        sprite = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("eye01.png"));
        Vec2 v = Vec2(0, dv);
        sprite->setScale(w/sprite->getContentSize().width, h/sprite->getContentSize().height);
        
        if (sprite_name == "bottom") {
            sprite->setRotation(0);
            v.y = -dv;
        }
        else if (sprite_name == "top") {
            sprite->setRotation(180);
            v.y = dv;
        }
        else if (sprite_name == "left") {
            sprite->setRotation(90);
            v.x = -dv;
            v.y = 0;
        }
        else if (sprite_name == "right") {
            sprite->setRotation(-90);
            v.x = dv;
            v.y = 0;
        }
        bulletv.push_back(v);
        
        sprite->setPosition(p);
        sprite->runAction(RepeatForever::create(Animate::create(animationEyes)));
        
        auto bullet = Sprite::create("eyebullet.png");
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
        
        auto bh = bullet->getContentSize().height;
        auto bw = bullet->getContentSize().width;
        ParticleFire *p_emitter = ParticleFire::createWithTotalParticles(500);
        p_emitter->setGravity(v);
        
        p_emitter->setScale(scale_map*5);
        p_emitter->setPosition(bw/2, bh/2);
        bullet->addChild(p_emitter, 4);
        bullets.pushBack(bullet);
        
        addChild(bullet, 3);
        return sprite;
    } else
        return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Eyes::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Eyes::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
        }
    }
}

bool Eyes::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == INSEYES_TAG or nodeB->getTag() == INSEYES_TAG or
            nodeA->getTag() == BULLET_TAG or nodeB->getTag() == BULLET_TAG) {
            audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);
            setNextLevelNum(7);
            isNewLevel = true;
            return false;
        }
        
    }
    else if (nodeA->getTag() == BULLET_TAG or nodeB->getTag()== BULLET_TAG)  {
        if (nodeA->getTag() == EYES_TAG or nodeB->getTag()== EYES_TAG) {
            
            return false;
        } else {
            auto node = nodeB;
            if (nodeA->getTag() == BULLET_TAG)
                node = nodeA;
            
            node->setOpacity(0);
            node->runAction(FadeTo::create(1, 255));
            node->setPosition(bulletpositions.at(stoi(node->getName())));
        }
    }

    
    return true;
}


void Eyes::collisionWithEnemy(Node *nodeA, Node *nodeB) {
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

void Eyes::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: inseye) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
        sprite->getPhysicsBody()->setGravityEnable(true);
    }
    auto i = 0;
    for (auto sprite: bullets) {
        sprite->getPhysicsBody()->applyForce(bulletv.at(i));
        i++;
    }
    
    resumeAllObjectLayer(eyes);
    resumeAllObjectLayer(inseye);
    resumeAllObjectLayer(bullets);
    
    resumeAllObjectLayer(pluses);
    cocos2d::Node::resume();
}

void Eyes::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: inseye) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    for (auto sprite: bullets) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(eyes);
    pauseAllObjectLayer(inseye);
    pauseAllObjectLayer(bullets);
    
    pauseAllObjectLayer(pluses);
}

void Eyes::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: inseye) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    for (auto sprite: bullets) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    stopAllObjectLayer(eyes);
    stopAllObjectLayer(inseye);
    stopAllObjectLayer(bullets);
   
    stopAllObjectLayer(pluses);
}

Scene* Eyes::returnRestartedScene(){
    return Eyes::createScene();
}

Scene* Eyes::returnNewScene(){
    return Hands::createScene();
}


