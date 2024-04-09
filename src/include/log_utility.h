// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef LOG_UTILITY_H
#define LOG_UTILITY_H

#include <QDebug>

#define MINUTE_OF_SECONDS 60
#define LOG_SIZE 1024 * 100 // log size in bytes
#ifdef _WIN32
#define LOG_FILE_NAME APP_LOG
#else
#define LOG_FILE_NAME "/tmp/" APP_LOG
#endif

void logMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

#endif // LOG_UTILITY_H
