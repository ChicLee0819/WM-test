// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef FTP_UTILITY_H
#define FTP_UTILITY_H

class IFTPUtility {
public:
    virtual ~IFTPUtility() {}
    virtual bool download_from_remote(const char* server, const char* port, const char* username, 
        const char* password, const char* remotePath, const char* localPath) = 0;
};

class TPCFTPUtility: public IFTPUtility {
public:
    bool download_from_remote(const char* server, const char* port, const char* username, 
        const char* password, const char* remotePath, const char* localPath) override;
};

#endif // FTP_UTILITY_H
