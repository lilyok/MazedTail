//
//  Doors.h
//  Dusk
//
//  Created by lilil on 08.01.15.
//
//

#ifndef __Dusk__Doors__
#define __Dusk__Doors__

#include "cocos2d.h"
#include "AbstractLabirint.h"


class Doors : public AbstractLabirint
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    
    // a selector callback
    //    void menuRestartCallback(cocos2d::Ref* pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(Doors);
protected:
    cocos2d::Vector<cocos2d::Sprite*> portals;
    cocos2d::Vector<cocos2d::Sprite*> fallings;
    cocos2d::ParticleSystemQuad* m_emitter;
    cocos2d::Vector<cocos2d::Sprite*> doors;
    cocos2d::Vector<cocos2d::Sprite*> buttons;

    
private:
    void stopScene();
    void pauseScene();
    void resumeScene();
        
    bool onContactBegin(const cocos2d::PhysicsContact& contact);
    void onContactSeperate(const cocos2d::PhysicsContact& contact);

    cocos2d::Scene* returnRestartedScene();
    cocos2d::Scene* returnNewScene();

    
    bool checkCollision(cocos2d::PhysicsContact const &contact, Node *nodeA, Node *nodeB);
    
    void collisionWithEnemy(Node * nodeA, Node * nodeB);
    
    
    int num_of_delta = 0;
    
};


#endif /* defined(__Dusk__Doors__) */
