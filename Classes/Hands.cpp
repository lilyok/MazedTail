//
//  Touch.cpp
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#include "Hands.h"
#include "Noses.h"
USING_NS_CC;

#define TORT_TAG 20

#define BUTTON_TAG 105
#define DOOR_TAG 100


Scene *Hands::createScene() {
    // 'scene' is an autorelease object
    auto scene = Scene::createWithPhysics();
    //scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
    // 'layer' is an autorelease object
    auto layer = Hands::create();
    layer->setPhyWorld(scene->getPhysicsWorld());
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool Hands::init() {
    //////////////////////////////
    // 1. super init first
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("flower.plist");
    
    
    if (!AbstractLabirint::init("rotates.tmx", "back2.png")) {
        return false;
    }
    
    
    this->scheduleUpdate();
    return true;
}

void Hands::ownEvent(){

}

Sprite *Hands::makeTexturedSprite(std::string sprite_name, int tag, cocos2d::Point p, cocos2d::Size size) {
   /* auto w = size.width;
    auto h = size.height;
    if (tag == DOOR_TAG || tag == BUTTON_TAG) {
        auto name = "bluewall.png";
        if (tag == BUTTON_TAG) {
            if (sprite_name == "orange")
                name = "orangeflower.png";
            else
                name = "blueflower.png";
        }
        auto sprite = Sprite::createWithSpriteFrameName(name);//Sprite::create();
        auto current_scaleX = w / sprite->getContentSize().width;
        auto current_scaleY = h / sprite->getContentSize().height;
        sprite->setScale(current_scaleX, current_scaleY);
        sprite->setOpacity(0);
        return sprite;
    } else*/
    return AbstractLabirint::makeTexturedSprite(sprite_name, tag, p, size);
}


bool Hands::onContactBegin(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel && !isPaused) {
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        if (nodeA && nodeB) {
            return checkCollision(contact, nodeA, nodeB);
        }
    }
    return true;
}

void Hands::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        
        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();
        
        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG)
                isPlus = false;
        }
    }
}

bool Hands::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        /*if (nodeA->getTag() == TORT_TAG or nodeB->getTag() == TORT_TAG) {
            audio->playEffect("pain.wav", false, 2.0f, 0.0f, 1.0f);
            collisionWithEnemy(nodeA, nodeB);
        }
        else */if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        } /*else if (nodeA->getTag() == BUTTON_TAG or nodeB->getTag() == BUTTON_TAG) {
            audio->playEffect("btnclick.wav", false, 1.0f, 0.0f, 1.0f);
            auto btn_name = nodeB->getName();
            if (nodeA->getTag() == BUTTON_TAG)
                btn_name = nodeA->getName();
            
            for (auto btn : buttons) {
                if (btn->getName() == btn_name)
                    
                    btn->runAction(Sequence::create(TintTo::create(0.75f, 200, 255, 0), TintTo::create(0.75, 255, 255, 255), NULL));
            }
            
            return false;
        }*/
        else if (nodeA->getTag() == NEWLEVEL_TAG or nodeB->getTag() == NEWLEVEL_TAG) {
            audio->playEffect("harpup.wav", false, 1.0f, 0.0f, 1.0f);
            isNewLevel = true;
            return false;
        }
        
    } /* else if ((nodeA->getTag() == TORT_TAG or nodeB->getTag() == TORT_TAG)  and
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


void Hands::collisionWithEnemy(Node *nodeA, Node *nodeB) {
    if (life_num > 0) {
        life_num--;
        
        char *res = new char[50];
        sprintf(res, "life%i.png", life_num);
        SpriteFrame *sp = SpriteFrameCache::getInstance()->getSpriteFrameByName(res);
        mylife->setSpriteFrame(sp);
/*        Sprite *tort;
        if (nodeA->getTag() == TORT_TAG)
            tort = torts.at(stoi(nodeA->getName()));
        else
            tort = torts.at(stoi(nodeB->getName()));
        
        int num = stoi(tort->getName());
        if (num % 3 == 0)
            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 250 - num*20, 255, 255), NULL));
        else if (stoi(tort->getName()) % 3 == 1)
            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 250 - num*20, 255), NULL));
        else
            tort->runAction(Sequence::create(TintTo::create(0.5f, 255, 0, 0), TintTo::create(0.5, 255, 255, 250 - num*20), NULL));
        
        
        */
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

void Hands::resumeScene() {
    AbstractLabirint::resumeScene();
    /*for (auto sprite: torts) {
        sprite->getPhysicsBody()->setVelocity(Vec2(MY_VELOCITY*scale_map, -MY_VELOCITY*scale_map));
    }
    
    resumeAllObjectLayer(torts);*/
    resumeAllObjectLayer(pluses);
}

void Hands::pauseScene() {
    AbstractLabirint::pauseScene();
    /*for (auto sprite: torts) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    pauseAllObjectLayer(torts);*/
    pauseAllObjectLayer(pluses);
}

void Hands::stopScene() {
    AbstractLabirint::stopScene();
    /*for (auto sprite: torts) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->resetForces();
    }
    stopAllObjectLayer(torts);*/
    stopAllObjectLayer(pluses);
}

Scene* Hands::returnRestartedScene(){
    return Hands::createScene();
}

Scene* Hands::returnNewScene(){
    return Noses::createScene();
}


