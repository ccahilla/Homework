//
//  BattleScene.cpp
//  gamename
//
//  Created by Craig Cahillane on 5/21/16.
//
//

#include "BattleScene.h"
#include "GridLayer.h"

USING_NS_CC;

cocos2d::Scene* BattleScene::createScene()
{
    auto scene = Scene::create();
    auto playerLayer = BattleScene::create();
    auto gridLayer1 = GridLayer::create();
    scene->addChild(playerLayer, 0);
    scene->addChild(gridLayer1 , 200);
    playerLayer->gridLayer = gridLayer1;
    
    return scene;
}

bool BattleScene::init() {
    if ( !Layer::init() ) {
        return false;
    }
    totalTime = 0.0;
    
    upCanBePressed = 1;
    downCanBePressed = 1;
    rightCanBePressed = 1;
    leftCanBePressed = 1;
    
    angVelocity = 5.0;
    angAcceleration = 1.0;
    velocityX = 0.0;
    velocityY = 0.0;
    accelerationX = 110.0;
    accelerationY = 110.0;
    phase = 0.0;
    
    healthBarMax = 20;
    currentHealth = healthBarMax;
    currentBoxBar = healthBarMax;
    noAttackInitialized = true;
    lastAttackHit = 0;
    attackTiming = 0.0;
    attackPositionX = 0;
    attackPositionY = 0;
    attackTimeLength = 2.0;
    phiSign = 0;
    
    gridNumX = 13;
    gridNumY = 7;
    gridSquareLength = 30.0;
    for(int i = 0; i < maxGridDim; ++i) {
        for(int j = 0; j < maxGridDim; ++j) {
            int ii = i - halfGridDim; int jj = j - halfGridDim;
            
            if(abs(ii) <= (gridNumX-1)/2 && abs(jj) <= (gridNumY-1)/2) { // Always for odd and even grids
                gridBoard[i][j] = 1;
            } else if( gridNumX % 2 == 0 && ii == gridNumX/2 && abs(jj) <= (gridNumY-1)/2 ) {  //If X dim is even
                gridBoard[i][j] = 1;
            } else if( gridNumY % 2 == 0 && jj == gridNumY/2 && abs(ii) <= (gridNumX-1)/2 ) {  //If Y dim is even
                gridBoard[i][j] = 1;
            } else if( gridNumX % 2 == 0 && ii == gridNumX/2 && gridNumY % 2 == 0 && jj == gridNumY/2 ) { //If X and Y dims are even
                gridBoard[i][j] = 1;
            } else {
                gridBoard[i][j] = 0;
            }
            for(int k = 0; k < gridPolygonVertices; ++k) {
                gridBoardX[i][j][k] = 0; // Initialize everything to zero
                gridBoardY[i][j][k] = 0;
            }
        }
    }
    
    drawPlayerGrid = DrawNode::create();
    healthBar = DrawNode::create();
    attackNode = DrawNode::create();
    enemyBase = DrawNode::create();
    playerCursor = DrawNode::create();
    playerCube = DrawNode::create();
    
    xCenter = this->getBoundingBox().getMidX();
    yCenter = this->getBoundingBox().getMidY();
    xMax = this->getBoundingBox().getMaxX();
    yMax = this->getBoundingBox().getMaxY();
    xMin = this->getBoundingBox().getMinX();
    yMin = this->getBoundingBox().getMinY();
    
//    cocos2d::log("\nBattle Scene");
//    cocos2d::log("xMax = %d, yMax = %d", xMax, yMax);
//    cocos2d::log("xMin = %d, yMin = %d", xMin, yMin);
//    cocos2d::log("xCenter = %d, yCenter = %d", xCenter, yCenter);
    
    positionX = xCenter;
    positionY = yCenter;
    
    drawPlayerGrid->setPosition(positionX, positionY / 2);
    healthBar->setPosition(positionX, positionY);
    attackNode->setPosition(positionX, positionY);
    enemyBase->setPosition(positionX, positionY);
    playerCursor->setPosition(drawPlayerGrid->getPosition());
    playerCube->setPosition(positionX / 2 , positionY);
    
    score = 0;
    std::stringstream ss;
    ss << score;
    scoreLabel = cocos2d::Label::createWithSystemFont(ss.str().c_str(),"Arial",32);
    scoreLabel->setPosition(0.9 * (xMax - xMin), 0.9 * (yMax - yMin));
    
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
    
    this->addChild(scoreLabel, 1);
    this->addChild(enemyBase, 4);
    this->addChild(attackNode, 3);
    this->addChild(drawPlayerGrid, 5);
    this->addChild(healthBar, 5);
    this->addChild(playerCursor, 1);
    this->addChild(playerCube, 10);
    
    
    this->scheduleUpdate();
    return true;
}

bool BattleScene::isAnyKeyPressed() {
    return !PressedKeys.empty();
}

bool BattleScene::isThisKeyPressed(EventKeyboard::KeyCode code) {
    // Check if the key is currently pressed by seeing it it's in the std::map keys
    if(PressedKeys.find(code) != PressedKeys.end())
        return true;
    return false;
}

bool BattleScene::isThisKeyReleased(EventKeyboard::KeyCode code) {
    // Check if the key is currently pressed by seeing it it's in the std::map keys
    // In retrospect, keys is a terrible name for a key/value paried datatype isnt it?
    if(ReleasedKeys.find(code) != ReleasedKeys.end())
        return true;
    return false;
}

double BattleScene::keyPressedDuration(EventKeyboard::KeyCode code) {
    return std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - PressedKeys[code]).count();
}
double BattleScene::keyReleasedDuration(EventKeyboard::KeyCode code) {
    return std::chrono::duration_cast<std::chrono::milliseconds> (std::chrono::high_resolution_clock::now() - ReleasedKeys[code]).count();
}

void BattleScene::performKeyActions(float deltaTime) {
    // Arrow Keys move drawGrid center around the board
    // Space Bar fires off a grid square into distance
    double xPos, yPos;
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW) && rightCanBePressed == 1) {
        xPos = playerCursor->getPositionX() + gridSquareLength;
        rightCanBePressed = 0;
        playerCursor->setPosition(xPos, playerCursor->getPositionY());
    }
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW) && leftCanBePressed == 1) {
        xPos = playerCursor->getPositionX() - gridSquareLength;
        leftCanBePressed = 0;
        playerCursor->setPosition(xPos, playerCursor->getPositionY());
    }
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_UP_ARROW) && upCanBePressed == 1) {
        yPos = playerCursor->getPositionY() + gridSquareLength;
        upCanBePressed = 0;
        playerCursor->setPosition(playerCursor->getPositionX(), yPos);
    }
    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW) && downCanBePressed == 1) {
        yPos = playerCursor->getPositionY() - gridSquareLength;
        downCanBePressed = 0;
        playerCursor->setPosition(playerCursor->getPositionX(), yPos);
    }
    //    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_Z) && keyZCanBePressed == 1) {
    //        keyZCanBePressed = 0;
    //        --currentBoxBar;
    //    }
    //    if(isThisKeyPressed(EventKeyboard::KeyCode::KEY_SPACE) && spaceCanBePressed == 1) {
    //        spaceCanBePressed = 0;
    //        int iRemove = 0; int jRemove = 0;
    //        int iRef = 0; int jRef = 0;
    //        for(int i = 0; i < maxGridDim; ++i) {
    //            for(int j = 0; j < maxGridDim; ++j) {
    //                if(gridBoard[i][j] != 0) {
    //                    double ii = i - (maxGridDim-1.0)/2.0;
    //                    double jj = j - (maxGridDim-1.0)/2.0;
    //                    if (abs(ii) + abs(jj) > abs(iRef) + abs(jRef)) {
    //                        iRef = ii;
    //                        jRef = jj;
    //                        iRemove = i;
    //                        jRemove = j;
    //                    }
    //                }
    //            }
    //        }
    //        gridBoard[iRemove][jRemove] = 0;
    //    }
    
    return;
}

void BattleScene::update(float deltaTime) {
    enemyBase->clear();
    attackNode->clear();
    drawPlayerGrid->clear();
    healthBar->clear();
    playerCube->clear();
    
    totalTime += deltaTime;
    
    if(isAnyKeyPressed()) {
        performKeyActions(deltaTime);
    }
    
    if( totalTime < gridLayer->getGridDrawingTime() ) {
        gridLayer->update(deltaTime);
    }
    
    //    double friction = 0.95;
    //    velocityX -= copysign(1.0,velocityX) * friction;
    //    velocityY -= copysign(1.0,velocityY) * friction;
    //    if(!isThisKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW) && !isThisKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW) &&
    //       !isThisKeyPressed(EventKeyboard::KeyCode::KEY_UP_ARROW) && !isThisKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) { // If no arrow key is pressed, stop drifting
    //        if (abs(velocityX) < 0.00001) {
    //            velocityX = 0.0;
    //        }
    //        if (abs(velocityY) < 0.00001) {
    //            velocityY = 0.0;
    //        }
    //    }
    // Debounce keys
    if(!isThisKeyPressed(EventKeyboard::KeyCode::KEY_LEFT_ARROW)) {
        leftCanBePressed = 1;
    }
    if(!isThisKeyPressed(EventKeyboard::KeyCode::KEY_RIGHT_ARROW)) {
        rightCanBePressed = 1;
    }
    if(!isThisKeyPressed(EventKeyboard::KeyCode::KEY_UP_ARROW)) {
        upCanBePressed = 1;
    }
    if(!isThisKeyPressed(EventKeyboard::KeyCode::KEY_DOWN_ARROW)) {
        downCanBePressed = 1;
    }
    
    //Begin attack update
    basicAttack(deltaTime);
    
    drawEnemyBase(deltaTime);
    
    drawHealthBar(deltaTime);
    
    drawPlayerCursor(deltaTime);
    
    drawPlayerCube(deltaTime);
    
    std::stringstream ss;
    ss << score;
    scoreLabel->setString(ss.str().c_str());
    
    // Grid movement
    //    double xPos = drawPlayerGrid->getPositionX() + velocityX;
    //    double yPos = drawPlayerGrid->getPositionY() + velocityY;
    //    if(xPos > xMax) {
    //        xPos = xMax;
    //        velocityX = -velocityX/3.0;
    //    } else if (xPos < xMin){
    //        xPos = xMin;
    //        velocityX = -velocityX/3.0;
    //    }
    //    if(yPos > yMax) {
    //        yPos = yMax;
    //        velocityY = -velocityY/3.0;
    //    } else if (yPos < yMin){
    //        yPos = yMin;
    //        velocityY = -velocityY/3.0;
    //    }
    //    drawPlayerGrid->setPosition(xPos, yPos);
    
    drawMainGrid();
    
}

void BattleScene::drawPlayerCube(float deltaTime) {
    
    double rr = 200.0;
    double theta[8];
    double phi[8];
    double theta0 = pi/6.0;
    double phi0 = 0.0;
    // Populate cube angles
    for(int ii = 0; ii < 2; ++ii) {
        for(int jj = 0; jj < 4; ++jj) {
            int index = jj + 4 * ii;
            theta[index] = theta0 + double(ii) * pi/2.0;// + 0.5 * totalTime;
            phi[index]   = phi0 + double(jj) * pi/2.0;// + 0.3 * totalTime;
        }
    }
    // Cartesian Coordinates
    double xx[8];
    double yy[8];
    double zz[8];
    double alpha = totalTime;
    double beta = 0.4 * totalTime;
    double gamma = -0.1 * totalTime;
    double rotationMatrix[3][3] =
    {{ cos(beta)*cos(gamma), cos(gamma)*sin(alpha)*sin(beta) + cos(alpha)*sin(gamma), -cos(alpha)*cos(gamma)*sin(beta) + sin(alpha)*sin(gamma)},
        {-cos(beta)*sin(gamma), cos(alpha)*cos(gamma) - sin(alpha)*sin(beta)*sin(gamma), cos(gamma)*sin(alpha) + cos(alpha)*sin(beta)*sin(gamma)},
        { sin(beta), -cos(beta)*sin(alpha), cos(alpha)*cos(beta)}};
    double xxPrime[8];
    double yyPrime[8];
    Point playerCubeVerticies[8];
    
    for(int ii = 0; ii < 8; ++ii) {
        xx[ii] = rr * sin(theta[ii]) * cos(phi[ii]);
        yy[ii] = rr * sin(theta[ii]) * sin(phi[ii]);
        zz[ii] = rr * cos(theta[ii]);
        
        xxPrime[ii] = rotationMatrix[0][0] * xx[ii] + rotationMatrix[0][1] * yy[ii] + rotationMatrix[0][2] * zz[ii];
        yyPrime[ii] = rotationMatrix[1][0] * xx[ii] + rotationMatrix[1][1] * yy[ii] + rotationMatrix[1][2] * zz[ii];
        
        playerCubeVerticies[ii] = Point(xxPrime[ii], yyPrime[ii]);
    }
    int edgeMatrix[8][8] = {{0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 0, 0, 1, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 0}};
    // Populate cube angles
    Color4F playerCubeColor = Color4F(0.8, 0.3, 0.3, 0.5); // Dark Red
    for(int ii = 0; ii < 8; ++ii) {
        for(int jj = 0; jj < 8; ++jj) {
            if(edgeMatrix[ii][jj] == 1) {
                playerCube->drawSegment(playerCubeVerticies[ii], playerCubeVerticies[jj], 1.0, playerCubeColor);
            }
        }
        playerCube->drawSegment(Point(0.0, 0.0), playerCubeVerticies[ii], 1.0, playerCubeColor);
    }
    return;
}

void BattleScene::drawPlayerCursor(float deltaTime) {
    
    Color4F playerCursorColorFill = Color4F(0.3, 0.3, 0.8, 0.5);  // Dark Blue
    Color4F playerCursorColorBorder = Color4F(0.6, 0.6, 1.0, 1.0); // Lighter Blue
    Point playerCursorVerticies[6];
    double xx[6]; double yy[6];
    double theta[6];
    double rr = 25.0/sqrt(2);
    
    for(int i = 0; i < 4; ++i) {
        
        theta[i] = i * 2.0*pi/4.0 + pi/4.0;
        xx[i] = rr * cos(theta[i]);
        yy[i] = rr * sin(theta[i]);
        playerCursorVerticies[i] = Point(xx[i], yy[i]);
    }
    playerCursor->drawPolygon(playerCursorVerticies, 4, playerCursorColorFill, 1.0, playerCursorColorBorder);
}

void BattleScene::drawEnemyBase(float deltaTime) {
    // If we reach here, attack is initialized
    Color4F baseColorFill = Color4F(0.0, 0.6, 0.2, 0.5);
    Color4F baseColorBorder = Color4F(0.0, 0.6, 0.2, 1.0); // Dark Green
    double baseLen = 60.0;
    
    // Base is hex
    Point baseVerticies[6];
    double xx[6]; double yy[6];
    double rr; double theta[6];
    double dd; double phi;
    
    //    double finalDist = sqrt(pow(attackPositionX, 2.0) + pow(attackPositionY,2.0));
    //    double maxTime = 3.0/4.0*attackTimeLength;
    //    double aa = -13.0/12.0 * finalDist/pow(maxTime, 2.0);  double bb = -2*aa*maxTime;
    rr = baseLen;
    phi = 0.0;
    dd = 0.0;
    
    for(int i = 0; i < 6; ++i) {
        double scaler = 1.0;
        if(i % 2 == 0) {
            scaler = 0.20;
        }
        theta[i] = i * 2.0*pi/6.0 - 2.0*totalTime;
        xx[i] = dd * cos(phi) + scaler * rr * cos(theta[i]);
        yy[i] = dd * sin(phi) + scaler * rr * sin(theta[i]);
        baseVerticies[i] = Point(xx[i], yy[i]);
    }
    
    enemyBase->drawPolygon(baseVerticies, 6, baseColorFill, 1.0, baseColorBorder);
    
}

void BattleScene::drawMainGrid() {
    phase = 0;
    double floaterX = 0.0 * ( 0.0 + cos(1.0*totalTime));
    double floaterY = 0.0 * ( 0.0 + cos(1.0*totalTime));
    double spacer = gridSquareLength/2.0;
    
    for(int i = 0; i < gridNumX; ++i) {  //Use gridNumX because it is smaller
        for(int j = 0; j < gridNumY; ++j) {
            int iGrid = i + halfGridDim - (gridNumX-1)/2; int jGrid = j + halfGridDim - (gridNumY-1)/2;
            
            double ii = i - (gridNumX-1.0)/2.0; double jj = j - (gridNumY-1.0)/2.0;
            double xx[4]; double yy[4]; double rr[4]; double theta[4];
            double absdist = 1.0;//abs(ii)+abs(jj);
            xx[0] = (gridSquareLength*ii - spacer);         yy[0] = (gridSquareLength*jj - spacer);
            rr[0] = sqrt(pow(xx[0],2) + pow(yy[0],2));      theta[0] = absdist*phase + atan2(yy[0], xx[0]);
            xx[0] = ii*floaterX + rr[0] * cos(theta[0]);    yy[0] = jj*floaterY + rr[0] * sin(theta[0]);
            
            xx[1] = (gridSquareLength*ii - spacer);         yy[1] = (gridSquareLength*jj + spacer);
            rr[1] = sqrt(pow(xx[1],2) + pow(yy[1],2));      theta[1] = absdist*phase + atan2(yy[1], xx[1]);
            xx[1] = ii*floaterX + rr[1] * cos(theta[1]);    yy[1] = jj*floaterY + rr[1] * sin(theta[1]);
            
            xx[2] = (gridSquareLength*ii + spacer);         yy[2] = (gridSquareLength*jj + spacer);
            rr[2] = sqrt(pow(xx[2],2) + pow(yy[2],2));      theta[2] = absdist*phase + atan2(yy[2], xx[2]);
            xx[2] = ii*floaterX + rr[2] * cos(theta[2]);    yy[2] = jj*floaterY + rr[2] * sin(theta[2]);
            
            xx[3] = (gridSquareLength*ii + spacer);         yy[3] = (gridSquareLength*jj - spacer);
            rr[3] = sqrt(pow(xx[3],2) + pow(yy[3],2));      theta[3] = absdist*phase + atan2(yy[3], xx[3]);
            xx[3] = ii*floaterX + rr[3] * cos(theta[3]);    yy[3] = jj*floaterY + rr[3] * sin(theta[3]);
            
            if(gridBoard[iGrid][jGrid] == 0) {
                continue;
            }
            
            for(int k = 0; k < 4; ++k) {
                gridBoardX[iGrid][jGrid][k] = xx[k];  // Store the vertices of every box in the grid
                gridBoardY[iGrid][jGrid][k] = yy[k];
                int k2 = (k + 1) % 4;
                drawPlayerGrid->drawSegment(Vec2(xx[k], yy[k]), Vec2(xx[k2], yy[k2]), 1, Color4F(1.0, 1.0, 1.0, 1.0));
            }
        }
    }
}

void BattleScene::basicAttack(float deltaTime) {
    
    attackTiming += deltaTime;
    if(noAttackInitialized) {  // If there is currently no attack
        if(attackTiming > attackTimeLength) {  // If attackTiming is greater than 3 seconds
            noAttackInitialized = false;  // Initialize attack
            attackTiming = 0.0;
            attackPositionX = cocos2d::RandomHelper::random_int(xMin+20, xMax-20) - (xMax - xMin)/2.0;
            attackPositionY = cocos2d::RandomHelper::random_int(yMin+20, yMax-20) - (yMax - yMin)/2.0;
            phiSign = cocos2d::RandomHelper::random_int(-1, 1);
        }
        return;
    }
    // If we reach here, attack is initialized
    Color4F attackColorFill = Color4F(1.0, 0.5, 0.0, 0.5);
    Color4F attackColorBorder = Color4F(1.0, 0.5, 0.0, 1.0); // Orange
    
    double crslen = 15.0;
    attackNode->drawSegment(Vec2(attackPositionX - crslen, attackPositionY - crslen), Vec2(attackPositionX + crslen, attackPositionY + crslen), 1, attackColorBorder);
    attackNode->drawSegment(Vec2(attackPositionX + crslen, attackPositionY - crslen), Vec2(attackPositionX - crslen, attackPositionY + crslen), 1, attackColorBorder);
    
    Point attackSquareVerticies[3];
    double xx[3]; double yy[3];
    double rr; double theta[3];
    double dd; double phi;
    
    double finalDist = sqrt(pow(attackPositionX, 2.0) + pow(attackPositionY,2.0));
    double maxTime = 3.0/4.0*attackTimeLength;
    double aa = -13.0/12.0 * finalDist/pow(maxTime, 2.0);  double bb = -2*aa*maxTime;
    rr = 5.0 + crslen * attackTiming / attackTimeLength;
    phi = atan2(attackPositionY, attackPositionX) + 0.1*phiSign*sin(pi * attackTiming / attackTimeLength);
    dd = aa * pow(attackTiming,2.0) + bb * attackTiming;
    
    for(int i=0; i < 3; ++i) {
        theta[i] = i * 2.0*pi/3.0 + 2.0*totalTime;
        xx[i] = dd * cos(phi) + rr * cos(theta[i]);
        yy[i] = dd * sin(phi) + rr * sin(theta[i]);
        attackSquareVerticies[i] = Point(xx[i], yy[i]);
    }
    
    attackNode->drawPolygon(attackSquareVerticies, 3, attackColorFill, 1.0, attackColorBorder);
    
    if(attackTiming > attackTimeLength) {
        checkIfAttackHits();
        noAttackInitialized = true;
    }
    
    return;
}

void BattleScene::checkIfAttackHits() {
    int attackHit = 1;  //Start off with attack hitting
    int deflectionSquareIndexI = -1;
    int deflectionSquareIndexJ = -1;
    float gridPosX; float gridPosY;
    float attackNodePosX; float attackNodePosY;
    //double attackGridCenterDist; double attackGridCenterTheta;
    
    drawPlayerGrid->getPosition(&gridPosX, &gridPosY);
    attackNode->getPosition(&attackNodePosX, &attackNodePosY);
    
    //Is attack covered by a grid square?
    for(int i = 0; i < gridNumX; ++i) {
        for(int j = 0; j < gridNumY; ++j) {
            int ii = i + halfGridDim - (gridNumX-1)/2; int jj = j + halfGridDim - (gridNumY-1)/2;
            if(gridBoard[ii][jj] == 1) {  // If a grid square exists
                
                int withinBoxCount = 0;
                for(int kk = 0; kk < 4; ++kk) {
                    int pk = (kk - 1) % 4;  // Prev Vertex
                    int nk = (kk + 1) % 4;  // Next Vertex
                    if(pk < 0) pk = 3;
                    
                    // Use the angles of the lines to determine whether the attack is inside the given grid box
                    double angle1; double angle2; double angleAttack; //angleAttack must be between angle1 and angle2
                    angle1 = atan2(gridBoardY[ii][jj][pk] - gridBoardY[ii][jj][kk], gridBoardX[ii][jj][pk] - gridBoardX[ii][jj][kk]);
                    angle2 = atan2(gridBoardY[ii][jj][nk] - gridBoardY[ii][jj][kk], gridBoardX[ii][jj][nk] - gridBoardX[ii][jj][kk]);
                    angleAttack = atan2(attackNodePosY + attackPositionY - gridPosY - gridBoardY[ii][jj][kk], attackNodePosX + attackPositionX - gridPosX - gridBoardX[ii][jj][kk] );
                    
                    if( abs(angle2 - angle1 + 3.0*pi/2.0) < 0.01 ) {  //If we are dealing with a phase wrapping issue about pi (getting -270 instead of 90 degrees)
                        angle2 += 2.0*pi; // Correct it such that the same code works below by adding 2*pi to everything negative
                        if(angleAttack < 0.0) {
                            angleAttack += 2.0*pi;
                        }
                    }
                    
                    if(angle1 <= angleAttack && angleAttack <= angle2) {
                        ++withinBoxCount;
                    }
                }
                if(withinBoxCount == 4) {  // If every pair of angles 1 and 2 has angleAttack inside of it
                    attackHit = 0;  // The attack misses
                    ++score;
                    deflectionSquareIndexI = ii;
                    deflectionSquareIndexJ = jj;
                    break;
                }
            }
            if(attackHit == 0) {
                break;
            }
        }
        if(attackHit == 0) {
            break;
        }
    }
    
    if(attackHit == 1) {
        currentHealth = currentHealth - 1;
        lastAttackHit = 1;
    } else {
        lastAttackHit = 0;
    }
    cocos2d::log("attackHit = %d", attackHit);
    return;
}

void BattleScene::drawHealthBar(float deltaTime) {
    double healthBarPosX = 0.0;
    double healthBarPosY = -0.8 * (yMax - yMin)/2;
    
    double healthBarBoxLength = 20.0;
    double healthBarSpacing = 5.0;
    
    for(int i=0; i < healthBarMax; ++i) {
        double currentPosX = healthBarPosX + (healthBarBoxLength + healthBarSpacing) * (i - (healthBarMax-1)/2);
        double xx[4]; double yy[4];
        Point healthSquareVerticies[4];
        double borderWidth = 1.0;
        
        Color4F healthColorFill;
        Color4F healthColorBorder;
        if(i < currentHealth) {
            healthColorFill = Color4F(0.7, 0.0, 0.0, 1.0);
        } else {
            healthColorFill = Color4F(0.0, 0.0, 0.0, 1.0);
        }
        if(i < currentBoxBar) {
            healthColorBorder = Color4F(0.75, 0.75, 0.75, 1.0);
        } else {
            healthColorBorder = Color4F(0.25, 0.25, 0.25, 1.0);
        }
        
        xx[0] = currentPosX - healthBarBoxLength/2.0;
        yy[0] = healthBarPosY - healthBarBoxLength/2.0;
        xx[1] = currentPosX - healthBarBoxLength/2.0;
        yy[1] = healthBarPosY + healthBarBoxLength/2.0;
        xx[2] = currentPosX + healthBarBoxLength/2.0;
        yy[2] = healthBarPosY + healthBarBoxLength/2.0;
        xx[3] = currentPosX + healthBarBoxLength/2.0;
        yy[3] = healthBarPosY - healthBarBoxLength/2.0;
        for(int j=0; j < 4; ++j) {
            healthSquareVerticies[j] = Point(xx[j], yy[j]);
        }
        healthBar->drawPolygon(healthSquareVerticies, 4, healthColorFill, borderWidth, healthColorBorder);
    }
    
    
    return;
}

std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> BattleScene::PressedKeys;
std::map<cocos2d::EventKeyboard::KeyCode, std::chrono::high_resolution_clock::time_point> BattleScene::ReleasedKeys;

