//
//  Shifts.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Shifts.h"
#include "MenuScene.h"
USING_NS_CC;

#define TORT_TAG 20

#define BUTTON_TAG 105
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
    this->shifts = makeObject(SHIFT_TAG, shifttmx, scale_map, xZero, yZero, BRICK, true, 0, 0);

//    for (auto shift : this->shifts) {
//        shift
//    }
//
//        auto p = shift->getPosition();
//        auto size = shift->getContentSize();
//        
//        shift->getPhysicsBody()->setMass(INFINITY);
//        shift = Sprite::create("greenwall.png");
//        
//        shift->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
//        
//        
//        shift->setTextureRect(Rect(p.x - size.width/2, p.y - size.height/2, size.width, size.height));
//        shift->setOpacity(255);
//
//    }

    this->scheduleUpdate();
    return true;
}

void Shifts::ownEvent(){
    if (num_bot_delta < BOT_DELTA) {
        num_bot_delta++;
    } else {
        auto h = visibleSize.height;
        for (auto shift : shifts) {
            if (shift->getName() == std::__1::to_string(num_shift)) {
                auto p = shift->getPosition();
                auto sh = shift->getContentSize().height*scale_map;

                if (p.y - sh/2 < h)
                    shift->runAction(MoveTo::create(0.5, Vec2(p.x, p.y + h/5)));
                else {
                    shift->runAction(Sequence::create(MoveTo::create(0.5, Vec2(p.x, p.y)), MoveTo::create(0,Vec2(p.x,p.y-2*h)),NULL));
                }
            }
        }
        num_bot_delta = 0;
    }
}

Sprite *Shifts::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
    Sprite * sprite;
    auto w = size.width;
    auto h = size.height;
    
    if (tag == SHIFT_TAG) {
        sprite = Sprite::create("greenwall.png");
        sprite->setOpacity(255);
    } else {
         sprite = Sprite::create();
         sprite->setOpacity(0);
    }
    sprite->getTexture()->setTexParameters({.minFilter =  GL_LINEAR, .magFilter =  GL_LINEAR, .wrapS =  GL_REPEAT, .wrapT =  GL_REPEAT});
    
    sprite->setTextureRect(Rect(p.x, p.y, size.width, size.height));
    
    
    auto current_scaleX = w / sprite->getContentSize().width;
    auto current_scaleY = h / sprite->getContentSize().height;
    sprite->setScale(current_scaleX, current_scaleY);
//    sprite->setOpacity(0);
    return sprite;
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
//    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
//        if (nodeA->getTag() == TORT_TAG or nodeB->getTag() == TORT_TAG) {
//            collisionWithEnemy(nodeA, nodeB);
//        }
//        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
//            collisionWithHealth(nodeA, nodeB);
//            return false;
//        } else if (nodeA->getTag() == BUTTON_TAG or nodeB->getTag() == BUTTON_TAG) {
//            auto btn_name = nodeB->getName();
//            if (nodeA->getTag() == BUTTON_TAG)
//                btn_name = nodeA->getName();
//            
//            for (auto btn : buttons) {
//                if (btn->getName() == btn_name)
//                    
//                    btn->runAction(Sequence::create(TintTo::create(0.75f, 200, 255, 0), TintTo::create(0.75, 255, 255, 255), NULL));
//            }
//            
//            for (auto door : Shifts) {
//                if (door->getName() == btn_name) {
//                    if (door->getRotation() < 45)
//                        door->runAction(RotateTo::create(0.5, 90));
//                    else
//                        door->runAction(RotateTo::create(0.5, 0));
//                    
//                }
//            }
//            
//            return false;
//        }
//        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
//            isNewLevel = true;
//            return false;
//        }
//        
//    }  else if ((nodeA->getTag() == TORT_TAG or nodeB->getTag() == TORT_TAG)  and
//                (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG or
//                 nodeA->getTag() == DOOR_TAG or nodeB->getTag() == DOOR_TAG or
//                 nodeA->getTag() == BUTTON_TAG or nodeB->getTag() == BUTTON_TAG)){
//                    if (nodeA->getTag() == TORT_TAG)
//                        botsManager->changeDirection(nodeA->getName());
//                    else
//                        botsManager->changeDirection(nodeB->getName());
//                    
//    }
    
    return true;
}


void Shifts::collisionWithEnemy(Node *nodeA, Node *nodeB) {
//    if (life_num > 0) {
//        life_num--;
//        
//        char *res = new char[50];
//        sprintf(res, "life%i.png", life_num);
//        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
//        mylife->setSpriteFrame(sp);
//        Sprite *tort;
//        if (nodeA->getTag() == TORT_TAG)
//            tort = torts.at(stoi(nodeA->getName()));
//        else
//            tort = torts.at(stoi(nodeB->getName()));
//        
//        int num = stoi(tort->getName());
//        if (stoi(tort->getName()) % 3 == 0)
//            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 250 - num*20, 255, 255), NULL));
//        else if (stoi(tort->getName()) % 3 == 1)
//            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 250 - num*20, 255), NULL));
//        else
//            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 255, 250 - num*20), NULL));
//        
//        
//        
//        if (life_num == 0) {
//            mysprite->runAction(TintTo::create(1.0f, 243, 44, 239));
//            isRestart = true;
//            stopTakingPoints();
//            isPaused = false;
//            pauseScene();
//            pause();
//        } else
//            mysprite->runAction(Sequence::create(TintTo::create(0.5f, 243, 44, 239), TintTo::create(0.5, 255, 255, 255), NULL));
//    }
}

void Shifts::resumeScene() {
    AbstractLabirint::resumeScene();
    for (auto sprite: torts) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    
    resumeAllObjectLayer(torts);
    resumeAllObjectLayer(pluses);
}

void Shifts::pauseScene() {
    AbstractLabirint::pauseScene();
    for (auto sprite: torts) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(torts);
    pauseAllObjectLayer(pluses);
}

void Shifts::stopScene() {
    AbstractLabirint::stopScene();
    for (auto sprite: torts) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();        
    }
    stopAllObjectLayer(torts);
    stopAllObjectLayer(pluses);
}

Scene* Shifts::returnRestartedScene(){
    return Shifts::createScene();
}

Scene* Shifts::returnNewScene(){
    return Shifts::createScene();
}


