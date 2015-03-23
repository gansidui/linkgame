//
// File: LinkGame.cpp
//

#include "LinkGame.h"
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <algorithm>

LinkGame::~LinkGame()
{
    for ( auto &x : this->vvpMap )
    {
        x.clear();
    }
    this->vvpMap.clear();

    for ( auto &x : this->vvpSamePic )
    {
        x.clear();
    }
    this->vvpSamePic.clear();

    this->vpCornerPoints.clear();
}

LinkGame::LinkGame(int _row, int _col, int _kindPic, int _numPic)
{
    this->row = _row;
    this->col = _col;
    this->kindPic = _kindPic;
    this->numPic = _numPic;
    this->initialNumPic = _numPic;
    this->isValid = true;
}

bool LinkGame::linkByOneLine(const Point& a, const Point& b)
{
    if ( a.x != b.x && a.y != b.y )
        return false;
    if ( a.x == b.x )
    {
        for ( int i = std::min(a.y, b.y)+1; i < std::max(a.y, b.y); ++ i )
        {
            if ( this->vvpMap[a.x][i].f )
                return false;
        }
    }
    else
    {
        for ( int i = std::min(a.x, b.x)+1; i < std::max(a.x, b.x); ++ i )
        {
            if ( this->vvpMap[i][a.y].f )
                return false;
        }
    }
    return true;
}

bool LinkGame::linkByTwoLines(const Point& a, const Point& b)
{
    if ( !this->vvpMap[a.x][b.y].f && linkByOneLine(a, this->vvpMap[a.x][b.y]) &&
        linkByOneLine(this->vvpMap[a.x][b.y], b) )
    {
        this->vpCornerPoints.push_back(this->vvpMap[a.x][b.y]);
        return true;
    }
    else if ( !this->vvpMap[b.x][a.y].f && linkByOneLine(a, this->vvpMap[b.x][a.y]) &&
        linkByOneLine(this->vvpMap[b.x][a.y], b) )
    {
        this->vpCornerPoints.push_back(this->vvpMap[b.x][a.y]);
        return true;
    }
    return false;
}

bool LinkGame::linkByThreeLines(const Point& a, const Point& b)
{
    // 由 a 开始向4个方向遍历 c ，判断 c 和 b 是否 linkByTwoLines
    for ( int i = 0; i <= this->row+1; ++ i )  // vertical
    {
        if ( i == a.x )
            continue;
        if ( !this->vvpMap[i][a.y].f && linkByOneLine(a, this->vvpMap[i][a.y]) &&
            linkByTwoLines(this->vvpMap[i][a.y], b) )
        {
            Point temp = this->vpCornerPoints[1];
            this->vpCornerPoints[1] = this->vvpMap[i][a.y];
            this->vpCornerPoints.push_back(temp);
            return true;
        }
    }
    for ( int i = 0; i <= this->col+1; ++ i )  // horizonal
    {
        if ( i == a.y )
            continue;
        if ( !this->vvpMap[a.x][i].f && linkByOneLine(a, this->vvpMap[a.x][i]) &&
            linkByTwoLines(this->vvpMap[a.x][i], b) )
        {
            Point temp = this->vpCornerPoints[1];
            this->vpCornerPoints[1] = this->vvpMap[a.x][i];
            this->vpCornerPoints.push_back(temp);
            return true;
        }
    }
    return false;
}

bool LinkGame::judgeLink(const Point& a, const Point& b)
{
    if ( !a.f || !b.f )     // 有空格为不相连
        return false;
    if ( a.x == b.x && a.y == b.y )  // 两点相同为不相连
        return false;
    if ( a.f != b.f )     // 图片种类不同为不相连
        return false;

    this->vpCornerPoints.clear();
    this->vpCornerPoints.push_back(a);

    bool flag = false;
    if ( linkByOneLine(a, b) || linkByTwoLines(a, b) || linkByThreeLines(a, b) )
        flag = true;

    this->vpCornerPoints.push_back(b);

    return flag;
}

void LinkGame::killTwoPoints(const Point& a, const Point& b)
{
    this->vvpMap[a.x][a.y].f = 0;
    this->vvpMap[b.x][b.y].f = 0;
    this->numPic -= 2;
}

void LinkGame::makeMap()
{
    this->numPic = this->initialNumPic;  // 重置numPic
    for ( auto &x : this->vvpMap )
    {
        x.clear();
    }
    this->vvpMap.clear();

    for ( int i = 0; i <= this->row+1; ++ i )
    {
        std::vector<Point> vp;
        for ( int j = 0; j <= this->col+1; ++ j )
        {
            vp.push_back(Point(i, j, 0));
        }
        this->vvpMap.push_back(vp);
    }

    // 若地图无解，那么一直产生数据
    do
    {
        for ( auto &x : this->vvpSamePic )
        {
            x.clear();
        }
        this->vvpSamePic.clear();
        std::srand((unsigned)std::time(0));
        int t = static_cast<int>(this->numPic / this->kindPic);
        ( t & 1 ) == 0 ? t : t -= 1;    // 每种图片有t张，保证t为偶数
        this->numPic = this->kindPic * t;   // 图片总数必须更新

        // 将 this->kindPic 种图片随机分布
        for ( int i = 1; i <= this->kindPic; ++ i )
        {
            std::vector<Point> vp;
            for ( int j = 0; j < t; ++ j )
            {
                bool flag = true;
                while ( flag )
                {
                    // 图片的下标分布在 [1,row][1,col]
                    int x = std::rand() % this->row + 1;
                    int y = std::rand() % this->col + 1;
                    if ( !this->vvpMap[x][y].f )
                    {
                        this->vvpMap[x][y] = Point(x, y, i);
                        vp.push_back(this->vvpMap[x][y]);
                        flag = false;
                    }

                }
            }
            this->vvpSamePic.push_back(vp);
        }

    } while ( !judgeValid() );
}

void LinkGame::dfsJudgeValid(int cnt)
{
    if ( cnt == this->numPic )
        this->isValid = true;
    if ( this->isValid )
        return;

    for ( unsigned int i = 0; i < this->vvpSamePic.size(); ++ i )
    {
        for ( unsigned int j = 0; j < this->vvpSamePic[i].size(); ++ j )
        {
            for ( unsigned int k = j+1; k < this->vvpSamePic[i].size(); ++ k )
            {
                int f = this->vvpMap[this->vvpSamePic[i][j].x][this->vvpSamePic[i][j].y].f;
                if ( judgeLink(this->vvpSamePic[i][j], this->vvpSamePic[i][k]) )
                {
                    this->vvpMap[this->vvpSamePic[i][j].x][this->vvpSamePic[i][j].y].f = 0;
                    this->vvpMap[this->vvpSamePic[i][k].x][this->vvpSamePic[i][k].y].f = 0;
                    dfsJudgeValid(cnt+2);
                    this->vvpMap[this->vvpSamePic[i][j].x][this->vvpSamePic[i][j].y].f = f;
                    this->vvpMap[this->vvpSamePic[i][k].x][this->vvpSamePic[i][k].y].f = f;
                }
            }
        }
    }
}

bool LinkGame::judgeValid()
{
    this->isValid = false;
    std::vector< std::vector<Point> > vvp(this->vvpMap);

    dfsJudgeValid(0);

    this->vvpMap.assign(vvp.begin(), vvp.end());
    return this->isValid;
}

bool LinkGame::judgeFinished()
{
    return !this->numPic;
}

void LinkGame::printMap()
{
    for ( int i = 0; i <= this->row+1; ++ i )
    {
        if ( !i )
        {
            std::cout << "      ";
            for ( int k = 0; k <= this->col+1; ++ k )
                std::cout << k << " ";
            std::cout << std::endl;
            for ( int k = 0; k <= this->col+1; ++ k )
                std::cout << "---";
            std::cout << std::endl;
        }
        for ( int j = 0; j <= this->col+1; ++ j )
        {
            if ( !j )
                std::cout << i << " |   ";
            std::cout << this->vvpMap[i][j].f << " ";
        }
        std::cout << std::endl;
    }
}

void LinkGame::randomMap()
{
    std::vector<Point> vpTemp;
    for ( int i = 1; i <= this->row; ++ i )
    {
        for ( int j = 1; j <= this->col; ++ j )
        {
            vpTemp.push_back(this->vvpMap[i][j]);
        }
    }
    std::random_shuffle(vpTemp.begin(), vpTemp.end()); // 随机重排

    int cnt = 0;
    for ( int i = 1; i <= this->row; ++ i )
    {
        for ( int j = 1; j <= this->col; ++ j )
        {
            this->vvpMap[i][j].f = vpTemp[cnt++].f;  // 只需重置f
        }
    }
}

bool LinkGame::giveTip(int &r1, int &c1, int &r2, int &c2)
{
    for ( int i = 1; i <= this->row; ++ i )
    {
        for ( int j = 1; j <= this->col; ++ j )
        {
            for ( int k = 1; k <= this->row; ++ k )
            {
                for ( int m = 1; m <= this->col; ++ m )
                {
                    if ( judgeLink(this->vvpMap[i][j], this->vvpMap[k][m]) )
                    {
                        r1 = i;
                        c1 = j;
                        r2 = k;
                        c2 = m;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
