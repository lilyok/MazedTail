//
//  Bot.cpp
//  MazedTail
//
//  Created by lilil on 14.03.15.
//
//

#include "BotsManager.h"
USING_NS_CC;

BotsManager::BotsManager(float xZero, float yZero, TMXTiledMap *map, std::string layer_name, std::string plist_name, std::string sprite_name, float scale_map, int tag) {

    TMXObjectGroup *botObject = map->getObjectGroup(layer_name);
    float x, y, obj_width, obj_height;
    if (botObject != nullptr){
        ValueVector objectsPoint = botObject->getObjects();
        int i = 0;
        for (auto objPointMap : objectsPoint) {
            ValueMap objPoint = objPointMap.asValueMap();
            x = objPoint.at("x").asFloat();
            y = objPoint.at("y").asFloat();
            obj_width = objPoint.at("width").asFloat();
            obj_height = objPoint.at("height").asFloat();
            
            bots.push_back(new Bot(xZero, yZero, obj_width, obj_height, x, y, plist_name, sprite_name, scale_map, tag, std::to_string(i)));
            i++;
        }
    }
}

void BotsManager::changeDirection(std::string name) {
    bots[stoi(name)]->changeDirection();
}

void BotsManager::changeDirectionAll() {
    for(auto bot : bots)
        bot->changeDirection();
}


Vector<Sprite*> BotsManager::getBotsSprites(){
    Vector<Sprite*> sprites;
    for (auto bot: bots)
        sprites.pushBack(bot->bot);
    
    return sprites;
}
