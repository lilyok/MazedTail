//
//  Portals.cpp
//
//  Created by lilil on 08.01.15.
//
//
#include "Portals.h"
#include "MenuScene.h"

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


    if (!AbstractLabirint::init("labirint.tmx")) {
        return false;
    }

    auto labirintcache = SpriteFrameCache::getInstance();
    labirintcache->addSpriteFramesWithFile("lb.plist");
    auto labirint = Sprite::createWithSpriteFrameName("labirint01.png");


//    Vector<SpriteFrame *> animLabirintFrames;
//    animLabirintFrames.reserve(4);
//    animLabirintFrames.pushBack(labirintcache->getSpriteFrameByName("labirint03.png"));
//    animLabirintFrames.pushBack(labirintcache->getSpriteFrameByName("labirint01.png"));
//    animLabirintFrames.pushBack(labirintcache->getSpriteFrameByName("labirint02.png"));
//    animLabirintFrames.pushBack(labirintcache->getSpriteFrameByName("labirint01.png"));
//
//
//    // create the animation out of the frames
//    Animation *animationLabirint = Animation::createWithSpriteFrames(animLabirintFrames, 0.5f);
//    auto animateLabirint = Animate::create(animationLabirint);
//    labirint->runAction(RepeatForever::create(animateLabirint));
    labirint->setPosition(Vec2(labirint->getContentSize().width / 2, labirint->getContentSize().height / 2));
    map->addChild(labirint, 2);


    TMXObjectGroup *holes = map->getObjectGroup("portals");

    this->portals = makeObject(PORTAL_TAG, holes, scale_map, xZero, yZero, BRICK, 0, 0);


    auto spidercache = SpriteFrameCache::getInstance();
    spidercache->addSpriteFramesWithFile("spider.plist");
    auto spidersCount = 3;
    auto spidersAnimSize = 2;

    TMXObjectGroup *fallings = map->getObjectGroup("fallings");
    this->fallings = makeObject(FALLING_TAG, fallings, spidercache, "spider", spidersCount, spidersAnimSize, scale_map,
            xZero, yZero, BALL, 0.8f, true, 1.0f, 0.2f, 1.0f);


    //////////////////////////////////////////////////


    this->m_emitter = ParticleFire::create();
    this->m_emitter->setScale(scale_map / 2);
    this->m_emitter->stopSystem();
    addChild(m_emitter, 3);

    for (auto portal : this->portals) {
        ParticleSystemQuad *p_emitter = ParticleGalaxy::create();
        p_emitter->setScale(scale_map / 2);
        p_emitter->setPosition(0, 0);//32*scale_map,32*scale_map);
        portal->addChild(p_emitter);
    }


    this->scheduleUpdate();
    return true;
}

void Portals::update(float delta) {

    if (mysprite->getNumberOfRunningActions() <= 0) {
        if (isRestart || isNewLevel) {
            this->pausedNodes = cocos2d::Director::getInstance()->getActionManager()->pauseAllRunningActions();
            if (isRestart) {
                restartItem->setVisible(true);
                mysprite->getPhysicsBody()->setGravityEnable(true);
            }
            else {
                stopScene();
                newlevelItem->setVisible(true);
            }

        }
    }

    if (!isRestart && !isNewLevel) {
        for (auto plus: pluses) {
            if (plus->getOpacity() < 255) {
                plus->setOpacity(plus->getOpacity() + 1);
            }
        }

    }
}

void Portals::onContactSeperate(const cocos2d::PhysicsContact &contact) {
    if (!isRestart && !isNewLevel) {
        if ((touchX != -500000) && (touchY != -500000)) {
            auto dx = touchX - mysprite->getPositionX();
            auto dy = touchY - mysprite->getPositionY() + mysprite->getContentSize().height * scale_map / 2;
            direction = NODIRECTION;
            goToPoint(dx, dy);
        }

        auto nodeA = contact.getShapeA()->getBody()->getNode();
        auto nodeB = contact.getShapeB()->getBody()->getNode();

        if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
            if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
                Node *node;
                if (nodeA->getTag() == PLUS_TAG) node = nodeA;
                else node = nodeB;
                if (node->getOpacity() == 255) {
                    node->setOpacity(0);
                    node->setGlobalZOrder(0);
                }
            } else if (nodeA->getTag() == COLLISION_TAG or nodeB->getTag() == COLLISION_TAG) {
                m_emitter->stopSystem();
            } else if (nodeA->getTag() == PORTAL_TAG or nodeB->getTag() == PORTAL_TAG) {
                isPortal = true;
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
    if (!isRestart && !isNewLevel) {
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
    if (nodeA->getTag() == PORTAL_TAG) {
        portal_name = nodeA->getName();
        node = nodeB;
    }
    else {
        portal_name = nodeB->getName();
        node = nodeA;
    }

    node->runAction(Sequence::create(TintTo::create(0.0f, 0, 255, 144), TintTo::create(0.5, 255, 255, 255), NULL));
    long pos = portal_name.length() - 1;
    int num = 1;
    if (portal_name.at(pos) == '1') num = 2;
    std::__1::string res = portal_name.substr(0, pos).append(std::__1::to_string(num));
    // cocos2d::log("start = %s, stop = %s", portal_name.c_str(), res.c_str());
    Vec2 nextPortalPos = Vec2(0, 0);
    Vec2 dPos = Vec2(0, 0);
    for (auto portal : portals) {
        if (portal->getName() == res) {
            nextPortalPos = portal->getPosition();
            auto dPos = Vec2(nextPortalPos.x - node->getPosition().x, 0);
            // stopAllObjects();

            if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
                stopAllObjects();
                direction = NODIRECTION;
                stopAllObjects();
                touchX = -500000;
                touchY = -500000;
                isPortal = false;
                node->setPosition(nextPortalPos);
            }
            else if (isSpiderPortal) {
                isSpiderPortal = false;
                node->setPositionX(nextPortalPos.x);
                node->setPositionY(nextPortalPos.y - portal->getContentSize().height * scale_map);
            }
            break;
        }
    }
}

bool Portals::checkCollision(PhysicsContact const &contact, Node *nodeA, Node *nodeB) {
    if (nodeA->getTag() == HERO_SPRITE_TAG or nodeB->getTag() == HERO_SPRITE_TAG) {
        if (nodeA->getTag() == FALLING_TAG or nodeB->getTag() == FALLING_TAG) {
            collisionWithEnemy(nodeA, nodeB);
        }
        else if (nodeA->getTag() == PLUS_TAG or nodeB->getTag() == PLUS_TAG) {
            collisionWithHealth(nodeA, nodeB);
            return false;
        }
        else if (nodeA->getTag() != NEWLEVEL_TAG and nodeB->getTag() != NEWLEVEL_TAG) {

            m_emitter->setPosition(contact.getContactData()->points[0]);
            m_emitter->resetSystem();
        } else {
            m_emitter = ParticleSmoke::create();
            m_emitter->setPosition(contact.getContactData()->points[0]);
            m_emitter->resetSystem();
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
            mysprite->runAction(TintTo::create(1.0f, 243, 44, 239));
            isRestart = true;
            stopScene();
            restartItem->setVisible(true);
        } else
            mysprite->runAction(Sequence::create(TintTo::create(0.5f, 243, 44, 239), TintTo::create(0.5, 255, 255, 255), NULL));
    }
}

void Portals::stopScene() {
    AbstractLabirint::stopScene();

    for (auto sprite: fallings) {
        sprite->getPhysicsBody()->setVelocity(Vec2(0, 0));
        sprite->getPhysicsBody()->setGravityEnable(false);
    }
    stopAllObjectLayer(portals);
    stopAllObjectLayer(fallings);
    stopAllObjectLayer(pluses);
}

void Portals::onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *event) {
    if (!isRestart && !isNewLevel && isPortal) {
        touchX = touch->getLocation().x;
        touchY = touch->getLocation().y;
    }
}
void Portals::menuRestartCallback(Ref * pSender) {
    auto newScene = Portals::createScene();
    cocos2d::Director::getInstance()->replaceScene(newScene);
    restartItem->setVisible(false);
}

void Portals::menuNewLeveltCallback(Ref * pSender) {
    auto newScene = MenuScene::createScene();
    cocos2d::Director::getInstance()->replaceScene(newScene);
    newlevelItem->setVisible(false);
}
