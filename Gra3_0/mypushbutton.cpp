#include "mypushbutton.h"

MyPushbutton::MyPushbutton(QWidget *parent) : QPushButton(parent)
{
    QString normal = ":/icon/image/white.PNG";
    QString action = ":/icon/image/red.PNG";
    //加载图片
    pix_normal.load(normal);
    pix_action.load(action);

    this->setFixedSize(pix_normal.width(),pix_normal.height());
    //设置不规则图片样式
    this->setStyleSheet("QPushButton{border:0px;}");//边框0像素

    this->setIcon(pix_normal);
    this->setIconSize(QSize(pix_normal.width(),pix_normal.height()));
}
void MyPushbutton::set_normal()
{
    this->setIcon(pix_normal);
    this->setIconSize(QSize(pix_normal.width(),pix_normal.height()));

}
void MyPushbutton::set_action()
{
    this->setIcon(pix_action);
    this->setIconSize(QSize(pix_action.width(),pix_action.height()));
}
