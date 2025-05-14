#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

//bro tuve que crear un .h solo para hacer un mouseevent💀
#include <QLabel>
#include <QWidget>
#include<QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget *parent=nullptr):QLabel(parent) {}

signals:

    void clicked();

protected:

    void mousePressEvent(QMouseEvent *event)override {

        emit clicked();

    }

};

#endif // CLICKABLELABEL_H
