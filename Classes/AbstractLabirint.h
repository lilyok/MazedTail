//
// Created by lilil on 27.02.15.
//


#ifndef __AbstractLabirint_H_
#define __AbstractLabirint_H_

#include "cocos2d.h"
#define GOING 0
#define NOTOUCH -500000

#define HERO_SPRITE_TAG 5
#define COLLISION_TAG 10
#define PLUS_TAG 30
#define BRICK 1
#define BALL 2
#define ANIMATION_DELAY 0.0001f
#define NEWLEVEL_TAG 40
#define MIN_FOR_DIRECTION 10
#define MY_VELOCITY 500
#define COUNT_OF_DELTA 5

class AbstractLabirint : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(std::string map_name, std::string back_name);

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init(std::string map_name, std::string back_name);


    // a selector callback
    virtual void menuCloseCallback(cocos2d::Ref* pSender);
    virtual void menuPauseCallback(cocos2d::Ref* pSender);
    virtual void menuRestartCallback(cocos2d::Ref* pSender);
    virtual void menuContinueCallback(cocos2d::Ref *pSender);
    // implement the "static create()" method manually
    static AbstractLabirint* create(std::string map_name, std::string back_name) {
        auto p = new AbstractLabirint();
        if (p->init(map_name, back_name)) {
            p->autorelease();
            return p;
        } else {
            delete p;
            return nullptr;
        }
    }


    virtual cocos2d::Scene* returnRestartedScene() { return this->getScene(); };
    virtual cocos2d::Scene* returnNewScene() { return this->getScene(); };
    virtual void ownEvent() {};

protected:
    cocos2d::Vector<cocos2d::Sprite*> collisions;

    cocos2d::Vector<Node*> pausedNodes;
    cocos2d::Sprite* mysprite;
    cocos2d::Sprite* mylife;
    cocos2d::Sprite* menuSprite;
    cocos2d::Vector<cocos2d::Sprite*> pluses;

    cocos2d::Animate* animateGoing;

    virtual void onEnter();
    virtual void onExit();

    void update(float delta) override;
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

    void setPhyWorld(cocos2d::PhysicsWorld* world){m_world = world;}
    virtual cocos2d::Sprite *makeTexturedSprite(std::string name, int tag, cocos2d::Point p, cocos2d::Size size);
    cocos2d::Vector<cocos2d::Sprite*>  makeObject(int tag, cocos2d::TMXObjectGroup *objects,
            cocos2d::SpriteFrameCache* spritecache, std::string name, int spritecount,
            int animsize, float scale_map, float xZero, float yZero, int form, float time,  bool isDynamic, float rest,
            float fric, float dens, int mask = 0xFFFFFFFF);
    cocos2d::Vector<cocos2d::Sprite*> makeObject(int tag, cocos2d::TMXObjectGroup *objects, float scale_map, float xZero, float yZero, int form,  bool isDynamic=false, int v = 0, int n = -1, int mask = 0xFFFFFFFF);
    cocos2d::Sprite* makePhysicsObjAt(int tag, cocos2d::Point p, cocos2d::Size size, bool isDynamic, float rest, float fric, float dens, int form, cocos2d::Animate* anim, std::string name, int mask = 0xFFFFFFFF);
    cocos2d::Sprite* makePhysicsObjAt(std::string name, int tag, cocos2d::Point p, cocos2d::Size size, int form,  bool isDynamic=false, int v = 0, int n = -1, int mask = 0xFFFFFFFF);

    void stopAllObjectLayer(cocos2d::Vector<cocos2d::Sprite*> sprites);
    void pauseAllObjectLayer(cocos2d::Vector<cocos2d::Sprite*> sprites);
    void resumeAllObjectLayer(cocos2d::Vector<cocos2d::Sprite*> sprites);
    void resumeAllObjects();
    void pauseAllObjects();
    void stopAllObjects();

    virtual void resumeScene();
    virtual void pauseScene();
    virtual void stopScene();
    void pause();
    void resume(bool isResumeScene = true);
    void hideBottom();
    void finishMenuAction();
    
    virtual bool onContactBegin(const cocos2d::PhysicsContact& contact) {return true;};
    virtual void onContactSeperate(const cocos2d::PhysicsContact& contact) {};
    
    void prettyShowItem(cocos2d::MenuItemImage* item, cocos2d::Vec2 endPosition);

    bool isRestart = false;
    bool isRestarted = false;
    bool isNewLeveled = false;
    bool isNewLevel = false;
    bool isPaused = false;
    bool isClose = false;
    bool isPlus = false;
    float touchX = NOTOUCH;
    float touchY = NOTOUCH;
    int life_num = 3;
    float icon_scale = 1.0;
    float scale_map = 1.0;
    float scale_hero = 1.0;
    float xZero = 0.0;
    float yZero = 0.0;

    
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
    cocos2d::MenuItemImage* restartItem;
    cocos2d::MenuItemImage* continueItem;
    cocos2d::MenuItemImage* closeItem;
    cocos2d::MenuItemImage* pauseItem;

    
    cocos2d::TMXTiledMap *map;


    cocos2d::PhysicsWorld* m_world;

    virtual bool checkCollision(cocos2d::PhysicsContact const &contact, Node *nodeA, Node *nodeB) {return true;};

    void collisionWithHealth(Node * nodeA, Node * nodeB);

    
    void goHero(bool isStarted = false);
    
    void goToPoint(float dx, float dy, bool isStarted = false);
    void startGoingAnimation();
    
    void stopTakingPoints();
};


#endif //__AbstractLabirint_H_
