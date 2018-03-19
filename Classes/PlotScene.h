#pragma once

#include "cocos2d.h"
#include <map>

class PlotScene : public cocos2d::Layer
{
    
public:
    static cocos2d::Scene* createScene();
    
    void drawDashedLine(cocos2d::Vec2 origin, cocos2d::Vec2 destination, double dashLength);
    bool isThisKeyPressed(cocos2d::EventKeyboard::KeyCode code);
    void performKeyActions(float deltaTime);
    double yFunction(double xCurData);
    
    virtual bool init() override;
    //virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, bool transformUpdated);
    CREATE_FUNC(PlotScene);
    
    void update(float) override;
    
private:
    cocos2d::DrawNode* drawLine1;
    std::vector<cocos2d::Label*> xLabels;
    std::vector<cocos2d::Label*> yLabels;
    cocos2d::Label * functionLabel;
    
    int xCenter;
    int yCenter;
    double totalTime;
    float angVel;  //Angular Velocity
    float vel;     //Velocity
    
    std::vector<double> xData;
    std::vector<double> yData;
    
    double xDataMax;
    double xDataMin;
    double xDataRange;
    double yDataMax;
    double yDataMin;
    double yDataRange;
    
    std::vector<double> xDataNew;
    std::vector<double> yDataNew;
    std::vector<double>::iterator result;
    
    std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> PressedKeys;
    std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> ReleasedKeys;
    
    double plotSizeX;
    double plotSizeY;
    int xGridLineNumber;
    int yGridLineNumber;
    double xGridSpace;
    double yGridSpace;
    
    int dataLength;
    
    double frequency;
    double amplitude;
};