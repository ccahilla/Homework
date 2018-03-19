//
//  GridLayer.cpp
//  Homework
//
//  Created by Craig Cahillane on 6/4/17.
//
//
/*
 This layer will be the main overlay on the battle scene.  
 It will serve as the closest layer to the player, with a very low opacity, i.e. the player can see through it quite easily.  
 It's purpose will be to provide a faint, omnipresent structure to the battlefield, which will (hopefully) become quite cluttered and exciting.
 */
#include "GridLayer.h"

USING_NS_CC;

bool GridLayer::init() {
    if ( !Layer::init() )
    {
        return false;
    }
    
    initialGridDrawingTime = 3.0;  // Three seconds to draw the main grid overlay
    xGridlines = 8;
    yGridlines = 5;
    
    visibleSize = Director::getInstance()->getVisibleSize(); // Get edges of the screen.  (Upper Left is origin = (0,0))
    origin = Director::getInstance()->getVisibleOrigin();
    
    xGridWidth = visibleSize.width / xGridlines;
    yGridHeight = visibleSize.height / yGridlines;
    
    xOffset = 0; //xGridWidth / 2.0;
    yOffset = 0; //yGridHeight / 2.0;
    
    gridNode = DrawNode::create();

    cocos2d::log("origin.x = %f, origin.y = %f", origin.x, origin.y);
    cocos2d::log("visibleSize.height = %f, visibleSize.width = %f", visibleSize.height, visibleSize.width);
    
    gridNode->setPosition( Vec2(origin.x, origin.y ) );
    
    this->addChild(gridNode, 1);  // Add gridNode to gridLayer as a child
    
//    this->scheduleUpdate();  // 
    return true;
}

void GridLayer::update(float deltaTime) {
    // Clear the gridNode of all drawings each time we update.  This is inefficient, but the idea behind have a bunch of layers is to not have to call this very often
    gridNode->clear();
    
    timeTracker += deltaTime; // Store the amount of time we've been here
    double progress = timeTracker / initialGridDrawingTime;
    
    // Draw the y grid lines
    for( int ii = 0; ii < yGridlines; ++ii ) {
        double yPos = ii * yGridHeight + yOffset;
        double x0, x1;
        if ( ii % 2 == 0 ) {
            x0 = 0.0;
            x1 = progress * visibleSize.width;
        } else {
            x0 = visibleSize.width;
            x1 = visibleSize.width * (1.0 - progress);
        }
        gridNode->drawSegment(Vec2(x0, yPos), Vec2(x1, yPos), 2, Color4F(1.0, 1.0, 1.0, 0.5));
    }
    // Draw the x grid lines
    for( int ii = 0; ii < xGridlines; ++ii ) {
        double xPos = ii * xGridWidth + xOffset;
        double y0, y1;
        if ( ii % 2 == 0 ) {
            y0 = 0.0;
            y1 = progress * visibleSize.height;
        } else {
            y0 = visibleSize.height;
            y1 = visibleSize.height * (1.0 - progress);
        }
        gridNode->drawSegment(Vec2(xPos, y0), Vec2(xPos, y1), 2, Color4F(1.0, 1.0, 1.0, 0.5));
    }
    
    return;
}

double GridLayer::getGridDrawingTime() {
    return initialGridDrawingTime;
}

void GridLayer::setGridDrawingTime(double newTime) {
    initialGridDrawingTime = newTime;
}
