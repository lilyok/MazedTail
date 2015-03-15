//
//  Bot.h
//  MazedTail
//
//  Created by lilil on 14.03.15.
//
//

#ifndef __MazedTail__Bot__
#define __MazedTail__Bot__

#include "cocos2d.h"

#include <stdio.h>
class Bot
{
public:
    Bot(float xZero, float yZero, float obj_width, float obj_height, float x, float y, std::string plist_name, std::string sprite_name, float scale_map, int tag, std::string name);
    
    void changeDirection();
    cocos2d::Sprite* bot;
    
    cocos2d::Animate* botAnimateBottom;
    cocos2d::Animate* botAnimateLeft;
    cocos2d::Animate* botAnimateRight;
    cocos2d::Animate* botAnimateTop;
    
    float scale_bot = 1.0;
    float scale_map = 1.0;
    int direction;
    
};
#endif /* defined(__MazedTail__Bot__) */
