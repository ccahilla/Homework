#include "PlotScene.h"

USING_NS_CC;

cocos2d::Scene* PlotScene::createScene()
{
    auto scene = Scene::create();
    auto layer = PlotScene::create();
    scene->addChild(layer);
    
    return scene;
}

bool PlotScene::init() {
    if ( !Layer::init() ) {
        return false;
    }
    totalTime = 0.0;
    angVel = 0.5;
    
    frequency = 5.0;
    amplitude = 10.0;
    drawLine1 = DrawNode::create();
    xCenter = this->getBoundingBox().getMidX();
    yCenter = this->getBoundingBox().getMidY();
    drawLine1->setPosition(xCenter, yCenter);
    //drawLine1->drawSegment(Vec2(0, 0), Vec2(-100, 100), 1, Color4F::WHITE);
    
    dataLength = 1001;
    double xRangeBegin = 0.0;
    double xRangeEnd = 100.0;
    double xMultiplier = (xRangeEnd - xRangeBegin)/(dataLength-1);
    for(int i=0; i < dataLength; ++i) {
        double xCurData = i * xMultiplier;
        double yCurData = yFunction(xCurData);
        xData.push_back(xCurData);
        yData.push_back(yCurData);
    }
    
    double yRangeBegin = -10.0;
    double yRangeEnd = 10.0;
    
    result = std::max_element(xData.begin(), xData.end());
    xDataMax = xData[std::distance(xData.begin(), result)];
    
    result = std::min_element(xData.begin(), xData.end());
    xDataMin = xData[std::distance(xData.begin(), result)];
    
    xDataRange = xDataMax - xDataMin;
    
    result = std::max_element(yData.begin(), yData.end());
    yDataMax = yData[std::distance(yData.begin(), result)];
    if (yRangeEnd > yDataMax)
        yDataMax = yRangeEnd;
    
    result = std::min_element(yData.begin(), yData.end());
    yDataMin = yData[std::distance(yData.begin(), result)];
    if (yRangeBegin < yDataMin)
        yDataMin = yRangeBegin;
    
    yDataRange = yDataMax - yDataMin;
    
    xGridLineNumber = 11;
    yGridLineNumber = 11;
    
    xGridSpace = 1.0 / (xGridLineNumber - 1);
    yGridSpace = 1.0 / (yGridLineNumber - 1);
    
    for (int i=0; i < dataLength; ++i) {
        xDataNew.push_back( (xData[i] - xDataMin)/xDataRange );
        yDataNew.push_back( (yData[i] - yDataMin)/yDataRange );
    }

    plotSizeX = 1200.0;
    plotSizeY = 720.0;
    float halfPlotSizeX = plotSizeX/2.0;
    float halfPlotSizeY = plotSizeY/2.0;
    drawLine1->setPosition(Vec2(drawLine1->getPositionX() - halfPlotSizeX, drawLine1->getPositionY() - halfPlotSizeY));
    
    this->addChild(drawLine1, 1);
    
    for (int i = 0; i < xGridLineNumber; ++i) {
        double xSpace = i * xGridSpace * plotSizeX;
        cocos2d::Label * label = cocos2d::Label::createWithSystemFont("","Courier",20);
        label->setPosition(drawLine1->getPositionX() + xSpace, drawLine1->getPositionY() - 50);
        addChild(label);
        xLabels.push_back(label);
    }
    for (int i = 0; i < yGridLineNumber; ++i) {
        double ySpace = i * yGridSpace * plotSizeY;
        cocos2d::Label * label = cocos2d::Label::createWithSystemFont("","Courier",20);
        label->setPosition(drawLine1->getPositionX() - 50, drawLine1->getPositionY() + ySpace);
        addChild(label);
        yLabels.push_back(label);
    }
    
    functionLabel = cocos2d::Label::createWithSystemFont("","Courier",26);
    functionLabel->setPosition(xCenter, yCenter + halfPlotSizeY + 40);
    addChild(functionLabel);
    
    // Keyboard event listener
    auto eventListener = EventListenerKeyboard::create();
    
    Director::getInstance()->getOpenGLView()->setIMEKeyboardState(true);
    eventListener->onKeyPressed = [=](EventKeyboard::KeyCode keyCode, Event* event){
        // If a key already exists, do nothing as it will already have a time stamp. Otherwise, sets the timestamp to now
        ReleasedKeys.erase(keyCode);
        if(PressedKeys.find(keyCode) == PressedKeys.end()){
            PressedKeys[keyCode] = std::chrono::high_resolution_clock::now();
        }
    };
    eventListener->onKeyReleased = [=](EventKeyboard::KeyCode keyCode, Event* event){
        // remove the key.  std::map.erase() doesn't care if the key doesnt exist
        PressedKeys.erase(keyCode);
        ReleasedKeys[keyCode] = std::chrono::high_resolution_clock::now();
    };
    
    this->_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener,this);
    
    this->scheduleUpdate();
    
    totalTime = 0.0;
    return true;
}

void PlotScene::update(float deltaTime) {
    drawLine1->clear();
    totalTime += deltaTime;
    
    const double pi = 3.14159265359;
    
    for (int i=0; i < dataLength; ++i) { // Always update yData(t)
        yData[i] = yFunction(xData[i]);
        yDataNew[i] = (yData[i] - yDataMin)/yDataRange;
    }
    
    double timer = 0.0;
    double plotAxisTime = 6.0;
    if (totalTime < plotAxisTime) {
        timer = totalTime / plotAxisTime;
    } else {
        timer = 1.0;
        char ss[400];
        std::sprintf(ss, "y(x,t) = %.2f * sin( %.2f * (0.1 * x + t) ) * exp(-0.5 * (x - 50)^2/15^2)", amplitude, frequency );
        functionLabel->setString(ss);
    }
    drawLine1->drawSegment(Vec2(0, 0), Vec2(timer * plotSizeX, 0), 1, Color4F::WHITE);
    drawLine1->drawSegment(Vec2(0, 0), Vec2(0, timer * plotSizeY), 1, Color4F::WHITE);

    // Draw x grid lines
    for(int i = 0; i < xGridLineNumber; ++i) {
        double xSpace = i * xGridSpace * plotSizeX;
        double xScale = double(i) / xGridLineNumber;
        double xTimer;
        if (xScale < timer) {
            xTimer = (1/(1-xScale)) * timer - xScale/(1-xScale);
            
            cocos2d::Label* curLabel = xLabels[i]; // Show x labels
            char ss[50];
            std::sprintf(ss, "%.1f", i * xDataRange * xGridSpace + xDataMin );
            curLabel->setString(ss);

        } else {
            xTimer = 0.0;
        }
        drawDashedLine(Vec2(xSpace, 0), Vec2(xSpace, xTimer * plotSizeY), 10.0);
    }
    // Draw y grid lines
    for(int i = 0; i < yGridLineNumber; ++i) {
        
        double ySpace = i * yGridSpace * plotSizeY;
        double yScale = double(i) / yGridLineNumber;
        double yTimer;
        if (yScale < timer) {
            yTimer = (1/(1-yScale)) * timer - yScale/(1-yScale);
            
            cocos2d::Label* curLabel = yLabels[i];
            char ss[50];
            std::sprintf(ss, "%.1f", i * yDataRange * yGridSpace + yDataMin );
            curLabel->setString(ss);
            
        } else {
            yTimer = 0.0;
        }
        drawDashedLine(Vec2(0, ySpace), Vec2(yTimer * plotSizeX, ySpace), 10.0);
    }
    
    // KEYBOARD ACTIONS
    performKeyActions(deltaTime);
//    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
//        totalTime -= deltaTime;
//    }
    
    for(int i = 0; i < dataLength; ++i){
        double xPixel = xDataNew[i] * plotSizeX;
        double yPixel = yDataNew[i] * plotSizeY;
        double phase = i*i * 2.0*pi/dataLength;
        drawLine1->drawDot(Vec2(xPixel + 1000.0*(1.0-timer*timer)*sin(2*pi*timer+phase), yPixel + 1000.0*(1.0-timer*timer)*cos(2*pi*timer+phase)), 6, Color4F(yDataNew[i], 0.4, (1.0 - yDataNew[i])/2.0, 1.0));
    }
    
}

double PlotScene::yFunction(double xCurData) {
    //const double pi = 3.14159265359;
    //double xSum = std::accumulate(xData.begin(), xData.end(), 0.0);
    double xMean = 50.0;//xSum / xData.size();
    //double xSq_sum = std::inner_product(xData.begin(), xData.end(), xData.begin(), 0.0);
    double xStdev = 15.0;//std::sqrt(xSq_sum / xData.size() - xMean * xMean);
    
    double yCurData = amplitude * sin(frequency * (0.1*xCurData + totalTime)) * exp( -1*pow(xCurData - xMean,2)/(2*pow(xStdev,2)) );
    return yCurData;
}

bool PlotScene::isThisKeyPressed(EventKeyboard::KeyCode code) {
    // Check if the key is currently pressed by seeing it it's in the std::map keys
    if(PressedKeys.find(code) != PressedKeys.end())
        return true;
    return false;
}

void PlotScene::performKeyActions(float deltaTime) {
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
        frequency = frequency + 4.0 * deltaTime;
        totalTime -= deltaTime;
    }
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
        frequency = frequency - 4.0 * deltaTime;
        totalTime -= deltaTime;
    }
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
        amplitude = amplitude + 5.0 * deltaTime;
    }
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
        amplitude = amplitude - 5.0 * deltaTime;
    }
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_SPACE)) {
        totalTime = 0.0;
    }
}


void PlotScene::drawDashedLine(cocos2d::Vec2 origin, cocos2d::Vec2 destination, double dashLength)
{
    cocos2d::Vec2 delta = destination - origin;
    float dist = origin.getDistance(destination);
    float x = delta.x / dist * dashLength;
    float y = delta.y / dist * dashLength;
    float linePercentage = 0.5f;
    
    cocos2d::Vec2 p1 = origin;
    for(float i = 0; i <= dist / dashLength * linePercentage; i++) {
        cocos2d::Vec2 p2 = Vec2(p1.x + x, p1.y + y);
        drawLine1->drawSegment(p1, p2, 1, Color4F::WHITE);
        p1 = Vec2(p1.x + x / linePercentage, p1.y + y / linePercentage);
    }
}