//
// File: LinkGame.h
//

#ifndef LINKGAME_H
#define LINKGAME_H

#include <vector>

struct Point
{
    Point() { x = y = f = 0; }
    Point(int _x, int _y, int _f): x(_x), y(_y), f(_f) { }
    Point(const Point& other) { x = other.x, y = other.y, f = other.f; }
    int x, y;  // x为行号，y为列号
    int f;    // f为0表示该点为空，否则表示该点为第f种图片
};

class LinkGame
{
public:
    virtual ~LinkGame();
    LinkGame(int _row, int _col, int _kindPic, int _numPic);

    // 判断两点是否相连
    bool judgeLink(const Point& a, const Point& b);
    // 消除两点
    void killTwoPoints(const Point& a, const Point& b);
    // 产生地图
    void makeMap();
    // 判断是否全部消除
    bool judgeFinished();
    // 打印地图
    void printMap();
    // 随机重排地图
    void randomMap();
    // 提示用户
    bool giveTip(int& r1, int& c1, int& r2, int& c2);

    int getRowNum() { return this->row; }
    int getColNum() { return this->col; }
    int getPicNum() { return this->numPic; }

public:
    std::vector< std::vector<Point> > vvpMap;  // 地图，行和列从1开始，这样方便处理边界上的点
    std::vector<Point> vpCornerPoints;     // 两个点连线中的拐角点（包含这两个点）

private:
    // 只要将相同的两个图片用三根以内（含三根）的直线连在一起就可以消除
    bool linkByOneLine(const Point& a, const Point& b);
    bool linkByTwoLines(const Point& a, const Point& b);
    bool linkByThreeLines(const Point& a, const Point& b);

    // 递归搜索所有可能下法，判断是否有解
    void dfsJudgeValid(int cnt);
    // 若有解则返回true，否则返回false
    bool judgeValid();

private:
    int row;  // 行数
    int col;  // 列数
    int kindPic;   // 图片的种数
    int numPic;    // 图片的数量
    int initialNumPic;  // 等于构造时的numPic,用于每次重新开始
    bool isValid;  // 产生的地图是否有效
    std::vector< std::vector<Point> > vvpSamePic; //把相同种类的图片链接起来,vvpSamePic[i].size()表示为第i种图片的数量
};

#endif // LINKGAME_H
