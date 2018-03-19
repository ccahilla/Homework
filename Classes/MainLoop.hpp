//
//  MainLoop.hpp
//  Homework
//
//  Created by Craig Cahillane on 3/18/18.
//
//
#pragma once

#ifndef MainLoop_hpp
#define MainLoop_hpp

#include <stdio.h>

class MainLoop : public cocos2d::Layer {
    
public:
    
    static cocos2d::Scene* createScene();
    virtual bool init() override; // override since init() is a member function for Layer, same for update
    void update(float deltaTime) override;
    CREATE_FUNC(MainLoop); //make the MainLoop::create() function using the cocos2d macro
    
private:
    
};
#endif /* MainLoop_hpp */
