#include "widget.h"
#include "ui_widget.h"

#include <QFileDialog>

#pragma execution_character_set("utf-8")
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    Camera(nullptr),
    ious(0.0),
    recnum(0),
    readcap(false)
{
    ui->setupUi(this);

    QList<int> camera_num = getCameraNum();
    foreach (auto c, camera_num) {
        ui->deviceComboBox->addItem(QString::number(c));
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::openVideo);
    timerp = new QTimer(this);
    connect(timerp, &QTimer::timeout, this, &Widget::on_photographBt_clicked);

    if (ui->deviceComboBox->count() > 0) {
        // 默认打开第一个
        on_startBt_clicked();
    }

    on_debugBt_clicked(false);
}

Widget::~Widget()
{
    delete ui;
}

QList<int> Widget::getCameraNum()
{
    QList<int> num;
    CvCapture * m_Captrue;
    //10是随意写的一个数字，根据实际项目，最多可达到多少摄像头数目
    for (int i = 0; i < 10; i++)
    {
        m_Captrue = cvCreateCameraCapture(i);
        if (m_Captrue == NULL)
            continue;
        cvReleaseCapture(&m_Captrue);//一定要释放 否则程序进程不能完全退出
        num.push_back(i);
    }
    return num;
}

void Widget::openVideo()
{
    if (!cap.isOpened()) //判断相机是否打开
    {
        std::cerr << "ERROR!!Unable to open camera\n";
        return;
    }

    cap >> simg; //以流形式捕获图像
    if (simg.empty()) {
        return;
    }
    cv::resize(simg, img, cv::Size(640, 480));

    cvtColor(img, img, cv::COLOR_BGR2RGB);
    if (ui->autoPhotographBt->isChecked()) {
        saveImg();
    }

    QImage Qtemp;
    if (ui->debugBt->isChecked()) {
        hsvimg = getHsvImg();
        debugHsvImg();
        Qtemp = QImage((const unsigned char*)(hsvimg.data), 640, 480, hsvimg.step, QImage::Format_Indexed8);
    } else {
        Qtemp = QImage((const unsigned char*)(img.data), 640, 480, img.step, QImage::Format_RGB888);
    }

    ui->CameraLabel->setPixmap(QPixmap::fromImage(Qtemp));
    ui->CameraLabel->resize(Qtemp.size());
}

float Widget::getHsvIou(cv::Mat hsv)
{
    int w = hsv.cols;
    int h = hsv.rows;
    int lx = clamp(ui->label_auto->y() - ui->CameraLabel->y(), 0, h);
    int rx = clamp(ui->label_auto->y() + ui->label_auto->height() - ui->CameraLabel->y(), 0, h);
    int ly = clamp(ui->label_auto->x() - ui->CameraLabel->x(), 0, w);
    int ry = clamp(ui->label_auto->x() + ui->label_auto->width() - ui->CameraLabel->x(), 0, w);
    // 由于标志物比较小，所以缩小area的面积，扩大iou的值
    int area = (ry - ly) * (rx - lx) / 100;
    int whitev = 0;

    // 遍历最后20行个值, i是h的值，j是w的值
    for (int i = lx ; i < rx; i++) {
        for (int j = ly ; j < ry; j++) {
            whitev += (hsv.at<uchar>(i, j) == 255);
        }
    }

    float iou = 1.0*whitev/area;
    return iou;
}

void Widget::debugHsvImg()
{
    if (hsvimg.empty()) {
        return;
    }

    float iou = getHsvIou(hsvimg);
    if (iou < ui->miniou->value()) {
        ui->maxioulabel->setStyleSheet("");
        ui->minioulabel->setStyleSheet("background-color: red");
    } else if (iou > ui->maxiou->value()) {
        ui->maxioulabel->setStyleSheet("background-color: red");
        ui->minioulabel->setStyleSheet("");
    } else {
        ui->maxioulabel->setStyleSheet("");
        ui->minioulabel->setStyleSheet("");
    }

}

cv::Mat Widget::getHsvImg()
{
    cv::Mat hsv;
    cvtColor(img, hsv, cv::COLOR_RGB2HSV);
    int hmin = ui->hminlabel->text().toInt();
    int hmax = ui->hmaxlabel->text().toInt();
    int smin = ui->sminlabel->text().toInt();
    int smax = ui->smaxlabel->text().toInt();
    int vmin = ui->vminlabel->text().toInt();
    int vmax = ui->vmaxlabel->text().toInt();

    cv::Mat temp;
    if (hmin > hmax) {
        cv::Mat h1;
        inRange(hsv, cv::Scalar(0, smin, vmin), cv::Scalar(hmax, smax, vmax), h1);
        cv::Mat h2;
        inRange(hsv, cv::Scalar(hmin, smin, vmin), cv::Scalar(255, smax, vmax), h2);
        cv::add(h1, h2, temp);
    } else {
        inRange(hsv, cv::Scalar(hmin, smin, vmin), cv::Scalar(hmax, smax, vmax), temp);
    }

    return temp;
}

void Widget::saveImg()
{
    cv::Mat hsv = getHsvImg();
    if (hsv.empty()) {
        return;
    }

    float iou = getHsvIou(hsv);

    if (readcap) {
        if (iou < ui->miniou->value()) {
            qDebug() <<" 条件成立，保存图片";
            timerp->start(140);
            readcap = false;
        }
    } else if (iou > ui->maxiou->value()) {
        readcap = true;
    }
}

int Widget::clamp(int v, int minv, int maxv)
{
    if (v < minv) {
        v = minv;
    }

    if (v > maxv) {
        v = maxv;
    }

    return v;
}

//点击了拍照按键
void Widget::on_photographBt_clicked()
{
    if (!simg.empty())
    {
        QDateTime dateTime(QDateTime::currentDateTime());
        QString time = dateTime.toString("yyyy-MM-dd-hh-mm-ss");
        //创建图片保存路径名
        QString filename = ui->label_path->text() + QString("/%1.jpg").arg(time);
        cv::imwrite(filename.toStdString().c_str(), simg);
    }

    timerp->stop();
}

void Widget::on_startBt_clicked()
{
    if (cap.isOpened()) {
        cap.release();
        timer->stop();
    }

    cap.open(ui->deviceComboBox->currentText().toInt());
//    cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
//    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1070);
    timer->start(30);
}

void Widget::on_autoPhotographBt_clicked()
{
    bool checked = ui->autoPhotographBt->isChecked();
    ui->photographBt->setEnabled(!checked);
    if (checked) {
        ui->autoPhotographBt->setText("拍照中");
        ui->debugBt->setChecked(false);
        on_debugBt_clicked(false);
    } else {
        ui->autoPhotographBt->setText("自动拍照");
    }

    recnum = 0;
}

void Widget::on_debugBt_clicked(bool checked)
{
    if (checked) {
        ui->debugWidget->show();
        ui->label_auto->show();
        resize(width(), 900);
         ui->autoPhotographBt->setChecked(false);
    } else {
        ui->debugWidget->hide();
        ui->label_auto->hide();
        resize(width(), 650);
    }
}

void Widget::on_changepathBt_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setFileMode(QFileDialog::Directory);
    fileDialog->exec();
    auto path = fileDialog->selectedFiles();
    ui->label_path->setText(path[0]);
}

void Widget::on_hmin_valueChanged(int value)
{
    ui->hminlabel->setText(QString::number(value));
}

void Widget::on_hmax_valueChanged(int value)
{
    ui->hmaxlabel->setText(QString::number(value));
}

void Widget::on_smin_valueChanged(int value)
{
    if (value > ui->smax->value()) {
        value = ui->smax->value();
        ui->smin->setValue(value);
    }

    ui->sminlabel->setText(QString::number(value));
}

void Widget::on_smax_valueChanged(int value)
{
    ui->smaxlabel->setText(QString::number(value));
}

void Widget::on_vmin_valueChanged(int value)
{
    if (value > ui->vmax->value()) {
        value = ui->vmax->value();
        ui->vmin->setValue(value);
    }

    ui->vminlabel->setText(QString::number(value));
}

void Widget::on_vmax_valueChanged(int value)
{
    ui->vmaxlabel->setText(QString::number(value));
}

