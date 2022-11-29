#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QtMessageHandler>

QString getLogName();
void onMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // LOG_H
