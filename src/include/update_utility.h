// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef UPDATE_UTILITY_H
#define UPDATE_UTILITY_H

#define TMP_SWUPDATE_FILE "/userdata/tmp_update.swu"
#define NEWER_VERSION  0
#define OLDER_VERSION 1
#define NO_SW_DESC_FILE 2

class IUpdateUtility {
public:
    virtual ~IUpdateUtility() {}
    void cleanup_files();

    virtual bool start_web_update() = 0;
    virtual bool start_local_update(const char* filepath) = 0;
    virtual bool start_factory_reset() = 0;
    virtual bool start_factory_check() = 0;
    virtual bool check_factory_status() = 0;
    virtual int check_downgrading(const char* filepath) = 0;
};

class TPCUpdateUtility: public IUpdateUtility {
public:
    bool start_web_update() override;
    bool start_local_update(const char* filepath) override;
    bool start_factory_reset() override;
    bool start_factory_check() override;
    bool check_factory_status() override;
    int check_downgrading(const char* filepath) override;

};

#endif // UPDATE_UTILITY_H
