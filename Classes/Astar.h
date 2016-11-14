#pragma once  
/*
//A*算法对象类
*/
#include <vector>  
#include <list>  
#include <unordered_map>
#include "cocos2d.h"
USING_NS_CC;
using namespace std;



//横向移动一格的路径评分
static const int COST_HORIZONTAL = 20;
//竖向移动一格的路径评分
static const int COST_VERTICAL = 5;
//斜向移动一格的路径评分
static const int COST_DIAGONAL = 12;

const int kCost1 = 10; //直移一格消耗  
const int kCost2 = 14; //斜移一格消耗  

struct APoint :public Ref
{
	int x, y; //点坐标，这里为了方便按照C++的数组来计算，x代表横排，y代表竖列  
	int F, G, H; //F=G+H  
	APoint *parent; //parent的坐标，这里没有用指针，从而简化代码  
	APoint(int _x, int _y) :x(_x), y(_y), F(0), G(0), H(0), parent(NULL)  //变量初始化  
	{
	}
};


class Astar
{
public:
	void InitAstar(std::vector<bool> &_maze,int mapwidth, int mapheight);
	Vector<APoint *> GetPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner);

private:
	APoint *findPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner);
	Vector<APoint *> getSurroundPoints(const APoint *point, bool isIgnoreCorner) const;
	bool isCanreach(const APoint *point, const APoint *target, bool isIgnoreCorner) const; //判断某点是否可以用于下一步判断  
	APoint *isInList(const Vector<APoint *> &list, const APoint *point) const; //判断开启/关闭列表中是否包含某点  
	APoint *getLeastFpoint(); //从开启列表中返回F值最小的节点  
	//计算FGH值  
	int calcG(APoint *temp_start, APoint *point);
	int calcH(APoint *point, APoint *end);
	int calcF(APoint *point);
private:
	std::vector<bool> maze;
	Vector<APoint *> openList;  //开启列表  
	Vector<APoint *> closeList; //关闭列表  

	

	int width;
	int height;
};