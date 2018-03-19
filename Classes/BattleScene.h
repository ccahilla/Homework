#pragma once

#include "cocos2d.h"
#include "GridLayer.h"
#include <map>

class BattleScene : public cocos2d::Layer
{
    
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init() override;
    
    bool isAnyKeyPressed();
    bool isThisKeyPressed(cocos2d::EventKeyboard::KeyCode);
    bool isThisKeyReleased(cocos2d::EventKeyboard::KeyCode);
    double keyPressedDuration(cocos2d::EventKeyboard::KeyCode);
    double keyReleasedDuration(cocos2d::EventKeyboard::KeyCode);
    void performKeyActions(float deltaTime);
    void drawMainGrid();
    void drawHealthBar(float deltaTime);
    void basicAttack(float deltaTime);
    void drawEnemyBase(float deltaTime);
    void checkIfAttackHits();
    void drawPlayerCursor(float deltaTime);
    void drawPlayerCube(float deltaTime);
    
    //virtual void draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, bool transformUpdated);
    CREATE_FUNC(BattleScene);
    
    void update(float) override;
    
private:
    const double pi = 3.14159265359;
    
    GridLayer* gridLayer;
    
    cocos2d::DrawNode* drawPlayerGrid;
    cocos2d::DrawNode* healthBar;
    cocos2d::DrawNode* attackNode;
    cocos2d::DrawNode* enemyBase;
    cocos2d::DrawNode* playerCursor;
    cocos2d::DrawNode* playerCube;
    cocos2d::Label * scoreLabel;
    
    static const int maxGridDim = 21;
    static const int halfGridDim = (maxGridDim - 1)/2;
    static const int gridPolygonVertices = 4;  // Want grid to be squares for now
    int gridBoard[maxGridDim][maxGridDim]; // 21 is arbitrary choice since I am a bad programmer
    double gridBoardX[maxGridDim][maxGridDim][gridPolygonVertices];
    double gridBoardY[maxGridDim][maxGridDim][gridPolygonVertices];
    int gridNumX; int gridNumY;
    int gridSquareLength;
    
    int leftCanBePressed;
    int rightCanBePressed;
    int upCanBePressed;
    int downCanBePressed;
    
    int healthBarMax;
    int currentHealth;
    int currentBoxBar;
    int score;
    bool noAttackInitialized;
    int lastAttackHit;
    double attackTiming;
    double attackTimeLength;
    double attackPositionX;
    double attackPositionY;
    int phiSign;
    
    static std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> PressedKeys;
    static std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> ReleasedKeys;
    
    int xCenter; int xMax; int xMin;
    int yCenter; int yMax; int yMin;
    double phase;
    double totalTime;
    double angVelocity;  //Angular Velocity
    double angAcceleration;
    double positionX;
    double positionY;
    double velocityX;
    double velocityY;
    double accelerationX;
    double accelerationY;
};

