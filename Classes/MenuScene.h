//
//  MenuScene.h
//  Dusk
//
//  Created by lilil on 08.01.15.
//
//

#ifndef __Dusk__MenuScene__
#define __Dusk__MenuScene__

#include <stdio.h>
#include "cocos2d.h"

/**
 * This scene contains the start button.
 * It inherits after CCLayer because it has only one layer.
 */
class MenuScene : public cocos2d::Layer
{
public:
    MenuScene();
    bool init();
    static cocos2d::Scene* createScene();
    void menuStartCallback(cocos2d::Ref* pSender);
    void menuRestartCallback(cocos2d::Ref* pSender);
    
    void menuCloseCallback(cocos2d::Ref* pSender);

    CREATE_FUNC(MenuScene);
    // implement the "static node()" method manually
//    LAYER_NODE_FUNC(MenuScene);
};
#endif /* defined(__Dusk__MenuScene__) */
