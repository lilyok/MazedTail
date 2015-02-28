//
// Created by lilil on 27.02.15.
//


#ifndef __AbstractLabirint_H_
#define __AbstractLabirint_H_

#include "cocos2d.h"
#define LEFT 0
#define RIGHT 1
#define TOP 2
#define BOTTOM 3
#define NODIRECTION -1

#define HERO_SPRITE_TAG 5
#define COLLISION_TAG 10
#define PLUS_TAG 30
#define BRICK 1
#define BALL 2
#define ANIMATION_DELAY 0.0001f
#define NEWLEVEL_TAG 40
#define MIN_FOR_DIRECTION 10
#define MY_VELOCITY 300

class AbstractLabirint : public cocos2d::Layer
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(std::string map_name);

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init(std::string map_name);


    // a selector callback
    virtual void menuCloseCallback(cocos2d::Ref* pSender);
    virtual void menuRestartCallback(cocos2d::Ref* pSender) {};
    virtual void menuNewLeveltCallback(cocos2d::Ref *pSender) {};
    // implement the "static create()" method manually
    static AbstractLabirint* create(std::string map_name) {
        auto p = new AbstractLabirint();
        if (p->init(map_name)) {
            p->autorelease();
            return p;
        } else {
            delete p;
            return nullptr;
        }
    }


protected:
    cocos2d::Vector<cocos2d::Sprite*> collisions;

    cocos2d::Vector<Node*> pausedNodes;
    cocos2d::Sprite* mysprite;
    cocos2d::Sprite* mylife;
    cocos2d::Vector<cocos2d::Sprite*> pluses;

    cocos2d::Animate* animateBottom;
    cocos2d::Animate* animateLeft;
    cocos2d::Animate* animateRight;
    cocos2d::Animate* animateTop;

    virtual void onEnter();
    virtual void onExit();

    void update(float delta) override;
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);

    void setPhyWorld(cocos2d::PhysicsWorld* world){m_world = world;}
    cocos2d::Vector<cocos2d::Sprite*>  makeObject(int tag, cocos2d::TMXObjectGroup *objects,
            cocos2d::SpriteFrameCache* spritecache, std::string name, int spritecount,
            int animsize, float scale_map, float xZero, float yZero, int form, float time,  bool isDynamic, float rest,
            float fric, float dens, int mask = 0xFFFFFFFF);
    cocos2d::Vector<cocos2d::Sprite*> makeObject(int tag, cocos2d::TMXObjectGroup *objects, float scale_map, float xZero, float yZero, int form, int v = 0, int n = -1, int mask = 0xFFFFFFFF);
    cocos2d::Sprite* makePhysicsObjAt(int tag, cocos2d::Point p, cocos2d::Size size, bool isDynamic, float rest, float fric, float dens, int form, cocos2d::Animate* anim, std::string name, int mask = 0xFFFFFFFF);
    cocos2d::Sprite* makePhysicsObjAt(int tag, cocos2d::Point p, cocos2d::Size size, int form, int v = 0, int n = -1, int mask = 0xFFFFFFFF);

    void stopAllObjectLayer(cocos2d::Vector<cocos2d::Sprite*> sprites);
    void stopAllObjects();

    virtual void stopScene();
    void goToPoint(float dx, float dy);

    virtual bool onContactBegin(const cocos2d::PhysicsContact& contact) {return true;};
    virtual void onContactSeperate(const cocos2d::PhysicsContact& contact) {};
    bool isRestart = false;
    bool isNewLevel = false;
    int direction = NODIRECTION;
    float touchX = -500000;
    float touchY = -500000;
    int life_num = 3;
    float scale_map = 1.0;
    float xZero = 0.0;
    float yZero = 0.0;
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
    cocos2d::MenuItemImage* restartItem;
    cocos2d::MenuItemImage* newlevelItem;
    cocos2d::TMXTiledMap *map;


    cocos2d::PhysicsWorld* m_world;

    virtual bool checkCollision(cocos2d::PhysicsContact const &contact, Node *nodeA, Node *nodeB) {return true;};

    void collisionWithHealth(Node * nodeA, Node * nodeB);

    bool goToPointX(float dx, float dy, float vx_old, float vy_old, float vx, cocos2d::PhysicsBody *body, cocos2d::Vec2 &pos, float &vy);

    bool goToPointY(float dx, float dy, float vx_old, float vy_old, float vy, cocos2d::PhysicsBody *body, cocos2d::Vec2 &pos, float &vx);
};


#endif //__AbstractLabirint_H_
