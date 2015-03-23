#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->row = 10;
    this->col = 12;
    this->kindPic = 20;
    this->numPic = 120;
    this->imageSize = 50;
    this->startX = 0;
    this->startY = 25;
    this->btnPrev = nullptr;
    this->btnTipBegin = nullptr;
    this->btnTipEnd = nullptr;
    this->linkGame.reset(new LinkGame(row, col, kindPic, numPic));
    this->isPlaying = false;
    this->isClearLine = false;
    ui->timeBar->setMaximum(100); // 设置进度条最大值
    ui->timeBar->setValue(ui->timeBar->maximum());
    startTimer(1000); // 启动定时器
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startGame_clicked()
{
    vecBtnLine.clear();
    deleteImageButtons();
    isPlaying = true;
    btnPrev = nullptr;
    btnTipBegin = nullptr;
    btnTipEnd = nullptr;
    numGiveTip = 0;
    numRandomMap = 0;
    linkGame->makeMap();  // 产生一个一定有解的地图
    createImageButtons();
    ui->timeBar->setValue(ui->timeBar->maximum());
    ui->randomMap->setText(QString("重排: ") + QString::number(MaxNumRandomMap));
    ui->giveTip->setText(QString("提示: ") + QString::number(MaxNumGiveTip));
}

void MainWindow::createImageButtons()
{
    for ( int i = 0; i <= row+1; ++ i )
    {
        for ( int j = 0; j <= col+1; ++ j )
        {
            // 保存位置信息,画线时需要
            posIndex[i][j].x = startX + imageSize * j;
            posIndex[i][j].y = startY + imageSize * i;

            // 最外一圈不需要按钮
            if ( 0 == i || 0 == j || row+1 == i || col+1 == j )
            {
                continue;
            }
            int f = linkGame->vvpMap[i][j].f; // 第 f 种图片
            if ( f )
            {
                imageButtons[i][j].reset(new QPushButton(this));
                imageButtons[i][j]->setGeometry(
                            startX + imageSize * j,
                            startY + imageSize * i,
                            imageSize, imageSize );
                QString path("./images/");
                path = path + QString::number(f) + QString(".bmp");
                // qDebug() << QDir::currentPath();
                QPixmap pixmap(path);
                QIcon icon(pixmap);
                imageButtons[i][j]->setIcon(icon);
                imageButtons[i][j]->setIconSize(QSize(imageSize, imageSize));
                imageButtons[i][j]->show();  // 显示按钮

                QObject::connect(imageButtons[i][j].get(), &QPushButton::clicked,
                                 this, &MainWindow::on_image_clicked);
            }
        }
    }
}

void MainWindow::deleteImageButtons()
{
    for ( int i = 1; i <= row; ++ i )
    {
        for ( int j = 1; j <= col; ++ j )
        {
            imageButtons[i][j].reset();
        }
    }
}

void MainWindow::setButtonSelected(QPushButton *pushButton)
{
    pushButton->setStyleSheet("QPushButton{padding: 5px; border: 5px solid red}");
    pushButton->setIconSize(QSize(imageSize-5, imageSize-5));
}

void MainWindow::setButtonInitialed(QPushButton *pushButton)
{
    pushButton->setStyleSheet("QPushButton{padding: 0px}");
    pushButton->setIconSize(QSize(imageSize, imageSize));
}

void MainWindow::paintLinkPath()
{
    // 利用画一排按钮去模拟画线,因为画线会被控件和窗口背景覆盖在下面
    // 模拟连线的按钮的边长为 imageSize * scale , 两个拐角点的中间也插入一些按钮
    const float scale = 3.0/10.0;
    // 在这两个拐角点间画一排按钮,这两个点必然在同一行或者同一列
    auto func = [this, scale](const Point& a, const Point& b)
    {
        if ( a.x == b.x )
        {
            for ( int j = std::min(a.y, b.y); j <= std::max(a.y, b.y); ++ j )
            {
                std::shared_ptr<QPushButton> button(new QPushButton(this));
                button->setGeometry(
                        posIndex[a.x][j].x + imageSize*(1-scale)/2,
                        posIndex[a.x][j].y + imageSize*(1-scale)/2,
                        imageSize*scale, imageSize*scale );
                button->setText(tr("☆"));
                button->setFlat(true);
                button->show();
                vecBtnLine.push_back(button);
            }
        }
        else
        {
            for ( int i = std::min(a.x, b.x); i <= std::max(a.x, b.x); ++ i )
            {
                std::shared_ptr<QPushButton> button(new QPushButton(this));
                button->setGeometry(
                        posIndex[i][a.y].x + imageSize*(1-scale)/2,
                        posIndex[i][a.y].y + imageSize*(1-scale)/2,
                        imageSize*scale, imageSize*scale );
                button->setText(tr("★"));
                button->setFlat(true);
                button->show();
                vecBtnLine.push_back(button);
            }
        }
    };

    for ( unsigned int i = 0; i < linkGame->vpCornerPoints.size()-1; ++ i )
    {
        func(linkGame->vpCornerPoints[i], linkGame->vpCornerPoints[i+1]);
    }

    isClearLine = true;  // 设置可以消除
}

void MainWindow::clearLinkPath()
{
    if ( isClearLine )
    {
        vecBtnLine.clear();
        isClearLine = false;
    }
}

void MainWindow::on_image_clicked()
{
    if (!isPlaying)
    {
        QMessageBox::information(this, tr("提示"), tr("请开始游戏!"));
        return;
    }

    clearLinkPath(); // 删掉上次画的的路径

    // 还原提示用户的按钮的样式
    if ( btnTipBegin && btnTipEnd )
    {
        setButtonInitialed(btnTipBegin);
        setButtonInitialed(btnTipEnd);
        btnTipBegin = nullptr;
        btnTipEnd = nullptr;
    }

    QPushButton *btnCur = (QPushButton*)sender();  // 得到信号发送者

    // 修改选中按钮的样式
    setButtonSelected(btnCur);

    if ( !btnPrev || btnPrev == btnCur )   // 上次没有点击图片或是同一张
    {
        btnPrev = btnCur;
        return;
    }

    // 还原上次选中的按钮的样式
    setButtonInitialed(btnPrev);

    // 计算对应的index
    int prevCol = (btnPrev->geometry().x() - startX) / imageSize;
    int prevRow = (btnPrev->geometry().y() - startY) / imageSize;
    int curCol = (btnCur->geometry().x() - startX) / imageSize;
    int curRow = (btnCur->geometry().y() - startY) / imageSize;

    if ( linkGame->judgeLink(linkGame->vvpMap[prevRow][prevCol],
                             linkGame->vvpMap[curRow][curCol]) )
    {
        paintLinkPath();  // 画出相连的路径

        int newTime = std::min(ui->timeBar->value()+NumAddTime, ui->timeBar->maximum());
        ui->timeBar->setValue(newTime); // 奖励时间

        linkGame->killTwoPoints(linkGame->vvpMap[prevRow][prevCol],
                                linkGame->vvpMap[curRow][curCol]);
        imageButtons[prevRow][prevCol].reset();
        imageButtons[curRow][curCol].reset();
        btnPrev = nullptr;
        if ( linkGame->judgeFinished() )
        {
            isPlaying = false;
            // 剩余时间作为分数
            QString res = QString("恭喜过关!\n\n得分:") +
                    QString::number(ui->timeBar->value());
            QMessageBox::information(this, tr("通知"), res);
        }
    }
    else
    {
        btnPrev = btnCur;
    }
}

void MainWindow::on_randomMap_clicked()
{
    if ( !isPlaying )
    {
        QMessageBox::information(this, tr("通知"), tr("还未开始,不能重排!"));
        return;
    }
    if ( numRandomMap >= MaxNumRandomMap )
    {
        return;
    }
    numRandomMap += 1;
    ui->randomMap->setText(QString("重排: ") + QString::number(MaxNumRandomMap-numRandomMap));
    ui->timeBar->setValue( ui->timeBar->value()-30 < 0 ? 0 : ui->timeBar->value()-30 ); // 重排一次扣30秒

    vecBtnLine.clear();
    deleteImageButtons();
    btnPrev = nullptr;
    btnTipBegin = nullptr;
    btnTipEnd = nullptr;
    linkGame->randomMap();
    createImageButtons();
}

void MainWindow::on_giveTip_clicked()
{
    if ( !isPlaying )
    {
        QMessageBox::information(this, tr("通知"), tr("还未开始,不能提示!"));
        return;
    }
    if ( numGiveTip >= MaxNumGiveTip )
    {
        return;
    }
    int r1, c1, r2, c2;
    if ( linkGame->giveTip(r1, c1, r2, c2) )
    {
        btnTipBegin = imageButtons[r1][c1].get();
        btnTipEnd = imageButtons[r2][c2].get();
        setButtonSelected(btnTipBegin);
        setButtonSelected(btnTipEnd);
        if ( btnPrev )
        {
            setButtonInitialed(btnPrev); // 还原上次选中的按钮的样式
            btnPrev = nullptr;
        }
        numGiveTip += 1;
        ui->giveTip->setText(QString("提示: ") + QString::number(MaxNumGiveTip-numGiveTip));
    }
    else
    {
        QMessageBox::information(this, tr("通知"), tr("无法给出提示,建议重排!") );
    }
}

void MainWindow::timerEvent(QTimerEvent *)
{
    if ( isClearLine )
    {
        clearLinkPath();
    }

    if ( isPlaying )
    {
        ui->timeBar->setValue( ui->timeBar->value() - 1 );
        if ( ui->timeBar->value() <= 0 )
        {
            isPlaying = false;
            QMessageBox::information(this, tr("通知"), tr("Game Over!"));
        }
    }
}
