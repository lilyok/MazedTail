//
//  Noses.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Noses.h"
#include "Tongue.h"
USING_NS_CC;

#define TORT_TAG 20

#define SMELL_TAG 105
#define NOSE_TAG 100
#define CHEESE_TAG 95
#define INSNOSE_TAG 90


Scene *Noses::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
   // scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Noses::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Noses::init() {
    //////////////////////////////
    // 1. super init first
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("smell.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("noselist.plist");
    
    
    if (!AbstractLabirint::init("noses.tmx", "back8.png")) {
        return false;
    }
    TMXObjectGroup *nosestmx = map->getObjectGroup("noses");
    this->noses = makeObject(NOSE_TAG, nosestmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0x00000FFF);

    TMXObjectGroup *smelltmx = map->getObjectGroup("noseway");
    this->smells = makeObject(SMELL_TAG, smelltmx, scale_map, xZero, yZero, BRICK, false, 0, 0, 0xFFFFF000);
    
    TMXObjectGroup *cheesetmx = map->getObjectGroup("cheeses");
    this->cheeses = makeObject(CHEESE_TAG, cheesetmx, scale_map, xZero, yZero, BALL, true, 0, 0, 0xFFFFF000);
    
    for (auto cheese : cheeses) {
        cheese->getPhysicsBody()->resetForces();
        cheese->getPhysicsBody()->setGravityEnable(false);
        cheese->getPhysicsBody()->setMass(0.001);
        cheese->getPhysicsBody()->setLinearDamping(10.0f);
    }

    auto insnoseCount = 1;
    auto insnoseAnimSize = 2;
    
    TMXObjectGroup *insnosetmx = map->getObjectGroup("insnose");
    this->insnose = makeObject(INSNOSE_TAG, insnosetmx, SpriteFrameCache::getInstance(), "noselist", insnoseCount, insnoseAnimSize, scale_map, xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);
    auto j = 0;
    for (auto n : insnose) {
        auto i = j + 1;
        if (j % 3 == 0)
            n->runAction(TintTo::create(0.0, 250 - i*20, 255, 255));
        else if (j % 3 == 1)
            n->runAction(TintTo::create(0.0, 255, 250 - i*20, 255));
        else if (j % 3 == 2)
            n->runAction(TintTo::create(0.0, 255, 255, 250 - i*20));
        i++;
    }

    
    this->scheduleUpdate();
    return true;
}

void Noses::ownEvent(){
    if (sf !=0)
        mysprite->getPhysicsBody()->applyForce(Vec2(0, sf));
    if (num_ins_delta < INS_DELTA) {
        num_ins_delta++;
    } else {
        for (auto ins : insnose){
            ins->getPhysicsBody()->resetForces();
            ins->getPhysicsBody()->setVelocity(Vec2(0,0));
            float vx = 0.5 * (1 - rand() % 3);
            float vy = 1;
            ins->getPhysicsBody()->applyImpulse(Vec2(Vec2(vx*scale_map, vy*scale_map)));
        }
        num_ins_delta = 0;
    }
}

Sprite *Noses::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    auto w = size.width;
    auto h = size.height;
    if (tag == CHEESE_TAG) {
        auto sprite = Sprite::create("cheese.png");
        auto current_scaleX = w / sprite->getContentSize().width;
        auto current_scaleY = h / sprite->getContentSize().height;
        sprite->setScale(current_scaleX, current_scaleY);
        return sprite;
    }
    else if (tag == NOSE_TAG) {
        Vector<SpriteFrame *> animNose;
        animNose.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("noselist01.png"));
        animNose.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("noselist02.png"));
        
        // create the animation out of the frames
        Animation *animationNose = Animation::createWithSpriteFrames(animNose, 0.5);
        
        auto sprite = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("noselist01.png"));
        sprite->setScale(w/sprite->getContentSize().width, h/sprite->getContentSize().height);
        
        if (sprite_name == "bottom") {
            sprite->setRotation(0);
        }
        else if (sprite_name == "top") {
            sprite->setRotation(180);
        }
        
        sprite->setPosition(p);
        sprite->runAction(RepeatForever::create(Animate::create(animationNose)));
        return sprite;
    }
    else if (tag == SMELL_TAG) {
        Vector<SpriteFrame *> animSmell;
        animSmell.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("smell01.png"));
        animSmell.pushBack(SpriteFrameCache::getInstance()->getSpriteFrameByName("smell02.png"));
        
        // create the animation out of the frames
        Animation *animationSmell = Animation::createWithSpriteFrames(animSmell, 0.5);
        
        auto sprite = Sprite::createWithSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("smell01.png"));
        sprite->setScale(w/sprite->getContentSize().width, h/sprite->getContentSize().height);
        
        sprite->setPosition(p);
        sprite->runAction(RepeatForever::create(Animate::create(animationSmell)));
        return sprite;
    }
    
    return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Noses::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Noses::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
            else if (nodeA->getTag() == SMELL_TAG or nodeB->getTag() == SMELL_TAG) {
                sf = 0;
            }
        } else  if (nodeA->getTag() == SMELL_TAG or nodeB->getName() == last_name) {
            if (nodeA->getTag() == CHEESE_TAG or nodeB->getTag() == CHEESE_TAG) {
                auto cheese = nodeA;
                if (nodeB->getTag() == CHEESE_TAG) {
                    cheese = nodeB;
                }
                cheese->getPhysicsBody()->resetForces();
            }
        }
    }
}

bool Noses::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == INSNOSE_TAG or nodeB->getTag() == INSNOSE_TAG) {
            audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        } else if (nodeA->getTag() == SMELL_TAG or nodeB->getTag() == SMELL_TAG) {
            auto btn_name = nodeB->getName();
            if (nodeA->getTag() == SMELL_TAG)
                btn_name = nodeA->getName();
            
            if (btn_name == "bottom" or btn_name == "bottom1")
                sf = MY_VELOCITY * 2 * scale_map;
            else if (btn_name == "top" or btn_name == "top1")
                sf = - MY_VELOCITY * 2 * scale_map;
            
            last_name = btn_name;
            return false;
        }
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);
            setNextLevelNum(9);
            isNewLevel = true;
            return false;
        }
        
    } else if (nodeA->getTag() == SMELL_TAG or nodeB->getTag() == SMELL_TAG) {
        auto smell = nodeB;
        auto cheese = nodeA;
        if (nodeA->getTag() == SMELL_TAG) {
            smell = nodeA;
            cheese = nodeB;
        }

        if (nodeA->getTag() == CHEESE_TAG or nodeB->getTag() == CHEESE_TAG) {
            cheese->setName(smell->getName());
            if (smell->getName() == "bottom" or smell->getName() == "top1"/**/) {
                smell->setName("top1");
                sf = - MY_VELOCITY * scale_map;
            }
            else if (smell->getName() == "top" or smell->getName() == "bottom1"/**/) {
                smell->setName("bottom1");
                sf = MY_VELOCITY * scale_map;
            }
            
            cheese->getPhysicsBody()->resetForces();
            cheese->getPhysicsBody()->applyForce(Vec2(0,sf/50));
        }
        return false;
    }
    
    /* else if ((nodeA->getTag() == TORT_TAG or nodeB->getTag() == TORT_TAG)  and
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


void Noses::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);
       Sprite *ins;
        if (nodeA->getTag() == INSNOSE_TAG)
            ins = insnose.at(stoi(nodeA->getName()));
        else
            ins = insnose.at(stoi(nodeB->getName()));
        
        int num = stoi(ins->getName());
        if (num % 3 == 0)
            ins->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 250 - num*20, 255, 255), NULL));
        else if (stoi(ins->getName()) % 3 == 1)
            ins->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 250 - num*20, 255), NULL));
        else
            ins->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 255, 250 - num*20), NULL));
        
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

void Noses::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: insnose) {
        sprite->getPhysicsBody()->setGravityEnable(true);
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    
    resumeAllObjectLayer(insnose);
    resumeAllObjectLayer(noses);
    resumeAllObjectLayer(smells);
    resumeAllObjectLayer(pluses);
}

void Noses::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: insnose) {
        sprite->getPhysicsBody()->setGravityEnable(false);
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(insnose);
    pauseAllObjectLayer(noses);
    pauseAllObjectLayer(smells);
    pauseAllObjectLayer(pluses);
}

void Noses::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: insnose) {
        sprite->getPhysicsBody()->setGravityEnable(false);
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    stopAllObjectLayer(insnose);
    stopAllObjectLayer(noses);
    stopAllObjectLayer(smells);
    stopAllObjectLayer(pluses);
}

Scene* Noses::returnRestartedScene(){
    return Noses::createScene();
}

Scene* Noses::returnNewScene(){
    return Tongue::createScene();
}


