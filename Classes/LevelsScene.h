//
//  LevelsScene.h
//  MazedTail
//
//  Created by lilil on 05.05.15.
//
//

#ifndef __Dusk__LevelsScene__
#define __Dusk__LevelsScene__

#include <stdio.h>
#include "cocos2d.h"
#define MAX_MENU 2
#define MAX_LEVELS 6
/**
 * This scene contains the start button.
 * It inherits after CCLayer because it has only one layer.
 */
class LevelsScene : public cocos2d::Layer
{
public:
    LevelsScene();
    bool init();
    static cocos2d::Scene* createScene();
    void start();    
    void close();
    
    void clearMap();
    void prepareMap(std::string name);
    void changeLevel(std::string name);
    float icon_scale = 1.0;
    float scale_map = 1.0;
    float xZero = 0.0;
    float yZero = 0.0;
    
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
    cocos2d::TMXTiledMap *map;
    
    void update(float delta) override;
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    virtual void onEnter();

    cocos2d::Vector<cocos2d::Sprite*> levels;
    cocos2d::Vector<cocos2d::Sprite*> buttons;
    
   // int high_score = 0;
   // cocos2d::UserDefault *def;

    bool isClose = false;
    bool isStart = false;
    bool isNext = false;
    bool isBack = false;
    int cur_menu = 1;

    int level_i = -1;
    
    void finishSplash(float dt);
    
    CREATE_FUNC(LevelsScene);
    // implement the "static node()" method manually
    //    LAYER_NODE_FUNC(LevelsScene);
};
#endif /* defined(__Dusk__LevelsScene__) */