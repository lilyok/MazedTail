//
//  Lifts.h
//  MazedTail
//
//  Created by lilil on 25.04.15.
//
//

#ifndef __MazedTail__Lifts__
#define __MazedTail__Lifts__

#include <AbstractLabirint.h>
#include "cocos2d.h"
#include <stdio.h>

class Lifts: public AbstractLabirint{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    cocos2d::Sprite *makeTexturedSprite(std::string name, int tag, cocos2d::Point p, cocos2d::Size size);
    
    
    // a selector callback
    //    void menuRestartCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(Lifts);
    
private:
    cocos2d::Vector<cocos2d::Sprite*> lifts;
    cocos2d::Vector<cocos2d::Sprite*> buttons;
    cocos2d::ParticleSystemQuad* m_emitter;

    cocos2d::Vector<cocos2d::ParticleSystemQuad*> m_emitters;

    cocos2d::Vector<cocos2d::Sprite*> butterfly;
    
    cocos2d::Vector<cocos2d::ParticleSystemQuad*> initFires();
    void changeFireColor(cocos2d::ParticleSystemQuad* p, std::string name);
    void changeFiresColor(std::string name);
    
    void ownEvent();
    
    void setLiftStatus();
    bool isHeroTop = true;
    bool isBlue = true;
    bool isGreen = true;
    
//    int num_spider_delta = 0;
//    int num_shift_delta = 0;
//    int num_shift = -1;
    
    
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
#endif /* defined(__MazedTail__Lifts__) */
