#ifndef __BankMap_SCENE_H__
#define __BankMap_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Astar.h"

USING_NS_CC;

class BankMap : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(BankMap);

private:

    // 按钮点击事件
    void buttonEvent(Ref *pSender, ui::Widget::TouchEventType type);

    // 按钮点击事件
    void businessEvent(Ref *pSender, ui::Widget::TouchEventType type);

    // 按钮点击事件
    void vipEvent(Ref *pSender, ui::Widget::TouchEventType type);

    // 按钮点击事件
    void indicateVIP(Ref *pSender, ui::Widget::TouchEventType type);

    void flashIndicate(float dt);

    void flashRoute(float dt);

    void cleanRoute(float dt);

    // 地图
    TMXTiledMap* _tileMap;

    // 地板层
    TMXLayer* _groundLayer;

    // 导航层
    TMXLayer* _routeLayer;

    // 对像层
    TMXLayer* _objectLayer;

    // 区域标识图层
    TMXLayer* _indicateLayer;

    // 寻路引擎
    Astar astar;

    // 屏幕宽度
    int MapWidth;

    // 屏幕高度
    int MapHeight;

    // 网格数据
    std::vector<bool> _grid;

    //初始化地图块参数
    void initGrid();
};

#endif // __BankMap_SCENE_H__
