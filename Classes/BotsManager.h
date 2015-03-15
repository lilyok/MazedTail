//
//  Bot.h
//  MazedTail
//
//  Created by lilil on 14.03.15.
//
//

#ifndef __MazedTail__BotsManager__
#define __MazedTail__BotsManager__
#include "cocos2d.h"

#include <stdio.h>

#include "Bot.h"

class BotsManager
{
public:
    BotsManager(float xZero, float yZero, cocos2d::TMXTiledMap *map, std::string layer_name, std::string plist_name, std::string sprite_name, float scale_map, int tag);
    
    cocos2d::Vector<cocos2d::Sprite*> getBotsSprites();
    void changeDirection(std::string name);
    
    std::vector<Bot*> bots;

    cocos2d::Animate* botAnimateBottom;
    cocos2d::Animate* botAnimateLeft;
    cocos2d::Animate* botAnimateRight;
    cocos2d::Animate* botAnimateTop;
    
    float scale_bot = 1.0;
    int direction;

};
#endif /* defined(__MazedTail__BotsManager__) */
