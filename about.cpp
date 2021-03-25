#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
}

About::~About()
{
    delete ui;
}

void About::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton)//判断是否是左键点击
    {
        m_mousePressPoint = event->pos();
    }
}

void About::mouseMoveEvent(QMouseEvent *event)
{
    QPoint tempPoint = event->pos();//获取事件发生位置
    if(ui->close->geometry().contains(tempPoint)) {
        return;
    }
    QRect selfRect = ui->back->geometry();//获取窗口图标相对于父窗体的矩形
    int y = selfRect.y();//获取矩形的高度
    int tempHight = ui->back->height() + y;//获取标题栏最下线到父窗体最上线的距离（作为判断对比值）

    if (m_mousePressPoint.y() < tempHight)//对比，确定是否移动。
    {
        int tempX = tempPoint.x() - m_mousePressPoint.x();
        int tempY = tempPoint.y() - m_mousePressPoint.y();
        int nowX = this->x();
        int nowY = this->y();
        nowX += tempX;
        nowY += tempY;
        this->move(nowX, nowY);
    }
}

void About::mouseReleaseEvent(QMouseEvent *event)
{
    m_mousePressPoint = event->pos();
}
