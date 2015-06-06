//
//  Balance.h
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#ifndef __MazedTail__Balance__
#define __MazedTail__Balance__

#include <stdio.h>
#include <AbstractLabirint.h>
#include "BotsManager.h"
#include "cocos2d.h"
#define BOT_DELTA 50

class Balance: public AbstractLabirint{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    cocos2d::Sprite *makeTexturedSprite(std::string name, int tag, cocos2d::Point p, cocos2d::Size size);
    
    
    // a selector callback
    //    void menuRestartCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(Balance);
    
private:
    BotsManager*  botsManager;
    cocos2d::Vector<cocos2d::Sprite*> balancers;
    cocos2d::Vector<cocos2d::Sprite*> unknown;
    cocos2d::Vector<cocos2d::Sprite*> unlines;
    cocos2d::Vector<cocos2d::Sprite*> movings;
    cocos2d::Vec2 myforce = cocos2d::Vec2(0,0);
    
    void ownEvent();
    
    bool onContactBegin(const cocos2d::PhysicsContact& contact);
    void onContactSeperate(const cocos2d::PhysicsContact& contact);
    
    bool checkCollision(cocos2d::PhysicsContact const &contact, Node *nodeA, Node *nodeB);
    void collisionWithEnemy(Node * nodeA, Node * nodeB);
    
    
    void stopScene();
    void pauseScene();
    void resumeScene();
    cocos2d::Scene* returnRestartedScene();
    cocos2d::Scene* returnNewScene();
    int num_bot_delta = 0;
    int num_of_portals = 0;
};
#endif /* defined(__MazedTail__Balance__) */
