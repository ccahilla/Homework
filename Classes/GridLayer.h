//
//  GridLayer.hpp
//  Homework
//
//  Created by Craig Cahillane on 6/4/17.
//
//

#ifndef GridLayer_h
#define GridLayer_h

#pragma once

#include "cocos2d.h"

class GridLayer : public cocos2d::Layer
{
public:
    
    virtual bool init() override; // Creates init() function
    void update(float ) override; // Creates update() function
    
    double getGridDrawingTime();
    void setGridDrawingTime(double);
    
    CREATE_FUNC(GridLayer);  // Create the GridLayer:create() function
    
private:
    cocos2d::DrawNode* gridNode;
    cocos2d::Size visibleSize;
    cocos2d::Vec2 origin;
    
    double initialGridDrawingTime;
    double timeTracker;
    int xGridlines;
    int yGridlines;
    double xGridWidth;
    double yGridHeight;
    double xOffset;
    double yOffset;
};

#endif /* GridLayer_h */
