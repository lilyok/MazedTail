//
//  Portals.h
//  Dusk
//
//  Created by lilil on 08.01.15.
//
//

#ifndef __Dusk__Portals__
#define __Dusk__Portals__

#include "cocos2d.h"
#include "AbstractLabirint.h"

#define COUNT_OF_DELTA 100

class Portals : public AbstractLabirint
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    
    // a selector callback
    void menuRestartCallback(cocos2d::Ref* pSender);
    void menuNewLeveltCallback(cocos2d::Ref *pSender);
    // implement the "static create()" method manually
    CREATE_FUNC(Portals);
protected:
    cocos2d::Vector<cocos2d::Sprite*> portals;
    cocos2d::Vector<cocos2d::Sprite*> fallings;
    cocos2d::ParticleSystemQuad* m_emitter;
    
private:
    void update(float delta) override;
    void stopScene();
//    void goToPoint(float dx, float dy);
    
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    bool onContactBegin(const cocos2d::PhysicsContact& contact);
    void onContactSeperate(const cocos2d::PhysicsContact& contact);

    bool isPortal = true;
    bool isSpiderPortal = true;
    
    bool checkCollision(cocos2d::PhysicsContact const &contact, Node *nodeA, Node *nodeB);

    void collisionWithEnemy(Node * nodeA, Node * nodeB);

    void entranceIntoPortal(Node * nodeA, Node * nodeB);
    
    int num_of_delta = 0;

};


#endif /* defined(__Dusk__Portals__) */
