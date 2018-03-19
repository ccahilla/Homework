//
//  MainLoop.cpp
//  Homework
//
//  Created by Craig Cahillane on 3/18/18.
//
//

#include "MainLoop.hpp"
#include "ProcessInputs.hpp"
#include "DrawGame.hpp"
USING_NS_CC;

cocos2d::Scene* MainLoop::createScene()
{
    auto scene = Scene::create();
    auto layer = MainLoop::create();
    scene->addChild(layer);
    
    return scene;
}

bool MainLoop::init() {
    if ( !Layer::init() ) {
        return false;
    }
    
    return true;
}

void MainLoop::update(float deltaTime) {

}
