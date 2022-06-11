#ifndef MYPUSHBUTTON_H
#define MYPUSHBUTTON_H

#include <QWidget>
#include <QPushButton>

class MyPushbutton : public QPushButton
{
    Q_OBJECT
public:
    explicit MyPushbutton(QWidget *parent = nullptr);
    QString normal;
    QString action;
    QPixmap pix_normal;
    QPixmap pix_action;
    void set_normal();
    void set_action();

};

#endif // MYPUSHBUTTON_H
