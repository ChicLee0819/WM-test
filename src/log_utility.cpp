// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include "./include/log_utility.h"

#include <QFile>
#include <QTextStream>
#include <QDateTime>

void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // check current log size
    QFile fileCheck(LOG_FILE_NAME);
    if (fileCheck.size() > LOG_SIZE) {
        fileCheck.remove();
    }

    QString text;
    switch (type)
    {
        case QtDebugMsg:
            text = QString("Debug:");
            break;
        case QtInfoMsg:
            text = QString("Info:");
            break;
        case QtWarningMsg:
            text = QString("Warning:");
            break;
        case QtCriticalMsg:
            text = QString("Critical:");
            break;
        case QtFatalMsg:
            text = QString("Fatal:");
        default:
            break;
    }
    // debug log message
    QString contextInfo = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString currentTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
    QString message = QString::fromLocal8Bit("%1 - %2 %3 %4").arg(currentTime).arg(text).arg(msg).arg(contextInfo);

    // write to file
    QFile outFile(LOG_FILE_NAME);
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream textStream(&outFile);
    textStream << message << Qt::endl;

    outFile.flush();
    outFile.close();
}
