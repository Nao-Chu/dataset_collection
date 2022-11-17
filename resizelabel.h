#ifndef RESIZELABEL_H
#define RESIZELABEL_H

#include <QLabel>

class ResizeLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ResizeLabel(QWidget *parent = nullptr);
    ~ResizeLabel();


private:
    // 鼠标点击事件，按下，移动，释放
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *event);

private:
    // 获取上一个点击坐标
    QPoint mouse_point_old_;
    int status;
};

#endif // ResizeLabel_H
