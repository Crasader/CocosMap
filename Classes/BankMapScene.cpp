#include "BankMapScene.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

Scene* BankMap::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = BankMap::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool BankMap::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    _tileMap = TMXTiledMap::create("map/smartbank.tmx");

    CCLOG("layer number %d", _tileMap->getLayerNum());

    _groundLayer = _tileMap->getLayer("ground");
    _routeLayer = _tileMap->getLayer("router");
    _objectLayer = _tileMap->getLayer("object");
    _tileMap->getLayer("vip")->setVisible(false);
//    _tileMap->getLayer("duoyu")->setVisible(false);
//CCLOG("indicate gid %d", _indicateLayer->getTileGIDAt(Vec2(0, 0)));
    _tileMap->setPosition(Vec2(origin.x + visibleSize.width / 2 - _tileMap->getContentSize().width / 2, origin.y + visibleSize.height / 2 - _tileMap->getContentSize().height / 2));
    MapWidth = _tileMap->getMapSize().width;
    MapHeight = _tileMap->getMapSize().height;
    this->initGrid();
    this->addChild(_tileMap);


    // 初始化寻路引擎
    astar.InitAstar(_grid, MapWidth, MapHeight);

    //将按钮放到层中
    ui::Button* personalButton = ui::Button::create("foot/personal.png");
    personalButton->addTouchEventListener(CC_CALLBACK_2(BankMap::buttonEvent, this));
    personalButton->setPosition(Vec2(0+ personalButton->getContentSize().width / 2 , 0 + personalButton->getContentSize().height / 2));
    this->addChild(personalButton);

    //将按钮放到层中
    ui::Button* bussnessButton = ui::Button::create("foot/buniness.png");
    bussnessButton->addTouchEventListener(CC_CALLBACK_2(BankMap::businessEvent, this));
    bussnessButton->setPosition(Vec2(0+ bussnessButton->getContentSize().width / 2 + personalButton->getContentSize().width, 0 + bussnessButton->getContentSize().height / 2));
    this->addChild(bussnessButton);

    //将按钮放到层中
    ui::Button* vipButton = ui::Button::create("foot/vip.png");
    vipButton->addTouchEventListener(CC_CALLBACK_2(BankMap::vipEvent, this));
    vipButton->setPosition(Vec2(0+ personalButton->getContentSize().width / 2 + personalButton->getContentSize().width * 2, 0 + personalButton->getContentSize().height / 2));
    this->addChild(vipButton);

    //将按钮放到层中
    ui::Button* area1Button = ui::Button::create("foot/foot_button_bg.png");

    area1Button->setTitleText("VIP区");
    area1Button->setTitleFontSize(30);
    area1Button->setTitleColor(Color3B(255, 255, 255));
    area1Button->setScale(0.5);
    area1Button->addTouchEventListener(CC_CALLBACK_2(BankMap::indicateVIP, this));
    area1Button->setPosition(Vec2(visibleSize.width - personalButton->getContentSize().width/2, visibleSize.height / 2 + personalButton->getContentSize().height / 2));
    this->addChild(area1Button);
//
//    //将按钮放到层中
//    ui::Button* area2Button = ui::Button::create("foot/foot_button_bg.png");
//    areaButton->setScale(0.5);
//    areaButton->addTouchEventListener(CC_CALLBACK_2(BankMap::indicateVIP, this));
//    areaButton->setPosition(Vec2(visibleSize.width - personalButton->getContentSize().width/2, visibleSize.height / 2 + personalButton->getContentSize().height / 2));
//    this->addChild(areaButton);
//
//    //将按钮放到层中
//    ui::Button* area3Button = ui::Button::create("foot/foot_button_bg.png");
//    areaButton->setScale(0.5);
//    areaButton->addTouchEventListener(CC_CALLBACK_2(BankMap::indicateVIP, this));
//    areaButton->setPosition(Vec2(visibleSize.width - personalButton->getContentSize().width/2, visibleSize.height / 2 + personalButton->getContentSize().height / 2));
//    this->addChild(areaButton);

    return true;
}

void BankMap::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
    
    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/
    
    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
    
    
}

void BankMap::initGrid()
{
    _grid.clear();

    int cellCount = _tileMap->getMapSize().width*_tileMap->getMapSize().height;

    for (int i = 0; i < cellCount; i++)
    {
        _grid.push_back(true);
        Vec2 tileCoord = Vec2(i%MapWidth, i / MapWidth);
        // 使用TMXLayer类的tileGIDAt函数获取TileMap坐标系里的“全局唯一标识”GID
        int tileGid = _objectLayer->getTileGIDAt(tileCoord);
        if (tileGid)
        {
            // 使用GID来查找指定tile的属性，返回一个Value
            Value properties = _tileMap->getPropertiesForGID(tileGid);
            // 返回的Value实际是一个ValueMap
            ValueMap map = properties.asValueMap();
            // 查找ValueMap，判断是否有”可碰撞的“物体，如果有，设置网格对象的isPass变量为false
            std::string value = map.at("collidable").asString();
            if (value.compare("true") == 0)
            {
                _grid.at(i) = false;
            }
        }
    }
}

void BankMap::buttonEvent(Ref *pSender, ui::Widget::TouchEventType type)
{
    APoint start(8, 4);
    APoint end(3, 6);
    Vector<APoint *> path = this->astar.GetPath(start, end, true);

    path.insert(0, &start);
    for (auto &p : path)
    {
        log("point===============>%d,%d",p->x,p->y);
        _routeLayer->setTileGID(42, Vec2(p->x, p->y));
    }

    this->schedule(schedule_selector(BankMap::flashRoute), 0.5, 3, 0);
    this->scheduleOnce(schedule_selector(BankMap::cleanRoute), 5);
}


void BankMap::businessEvent(Ref *pSender, ui::Widget::TouchEventType type)
{
    APoint start(8, 4);
    APoint end(3, 2);

    Vector<APoint *> path = this->astar.GetPath(start, end, true);

    path.insert(0, &start);
    for (auto &p : path)
    {
        log("point===============>%d,%d",p->x,p->y);
        _routeLayer->setTileGID(42, Vec2(p->x, p->y));
    }

    this->schedule(schedule_selector(BankMap::flashRoute), 0.5, 3, 0);
    this->scheduleOnce(schedule_selector(BankMap::cleanRoute), 5);
}

void BankMap::vipEvent(Ref *pSender, ui::Widget::TouchEventType type)
{
    APoint start(8, 4);
    APoint end(6, 7);

    Vector<APoint *> path = this->astar.GetPath(start, end, true);

    path.insert(0, &start);
    for (auto &p : path)
    {
        log("point===============>%d,%d",p->x,p->y);
        _routeLayer->setTileGID(42, Vec2(p->x, p->y));
    }

    this->schedule(schedule_selector(BankMap::flashRoute), 0.5, 3, 0);
    this->scheduleOnce(schedule_selector(BankMap::cleanRoute), 5);
}

void BankMap::indicateVIP(Ref *pSender, ui::Widget::TouchEventType type)
{
    // 间隔0.5秒，重复1+5次，延迟0秒
    this->schedule(schedule_selector(BankMap::flashIndicate), 0.5, 5, 0);
}

void BankMap::flashIndicate(float dt)
{
    _tileMap->getLayer("vip")->setVisible(!_tileMap->getLayer("vip")->isVisible());

}

void BankMap::flashRoute(float dt)
{
    _tileMap->getLayer("router")->setVisible(!_tileMap->getLayer("router")->isVisible());

}

void BankMap::cleanRoute(float dt)
{
    for (int i = 0; i < MapWidth; i ++)
    {
        for (int j = 0; j < MapHeight; j++)
        {
            _routeLayer->setTileGID(0, Vec2(i, j));
        }
    }
}
