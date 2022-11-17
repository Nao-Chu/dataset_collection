#include "resizelabel.h"
#include <QMouseEvent>
#include <QPainter>

ResizeLabel::ResizeLabel(QWidget *parent) :
    status(0)
{
    this->setParent(parent);
}

ResizeLabel::~ResizeLabel()
{

}

void ResizeLabel::mousePressEvent(QMouseEvent *ev)
{
    mouse_point_old_ = ev->pos();
    int x = width() - 20;
    int y = height() - 20;
    // 放大缩小
    if (mouse_point_old_.x() > x && mouse_point_old_.y() > y) {
        status = 1;
    } else {
        status = 2;
    }
}

void ResizeLabel::mouseMoveEvent(QMouseEvent *ev)
{
   if(status == 1) {
        QPoint sizepos = ev->pos() - mouse_point_old_;
        int w = width() + sizepos.x();
        int h = height() + sizepos.y();
        mouse_point_old_ = ev->pos();
        if (w < 30) {
            w = 30;
        }
        if (h < 30) {
            h = 30;
        }
        resize(w,h);
   } else if (status == 2) {
        QPoint movepos = pos() + ev->pos() - mouse_point_old_;
        move(movepos);
   }
}

void ResizeLabel::mouseReleaseEvent(QMouseEvent *)
{
    status = 0;
}

void ResizeLabel::paintEvent(QPaintEvent *event)
{
    //先调用父类的paintEvent为了显示'背景'!!!
    QLabel::paintEvent(event);

    QPainter painter(this);
    painter.setPen(QPen(Qt::cyan, 20));
    QSize size = this->size();
    int width = size.width();
    int height = size.height();

    painter.drawRect(width - 20, height - 20, 20, 20);
}
