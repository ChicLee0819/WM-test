// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>

#include "./include/utility.h"
#include "./include/update_utility.h"



const char* START_WEB_UPDATE_CMD = "swupdate --update -w \"-r /www\" --reboot";
const char* START_LOCAL_UPDATE_CMD = "sync;sync;swupdate --update -g -i %s -N 1.5 --reboot  & ";
const char* START_FACTORY_RESET_CMD = "sync;sync;atcc.factory reset & ";
const char* START_FACTORY_CHECK_CMD = "atcc.factory check ";
const char* GET_FACTORY_STATUS_CMD = "atcc.factory status ";

void IUpdateUtility::cleanup_files() {
    if (is_file_exist(TMP_SWUPDATE_FILE)) {
        remove(TMP_SWUPDATE_FILE);
    }
}

bool TPCUpdateUtility::start_web_update() {
    qDebug("Start web update!");
    return execute_cmd_set_info(START_WEB_UPDATE_CMD);
}

bool TPCUpdateUtility::start_local_update(const char* filepath) {
    qDebug("Start local update!");
    return execute_cmd_set_info(START_LOCAL_UPDATE_CMD, filepath);
}

bool TPCUpdateUtility::start_factory_reset() {
    qDebug("Start factory reset!");
    return execute_cmd_set_info(START_FACTORY_RESET_CMD);
}

bool TPCUpdateUtility::start_factory_check() {
   qDebug("Start factory check!");
   return execute_cmd_set_info(START_FACTORY_CHECK_CMD);
}

bool TPCUpdateUtility::check_factory_status()
{
    qDebug("Start factory status check!");
    bool isRunning = false;
    const auto ret = execute_cmd_get_single_info(GET_FACTORY_STATUS_CMD);
    isRunning = (!ret.first.empty());
    return isRunning;
}


int TPCUpdateUtility::check_downgrading(const char* filepath) {

    bool is_older_version = false;
    bool find_config = false;

    QFileInfo info(filepath);
    QString sw_descPath = info.absolutePath();
    sw_descPath = sw_descPath + "/sw-description";
    if (QFile::exists(sw_descPath))
    {
	    QFile::remove(sw_descPath);
    }

    const char *GET_VERSION_CMD = " cpio --extract \"sw-description\"  < %s ";
    execute_cmd_set_info(GET_VERSION_CMD, filepath);
    
    QFile inputFile(filepath);
    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream in(&inputFile);
        QString content;
        while (1)
        {
            content = in.readLine();
            if (content.contains("version"))
            {
                //can not downgrading as version imagev1.5 
                QStringList list = content.split("\"");
                int result = qstrcmp(QString(list[1].data()).toStdString().c_str(),"1.5") ;
                if ( result ==0 || result < 0)
                {
                    is_older_version= true;
                }           
                find_config = true;
                break;
            }
        }
        inputFile.close();
        if (QFile::exists(sw_descPath))
        {
            QFile::remove(sw_descPath);
        }
    }
    if (is_older_version)
    {
        return OLDER_VERSION; 
    }
    if (!find_config)
    {
         return NO_SW_DESC_FILE ;
    }
    return NEWER_VERSION ;
}
