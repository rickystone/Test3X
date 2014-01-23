//
//  match.cpp
//  MyGame
//
//  Created by ricky stone on 14-1-15.
//
//

#include "match.h"
#include "CCSprite.h"
#include "cocos2d.h"

USING_NS_CC;

#define MOVEDURATION 0.01f

match::match() :_canbTouch(true)
{}

match::~match() {}

match* match::create()
{
    match* pret = new match;
    pret->autorelease();
    pret->init();
    return pret;
}

bool match::init()
{
    auto s = Director::getInstance()->getWinSize();
    auto draw = DrawNode::create();
    addChild(draw, 10);
    
    for(int r = 0; r<ROW; r++){
        for(int c=0; c<COL; c++){
            _rc[r][c] = kUnSigned;
            _boxesPos[r][c] = Point(-1,-1);
            _rcSprites[r][c] = nullptr;
        }
    }
    
    Point start = Point(10+8, s.height*0.2+8);
    createRowsSprite(start, 10);
    return true;
}

void match::createRowSprites(cocos2d::Point start, int r)
{
    auto sprite = Sprite::create("Icon.png");
    Point prevPt = Point(start.x, start.y + r*sprite->getContentSize().height + 2);
    for(int c=0; c<COL; c++)
    {
        auto sprite = Sprite::create("Icon.png");
        Color3B color3B;
        colorSpriteEnum randomEnum;
        randomSprite(color3B,randomEnum);
        
        sprite->setColor(color3B);
        _rc[r][c] = randomEnum;
        
        addChild(sprite);
        sprite->setTag(r*ROW+c);
        sprite->setPosition(Point(prevPt.x + sprite->getContentSize().width + 2, prevPt.y));
        prevPt = sprite->getPosition();
        
        _boxesPos[r][c] = sprite->getPosition();
        _rcSprites[r][c] = sprite;
        sprite->retain();
    }
}

void match::createRowsSprite(Point start, int n)
{
    auto s = Director::getInstance()->getWinSize();
    for(int r = 0; r < ROW; r++)
    {
        createRowSprites(start, r);
    }
}

void match::detectSprite(cocos2d::Touch *touch)
{
    Point location = touch->getLocation();
    for(int r=0; r<ROW; r++){
        for(int c=0; c<COL; c++){
            Sprite* sprite = _rcSprites[r][c];
            if(sprite)
            {
                Point point  = sprite->convertToNodeSpace(location);
                auto rect = Rect(0, 0, sprite->getContentSize().width, sprite->getContentSize().height);
                bool contain = rect.containsPoint(point);
                if(contain)
                {
                    CCLOG("fuck touch it r=%d, c=%d\n", r, c);
                    std::vector<Point> collections;
                    findSameColorsSprite(r, c, collections);
                    break;
                }
            }
        }
    }
}

void match::findSameColorsSprite(int r, int c, std::vector<Point>& collection)
{
    colorSpriteEnum curKc = _rc[r][c];
    std::queue<Point> queues;
    queues.push(Point(r,c));
    
    collection.push_back(Point(r,c));
    
    while (queues.size() > 0) {
        Point pt = queues.front();
        queues.pop();
        int r = pt.x;
        int c = pt.y;
        
        processRC(queues, collection, curKc, r-1, c);
        processRC(queues, collection, curKc, r+1, c);
        processRC(queues, collection, curKc, r, c+1);
        processRC(queues, collection, curKc, r, c-1);
    }
    
    for(int i=0; i<collection.size(); i++)
    {
        Point pt = collection[i];
        int r = (int)pt.x;
        int c = (int)pt.y;
        
        CCLOG("r=%d, c=%d\n", r, c);
        _rc[r][c] = kUnSigned;
        _rcSprites[r][c]->removeFromParentAndCleanup(true);
        CC_SAFE_RELEASE_NULL(_rcSprites[r][c]);
    }
    
    processTouchBoxes();
}

void match::processRC(std::queue<Point>& queues, std::vector<Point>& collection, colorSpriteEnum curKc, int r, int c)
{
    bool check_bool = checkBundary(r, c);
    if(check_bool)
    {
        Point pt(r,c);
        colorSpriteEnum Kc = _rc[(int)r][(int)c];
        if(curKc == Kc)
        {
            bool check_contain = checkIfContain(collection, r, c);
            if(!check_contain)
            {
                collection.push_back(pt);
                queues.push(pt);
            }
        }
    }
}

bool match::checkBundary(int r, int c)
{
    if(r<0 || r>=ROW)
        return false;
    
    if(c<0 || c>=COL)
        return false;
    
    return true;
}

bool match::checkIfContain(const std::vector<Point>& colleciton, int r, int c)
{
    Point pt(r,c);
    const std::vector<Point>::const_iterator it = std::find(colleciton.cbegin(), colleciton.cend(), pt);
    if(it == colleciton.cend())
        return false;
    return true;
}

void match::processTouchBoxes()
{
    auto act1 = CallFunc::create( std::bind(&match::moveBoxes, this));
    auto delay = DelayTime::create(0.2f);
    auto act2 = CallFunc::create( std::bind(&match::bornBoxes, this));
    auto sequence = Sequence::create(act1,delay,act2,nullptr);
    this->runAction(sequence);
}

void match::moveBoxes()
{
    for(int c=0; c<COL; c++)
       moveRowBoxes(c);
}

void match::bornBoxes()
{
    for(int c=0; c<COL; c++)
       addBoxes(c);
}

void match::resetCanbeTouch()
{
    _canbTouch = true;
}

void match::moveRowBoxes(int c)
{
    int r=0;
    
    //find first space
    for(; r<ROW; r++)
    {
        if(_rc[r][c] == kUnSigned)
            break;
    }
    
    if(r >= ROW)
       return;
    
    //find how many space
    int cnt = 1;
    int prevR = r;
    while(r<ROW && _rc[r][c] == kUnSigned)
    {
        r = r+1;
        cnt = cnt+1;
    }
    
    //break while
    cnt -= 1;
    r = prevR+cnt;
    
    Point destinations[ROW-2];
    for(int xr = r, i=0; xr < ROW; xr++, i++)
    {
        destinations[i] = _boxesPos[xr-cnt][c];
    }
    
    //for bottom to top, move sprite down to dest position
    for(int i=0; r<ROW; r++, i++)
    {
        Sprite* sprite = _rcSprites[r][c];
        if(sprite)
        {
            Point pt = destinations[i];
            MoveTo *moveto = MoveTo::create(MOVEDURATION, pt);
            sprite->runAction(moveto);
            
            _rc[r-cnt][c] = _rc[r][c];
            _rc[r][c] = kUnSigned;
            
            _rcSprites[r-cnt][c] = _rcSprites[r][c];
            _rcSprites[r][c] = nullptr;
        }
    }
}

void match::addBoxes(int c)
{
    int cnt = 0;
    for(int r=ROW-1; r>=0; r--)
    {
        if(_rc[r][c] == kUnSigned)
            cnt += 1;
    }
    
    for(int x=0; x<cnt; x++)
    {
        int r = ROW-1-x;
        Point pt = _boxesPos[r][c];
        
        auto sprite = Sprite::create("Icon.png");
        Color3B color3B;
        colorSpriteEnum randomEnum;
        randomSprite(color3B,randomEnum);
        
        sprite->setColor(color3B);
        _rc[r][c] = randomEnum;
        
        addChild(sprite);
        sprite->setTag(r*ROW+c);
        sprite->setPosition(pt);
        _rcSprites[r][c] = sprite;
        sprite->retain();
        
        sprite->setPosition(Point(pt.x, pt.y + 100));
        MoveTo* moveto = MoveTo::create(MOVEDURATION, pt);
        sprite->runAction(moveto);
    }
}

void match::mergeBoxes()
{
    int c = 0;
    for(; c<COL; c++)
    {
        if(_rc[0][c] == kUnSigned)
            break;
    }
    if(c >= COL)
        return;
}

void match::showDestroyBoxes()
{
    for(int i=0 ;i<_destroyBoxes.size(); i++)
    {
        Sprite* ss = _destroyBoxes.at(i);
        if(ss)
        {
            ss->removeFromParentAndCleanup(true);
        }
    }
    _destroyBoxes.clear();
    
    for(int r=0; r<ROW; r++){
        for(int c=0; c<COL; c++){
            colorSpriteEnum kk = _rc[r][c];
            if(kk == kUnSigned)
            {
                Point pt = _boxesPos[r][c];
                Sprite *ss = Sprite::create("circle.png");
                addChild(ss);
                _destroyBoxes.pushBack(ss);
                ss->setPosition(pt);
            }
        }
    }
}

void match::freshboxes(float dt)
{
    unschedule(schedule_selector(match::freshboxes));
    for(int c=0; c<COL; c++){
    }
}


void match::randomSprite(Color3B& color3B, colorSpriteEnum& randomEnum)
{
    int idc = CCRANDOM_0_1()*5+1;
    
    const static Color3B WHITE;
    const static Color3B YELLOW;
    const static Color3B BLUE;
    const static Color3B GREEN;
    const static Color3B RED;
    const static Color3B MAGENTA;
    const static Color3B BLACK;
    const static Color3B ORANGE;
    const static Color3B GRAY;
    
    switch (idc) {
        case 1:
            color3B = Color3B::YELLOW;
            randomEnum = kA;
            break;
        case 2:
            color3B = Color3B::BLUE;
            randomEnum = kB;
            break;
        case 3:
            color3B = Color3B::GREEN;
            randomEnum = kC;
            break;
        case 4:
            color3B = Color3B::RED;
            randomEnum = kD;
            break;
        case 5:
            color3B = Color3B::MAGENTA;
            randomEnum = kE;
            break;
        default:
            break;
    }
    return;
}

void match::onEnter()
{
    Layer::onEnter();
    
    //Register Touch Event
    //auto listener = EventListenerTouchAllAtOnce::create();
    //listener->onTouchesBegan = CC_CALLBACK_2(match::onTouchesBegan, this);
    //listener->onTouchesMoved = CC_CALLBACK_2(match::onTouchesMoved, this);
    //listener->onTouchesEnded = CC_CALLBACK_2(match::onTouchesEnded, this);
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(match::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(match::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(match::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void match::onExit()
{
    //auto director = Director::getInstance();
    //director->getTouchDispatcher()->removeDelegate(this);
    Layer::onExit();
}

/*
void match::onTouchesBegan(const std::vector<Touch*>& touches, Event  *event)
{
    Touch *touch = touches[0];
    CCLOG("Paddle::onTouchBegan id = %d, x = %f, y = %f", touch->getID(), touch->getLocation().x, touch->getLocation().y);
}

void match::onTouchesMoved(const std::vector<Touch*>& touches, Event *event)
{
    Touch *touch = touches[0];
    CCLOG("Paddle::onTouchBegan id = %d, x = %f, y = %f", touch->getID(), touch->getLocation().x, touch->getLocation().y);
}

void match::onTouchesEnded(const std::vector<Touch*>& touches, Event  *event)
{
    Touch *touch = touches[0];
    CCLOG("Paddle::onTouchBegan id = %d, x = %f, y = %f", touch->getID(), touch->getLocation().x, touch->getLocation().y);
    detectSprite(touch);
}
*/

 bool match::onTouchBegan(Touch* touch, Event* event)
 {
   CCLOG("Paddle::onTouchBegan id = %d, x = %f, y = %f", touch->getID(), touch->getLocation().x, touch->getLocation().y);
   return true;
 }
 
 void match::onTouchMoved(Touch* touch, Event* event)
 {
   CCLOG("Paddle::onTouchMoved id = %d, x = %f, y = %f", touch->getID(), touch->getLocation().x, touch->getLocation().y);
 }
 
 void match::onTouchEnded(Touch* touch, Event* event)
 {
     detectSprite(touch);
 }
