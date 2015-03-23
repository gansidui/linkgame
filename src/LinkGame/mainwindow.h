#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <vector>
#include <memory>
#include "LinkGame.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startGame();    // 开始游戏
    void deleteImageButtons();  // 删除显示图片的按钮
    void createImageButtons();   // 生成显示图片的按钮
    void setButtonSelected(QPushButton* pushButton);   // 设置这个按钮被选中的样式
    void setButtonInitialed(QPushButton* pushButton);   // 设置这个按钮为最初的样式
    void paintLinkPath();   // 画出相连的路径
    void clearLinkPath();   // 消除相连的路径

private:
    std::unique_ptr<LinkGame> linkGame; // 游戏对象
    std::shared_ptr<QPushButton> imageButtons[50][50]; // 显示图片的按钮
    QPushButton *btnPrev;   // 保持上次被点击的图片
    QPushButton *btnTipBegin, *btnTipEnd;  // 提示用户可以选择连线这两个按钮
    Point posIndex[50][50];  // 画线时需要位置信息
    std::vector< std::shared_ptr<QPushButton> > vecBtnLine;  // 保存表示线的按钮

    int startX, startY;   // 第一张图片的起始位置
    int imageSize;        // 图片大小
    int row;      // 行数
    int col;      // 列数
    int kindPic;  // 图片种类
    int numPic;  // 图片总数
    bool isPlaying;   // 正在游戏中
    const int NumAddTime = 3;   // 每次消掉一对后增加的时间
    const int MaxNumRandomMap = 3;  // 重排的数量限制
    const int MaxNumGiveTip = 3;   // 提示的数量限制
    int numRandomMap;    // 重排次数
    int numGiveTip;     // 提示次数
    bool isClearLine;   // 是否可以消除画线

private slots:
    void on_startGame_clicked();  // 开始

    void on_image_clicked();    // 自定义的图片点击响应函数

    void on_randomMap_clicked();  // 重排

    void on_giveTip_clicked();   // 给出提示

    void timerEvent(QTimerEvent *);   // 默认的定时器响应函数

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
