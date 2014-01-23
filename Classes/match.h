//
//  match.h
//  MyGame
//
//  Created by ricky stone on 14-1-15.
//
//

#ifndef MyGame_match_h
#define MyGame_match_h

#include "CCLayer.h"

#include "cocos2d.h"

#define ROW 8
#define COL 8


enum  colorSpriteEnum{
     kA,
     kB,
     kC,
     kD,
     kE,
     kUnSigned,
};

class match :  public cocos2d::Layer
{
public:
    match();
    ~match();
    
    static match* create();
    bool init();

    
    virtual void onEnter();
    virtual void onExit();
    
    //void onTouchesBegan(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    //void onTouchesMoved(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    //void onTouchesEnded(const std::vector<cocos2d::Touch*>& touches, cocos2d::Event *event);
    
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
    
    void detectSprite(cocos2d::Touch* touch);
    
    void findSameColorsSprite(int r, int c, std::vector<cocos2d::Point>& collection);
    
    void processRC(std::queue<cocos2d::Point>& queues, std::vector<cocos2d::Point>& collection, colorSpriteEnum curKc, int r, int c);
    bool checkBundary(int r, int c);
    bool checkIfContain(const std::vector<cocos2d::Point>& colleciton, int r, int c);
    
    void moveRowBoxes(int c);
    void freshboxes(float dt);
    void mergeBoxes();
    
    void showDestroyBoxes();
    
    void addBoxes(int c);
    
    void randomSprite(cocos2d::Color3B& color3B, colorSpriteEnum& randomEnum);
    
    
    void processTouchBoxes();
    void moveBoxes();
    void bornBoxes();
    void resetCanbeTouch();
private:
    void createRowSprites(cocos2d::Point startPt, int row);
    void createRowsSprite(cocos2d::Point start,int rows);
    
private:
    colorSpriteEnum _rc[ROW][COL];
    cocos2d::Point _boxesPos[ROW][COL];
    cocos2d::Sprite* _rcSprites[ROW][COL];
    cocos2d::Vector<cocos2d::Sprite*> _destroyBoxes;
    bool _canbTouch;
};

#endif
