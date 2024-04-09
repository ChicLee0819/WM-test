// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <QDebug>

#include "./include/utility.h"
#include "./include/ftp_utility.h"

/*
ftpget [OPTIONS] HOST LOCAL_FILE REMOTE_FILE
Retrieve a remote file via FTP
Options:
        -c,--continue   Continue previous transfer
        -v,--verbose    Verbose
        -u,--username   Username
        -p,--password   Password
        -P,--port       Port number
*/
const char* FTPGET_CMD = "ftpget -u %s -p %s -P %s %s %s %s 2>&1";

bool TPCFTPUtility::download_from_remote(const char* server, const char* port, const char* username, 
        const char* password, const char* remotePath, const char* localPath) {
    char localFile[BUFF_SIZE] = {0};
    // check input
    if (!server || !port || !username || !password || !remotePath || !localPath) {
        qDebug("missing parameter");
        return false;
    }
    if (strlen(server) == 0 || strlen(port) == 0 || strlen(username) == 0 || 
        strlen(password) == 0 || strlen(remotePath) == 0 || strlen(localPath) == 0) {
        qDebug("empty server:%s port:%s username:%s password:%s remotePath:%s localPath:%s", 
            server, port, username, password, remotePath, localPath);
        return false;
    }
    // check is folder
    if (is_folder_exist(localPath)) {
        snprintf(localFile, BUFF_SIZE, "%s/%s", localPath, get_filename_from_fullpath(remotePath).c_str());
    }
    else {
        snprintf(localFile, BUFF_SIZE, "%s", localPath);
    }
    return execute_cmd_set_info(FTPGET_CMD, username, password, port, server, localFile, remotePath);
}
