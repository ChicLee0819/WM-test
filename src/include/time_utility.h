// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef TIME_UTILITY_H
#define TIME_UTILITY_H

#include <string>
#include <vector>

#define TIMESYNCD_SERVICE_TIMEOUT 20

using namespace std;

class ITimeUtility {
public:
    virtual ~ITimeUtility() {}
    virtual pair<vector<string>, bool> get_timezones() = 0;
    virtual pair<string, bool> get_current_timezone() = 0;
    virtual pair<string, bool> get_current_date() = 0;
    virtual pair<string, bool> get_current_time() = 0;
    virtual pair<string, bool> get_ntp_server() = 0;
    virtual pair<bool, bool> get_ntp_enabled() = 0;
    virtual pair<string, bool> set_timezone(const char* timezone) = 0;
    virtual pair<string, bool> set_manual_date_time(const char* datetime) = 0;
    virtual pair<string, bool> set_sync_with_ntp_server(bool enabled, const char* ntpServer) = 0;
};

class TPCTimeUtility: public ITimeUtility {
public:
    pair<vector<string>, bool> get_timezones() override;
    pair<string, bool> get_current_timezone() override;
    pair<string, bool> get_current_date() override;
    pair<string, bool> get_current_time() override;
    pair<string, bool> get_ntp_server() override;
    pair<bool, bool> get_ntp_enabled() override;
    pair<string, bool> set_timezone(const char* timezone) override;
    pair<string, bool> set_manual_date_time(const char* datetime) override;
    pair<string, bool> set_sync_with_ntp_server(bool enabled, const char* ntpServer) override;

private:
    pair<string, bool> _set_time_info(const char* cmd);
    void _wait_ntp_client_stopped();
};

#endif // TIME_UTILITY_H
