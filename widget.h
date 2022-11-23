#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QCameraInfo>
#include <QDateTime>
#include <QMessageBox>
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void openVideo();
    cv::Mat getHsvImg();
    void saveImg();
    int  clamp(int v, int minv, int maxv);
    QList<int> getCameraNum();
    float getHsvIou(cv::Mat hsv);
    void debugHsvImg();

private slots:
    void on_startBt_clicked();
    void on_autoPhotographBt_clicked();
    void on_changepathBt_clicked();
    void on_photographBt_clicked();
    void on_hmin_valueChanged(int value);
    void on_hmax_valueChanged(int value);
    void on_smin_valueChanged(int value);
    void on_smax_valueChanged(int value);
    void on_vmin_valueChanged(int value);
    void on_vmax_valueChanged(int value);
    void on_debugBt_clicked(bool checked);

private:
    Ui::Widget *ui;
    //摄像头对象指针
    QCamera* Camera;
    //摄像头的取景器
    QCameraViewfinder* CameraViewFinder;
    //存储摄像机设备信息
    QList<QCameraInfo> CameraInfoList;
    //记录摄像头内容
    QCameraImageCapture* CameraImageCapture;
    //照片的名字
    QString ImageName;

    cv::VideoCapture cap;
    cv::Mat img;
    cv::Mat simg;
    cv::Mat hsvimg;
    QTimer *timer;
    QTimer *timerp;
    float ious;
    int recnum;
    bool readcap;
};
#endif // WIDGET_H
