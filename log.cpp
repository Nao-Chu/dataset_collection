#include "log.h"

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QTextCodec>

#define TIME QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
#define DATE QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")
#define DIR  "tmp"
#define FILENUM     5
const qint64 FILESIZE = 2*1024*1024;

void onMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QString strOutStream = "";
    QByteArray localMsg = msg.toLocal8Bit();
    QString cfile = QString(context.file);
    cfile = cfile.right(cfile.length() - cfile.lastIndexOf("\\") - 1);
    switch (type) {
    case QtDebugMsg:
        strOutStream = QString("[Debug] [%1 %2 %3 Line:%4]: %5\n")
                .arg(TIME, cfile, QString(context.function))
                .arg(context.line).arg(QString(localMsg));
        break;
    case QtInfoMsg:
        strOutStream = QString("[Info] [%1 %2 %3 Line:%4]: %5\n")
                .arg(TIME, cfile, QString(context.function))
                .arg(context.line).arg(QString(localMsg));
        break;
    case QtWarningMsg:
        strOutStream = QString("[Warning] [%1 %2 %3 Line:%4]: %5\n")
                .arg(TIME, cfile, QString(context.function))
                .arg(context.line).arg(QString(localMsg));
        break;
    case QtCriticalMsg:
        strOutStream = QString("[Critical] [%1 %2 %3 Line:%4]: %5\n")
                .arg(TIME, cfile, QString(context.function))
                .arg(context.line).arg(QString(localMsg));
        break;
    case QtFatalMsg:
        strOutStream = QString("[Fatal] [%1 %2 %3 Line:%4]: %5\n")
                .arg(TIME, cfile, QString(context.function))
                .arg(context.line).arg(QString(localMsg));
        abort();
    }

    // 获取日志文件名字，会固定文件个数以及文件大小
    QString strFileName = getLogName();
    QFile logfile(strFileName);
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    logfile.open(QIODevice::WriteOnly | QIODevice::Append);
    if (!strOutStream.isEmpty()) {
        QTextStream logStream(&logfile);
        logStream.setCodec("utf-8");
        logStream << strOutStream <<"\r\n";
    }
    //清楚缓存文件，解锁
    logfile.flush();
    logfile.close();

    mutex.unlock();
}

QString getLogName()
{
    QDir dir(DIR);
    QStringList filters;
    filters << "*.log";
    dir.setNameFilters(filters);
    dir.setSorting(QDir::Name);

    // 获取最后一个文件，判断文件大小
    QFileInfoList filelst = dir.entryInfoList();
    int count = filelst.count();

    // 不存在文件，则直接新建
    if (count <= 0) {
        return QString("%1/%2.log").arg(DIR, DATE);
    }

    // 获取最后一个文件
    QString strFileName = filelst.at(count - 1).absoluteFilePath();
    QFile logfile(strFileName);
    logfile.open(QIODevice::WriteOnly | QIODevice::Append);

    // 若文件过大，则新建一个文件
    if (logfile.size() > FILESIZE) {
        strFileName = QString("%1/%2.log").arg(DIR, DATE);
        ++count;
    }

    logfile.close();

    // 文件个数不多，无需删除
    if (count < FILENUM){
        return strFileName;
    }

    // 删除多余文件
    for (int i = 0; i < count - FILENUM; ++i) {
        QFileInfo mItem = filelst.at(i);
        QFile::remove(mItem.absoluteFilePath());
    }

    return strFileName;
}
