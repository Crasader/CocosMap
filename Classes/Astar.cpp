#include <math.h>  
#include "Astar.h"  


void Astar::InitAstar(std::vector<bool> &_maze, int mapwidth, int mapheight)
{
	maze = _maze;
	width = mapwidth;
	height = mapheight;
}

int Astar::calcG(APoint *temp_start, APoint *point)
{
	/* 原正方形  */
	//int extraG = (abs(point->x - temp_start->x) + abs(point->y - temp_start->y)) == 1 ? kCost1 : kCost2;
	//int parentG = point->parent == NULL ? 0 : point->parent->G; //如果是初始节点，则其父节点是空  
	//return parentG + extraG;

	//45度菱形

	int g = 0;
	if (temp_start->y == point->y) // 横向  左右
	{
		g = temp_start->G + COST_HORIZONTAL;
	}
	else if (temp_start->y + 2 == point->y || temp_start->y - 2 == point->y) // 竖向  上下
	{
		g = temp_start->G + COST_VERTICAL * 2;
	}
	else // 斜向  左上 左下 右上 右下
	{
		g = temp_start->G + COST_DIAGONAL;
	}
	return g;
}

int Astar::calcH(APoint *point, APoint *end)
{
	//原正方形 用简单的欧几里得距离计算H
//	return sqrt((double)(end->x - point->x)*(double)(end->x - point->x) + (double)(end->y - point->y)*(double)(end->y - point->y))*kCost1;
	
    //45度菱形  曼哈顿算法
	int to0 = point->x * COST_HORIZONTAL + ((int)point->y & 1) * COST_HORIZONTAL / 2;
	int endTo0 = end->x * COST_HORIZONTAL + ((int)end->y & 1) * COST_HORIZONTAL / 2;
	return abs((float)endTo0 - (float)to0) + abs((float)end->y - (float)point->y) * COST_VERTICAL;
}

int Astar::calcF(APoint *point)
{
	return point->G + point->H;
}

APoint *Astar::getLeastFpoint()
{

	if (!openList.empty())
	{
		auto resPoint = openList.front();
		for (auto point : openList)
		if (point->F<resPoint->F)
			resPoint = point;
		return resPoint;
	}
	return NULL;
}

// 返回的是最后一个点
APoint *Astar::findPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner)
{
	openList.clear();
	closeList.clear();
	CCLOG("start %d,%d", startPoint.x,startPoint.y);
	CCLOG("end %d,%d", endPoint.x,endPoint.y);
	openList.pushBack(new APoint(startPoint.x, startPoint.y)); //置入起点,拷贝开辟一个节点，内外隔离  
	while (!openList.empty())
	{

        // X是下一个点，X从周围的八个点中选取
        // F = G + H
        // G 是起点到X点的距离
        // H 是终点到X点的距离
		auto curPoint = getLeastFpoint(); //找到F值最小的点  
		openList.eraseObject(curPoint); //从开启列表中删除  
		closeList.pushBack(curPoint); //放到关闭列表  
		//1,找到当前周围八个格中可以通过的格子  
		auto surroundPoints = getSurroundPoints(curPoint, isIgnoreCorner);
		for (auto &target : surroundPoints)
		{
			//2,对某一个格子，如果它不在开启列表中，加入到开启列表，设置当前格为其父节点，计算F G H  
			if (!isInList(openList, target))
			{
				target->parent = curPoint;

				target->G = calcG(curPoint, target);
				target->H = calcH(target, &endPoint);
				target->F = calcF(target);

				openList.pushBack(target);
			}
			//3，对某一个格子，它在开启列表中，计算G值, 如果比原来的大, 就什么都不做, 否则设置它的父节点为当前点,并更新G和F  
			else
			{
				int tempG = calcG(curPoint, target);
				if (tempG<target->G)
				{
					target->parent = curPoint;

					target->G = tempG;
					target->F = calcF(target);
					//CCLOG("%d,%d", target.x, target.y);
				}
			}
			APoint *resPoint = isInList(openList, &endPoint);
			if (resPoint)
				return resPoint; //返回列表里的节点指针，不要用原来传入的endpoint指针，因为发生了深拷贝  
		}
	}

	return NULL;
}

Vector<APoint *> Astar::GetPath(APoint &startPoint, APoint &endPoint, bool isIgnoreCorner)
{
	log("from point===============>%d,%d", startPoint.x, startPoint.y);
	log("to point===============>%d,%d", endPoint.x, endPoint.y);
	APoint *result = findPath(startPoint, endPoint, isIgnoreCorner);
	Vector<APoint *> path;
	//返回路径，如果没找到路径，返回空链表  
	int s = 1;
	while (result->parent)
	{		
	 //	result->y = height - result->y - 1;
		path.insert(0,result);
		result = result->parent;
		s++;
	}
	log("s===>%d",s);
	return path;
}

APoint *Astar::isInList(const Vector<APoint *> &list, const APoint *point) const
{
	//判断某个节点是否在列表中，这里不能比较指针，因为每次加入列表是新开辟的节点，只能比较坐标  
	for (auto p : list)
	if (p->x == point->x&&p->y == point->y)
		return p;
	return NULL;
}

bool Astar::isCanreach(const APoint *point, const APoint *target, bool isIgnoreCorner) const
{
    // 如果点与当前节点重合、超出地图、是障碍物、或者在关闭列表中，返回false
    if (target->x<0 || target->x>width-1  // x方向是否超出地图
        || target->y<0 || target->y>height-1    // y方向是否超出地图
        || maze[width*target->y + target->x] == false   // 目标点是否是障碍点
        || (target->x == point->x&&target->y == point->y)   // 是否重合
        || isInList(closeList, target))     // 是否在关闭列表中
    {
		return false;
    }
	else
	{
        // 如果是正方向，则直接返回
		if (abs(point->x - target->x) + abs(point->y - target->y) == 1) //非斜角可以  
			return true;
		else
		{
			//斜对角要判断是否绊住  
			if (maze[width*target->y + point->x] == true && maze[width*point->y + target->x] == true)
				return true;
			else
				return isIgnoreCorner;
		}
	}
}

Vector<APoint *> Astar::getSurroundPoints(const APoint *point, bool isIgnoreCorner) const
{
	Vector<APoint *> surroundPoints;

	//原正方形
	/*for (int x = point->x - 1; x <= point->x + 1; x++)
	for (int y = point->y - 1; y <= point->y + 1; y++)
	if (isCanreach(point, new APoint(x, y), isIgnoreCorner))
		surroundPoints.pushBack(new APoint(x, y));*/

	//45度菱形  菱形组合的地图八方向与正常不同

	// 左
	auto p = new APoint(point->x - 1, point->y);
	if (isCanreach(point, p, isIgnoreCorner)) // 可走并且不在关闭列表
	{
		surroundPoints.pushBack(p);
	}

	// 右
    p = new APoint(point->x + 1, point->y);
	if (isCanreach(point, p, isIgnoreCorner))
	{
		surroundPoints.pushBack(p);
	}

	// 上
	p = new APoint(point->x, point->y-1);
	if (isCanreach(point, p, isIgnoreCorner))
	{
		surroundPoints.pushBack(p);
	}
	// 下
	p = new APoint(point->x , point->y+1);
	if (isCanreach(point, p, isIgnoreCorner))
	{
		surroundPoints.pushBack(p);
	}

//	// 左上
//	p = new APoint(point->x - 1+ ((int)point->y & 1), point->y-1);
//	if (isCanreach(point, p, isIgnoreCorner))
//	{
//		surroundPoints.pushBack(p);
//	}
//
//	// 左下
//	p = new APoint(point->x - 1 + ((int)point->y & 1), point->y + 1);
//	if (isCanreach(point, p, isIgnoreCorner))
//	{
//		surroundPoints.pushBack(p);
//	}
//
//	//右上
//	p = new APoint(point->x  + ((int)point->y & 1), point->y - 1);
//	if (isCanreach(point, p, isIgnoreCorner))
//	{
//		surroundPoints.pushBack(p);
//	}
//
//	//右下
//	p = new APoint(point->x + ((int)point->y & 1), point->y +1);
//	if (isCanreach(point, p, isIgnoreCorner))
//	{
//		surroundPoints.pushBack(p);
//	}

	return surroundPoints;
}
