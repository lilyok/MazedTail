//
//  Holes.h
//  MazedTail
//
//  Created by lilil on 01.05.15.
//
//

#ifndef __MazedTail__Holes__
#define __MazedTail__Holes__

#include <AbstractLabirint.h>
#include "cocos2d.h"
#include <stdio.h>

class Holes: public AbstractLabirint{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onTouchCancelled(cocos2d::Touch *touch, cocos2d::Event *event);
    
    cocos2d::Sprite *makeTexturedSprite(std::string name, int tag, cocos2d::Point p, cocos2d::Size size);
    
    
    // a selector callback
    //    void menuRestartCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(Holes);
    
private:
    std::vector<bool> isA;
    cocos2d::Vector<cocos2d::Sprite*> rabbits;
    cocos2d::Vector<cocos2d::Sprite*> holes;
    
    
    void ownEvent();
    
    std::string hole_name = "";
    
    bool onContactBegin(const cocos2d::PhysicsContact& contact);
    void onContactSeperate(const cocos2d::PhysicsContact& contact);
    
    bool checkCollision(cocos2d::PhysicsContact const &contact, Node *nodeA, Node *nodeB);
    void collisionWithEnemy(Node * nodeA, Node * nodeB);
    
    
    void stopScene();
    void pauseScene();
    void resumeScene();
    cocos2d::Scene* returnRestartedScene();
    cocos2d::Scene* returnNewScene();
    
    
};
#endif /* defined(__MazedTail__Holes__) */
