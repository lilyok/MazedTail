//
//  Noses.h
//  MazedTail
//
//  Created by lilil on 22.03.15.
//
//

#ifndef __MazedTail__Noses__
#define __MazedTail__Noses__

#include <stdio.h>
#include <AbstractLabirint.h>
#include "cocos2d.h"
#define INS_DELTA 100

class Noses: public AbstractLabirint{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    cocos2d::Sprite *makeTexturedSprite(std::string name, int tag, cocos2d::Point p, cocos2d::Size size);
    
    
    // a selector callback
    //    void menuRestartCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(Noses);
    
private:
    cocos2d::Vector<cocos2d::Sprite*> noses;
    cocos2d::Vector<cocos2d::Sprite*> smells;
    cocos2d::Vector<cocos2d::Sprite*> cheeses;
    cocos2d::Vector<cocos2d::Sprite*> insnose;
    
    int num_ins_delta = 0;
    int sf = 0;
    std::string last_name = "";
    
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
    
    
};
#endif /* defined(__MazedTail__Noses__) */
