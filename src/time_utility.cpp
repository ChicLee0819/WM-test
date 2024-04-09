// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <sstream>
#ifdef _WIN32
#else
#include <unistd.h>
#endif
#include <QDebug>

#include "./include/utility.h"
#include "./include/time_utility.h"

const char* TYPE_ACTIVE = "active";
const char* TYPE_INACTIVE = "inactive";

// Network Time Synchronization configuration files path
const char* NTP_CONF_PATH =            "/etc/systemd/timesyncd.conf.d";
const char* NTP_CONF_FILENAME =        "userntp.conf";
const char* NTP_CONF_TEMPLATE =        "[Time]\nNTP=%s";
const char* RESTART_TIMESYNCD_CMD =    "systemctl restart systemd-timesyncd";
const char* GET_TIMESYNCD_STATUS_CMD = "systemctl is-active systemd-timesyncd | tr -d '\\n'";

// ex: timedatectl
/*
               Local time: Fri 2022-01-14 07:55:44 CST
           Universal time: Thu 2022-01-13 23:55:44 UTC
                 RTC time: Thu 2022-01-13 23:55:44
                Time zone: Asia/Taipei (CST, +0800)
System clock synchronized: no
              NTP service: inactive
          RTC in local TZ: no
*/
const char* GET_CURRENT_TIMEZONE_CMD = "timedatectl | grep -i 'Time zone' | awk -F ' ' '{print $3}' | tr -d '\\n'";
const char* GET_CURRENT_DATE_CMD =     "timedatectl | grep -i 'Local time' | awk -F ' ' '{print $4}' | tr -d '\\n'";
const char* GET_CURRENT_TIME_CMD =     "timedatectl | grep -i 'Local time' | awk -F ' ' '{print $5}' | tr -d '\\n'";
const char* GET_NTP_ENABLED_CMD =      "timedatectl | grep -i 'NTP service' | awk -F ' ' '{print $3}' | tr -d '\\n'";
const char* GET_NTP_SERVER_CMD =       "timedatectl show-timesync -p ServerName --value | tr -d '\\n'";
const char* ENABLE_NTP_CMD =           "timedatectl set-ntp true 2>&1";
const char* DISABLE_NTP_CMD =          "timedatectl set-ntp false 2>&1";
const char* SET_TIMEZONE_CMD =         "timedatectl set-timezone '%s' 2>&1";
const char* SET_DATETIME_CMD =         "timedatectl set-time '%s' 2>&1";
const char* LIST_TIMEZONES_CMD =       "timedatectl list-timezones";
const char* DISABLE_COLOR_CODE_ENV =   "SYSTEMD_LOG_COLOR=false";

pair<vector<string>, bool> TPCTimeUtility::get_timezones() {
    return execute_cmd_get_vector(LIST_TIMEZONES_CMD);
}

pair<string, bool> TPCTimeUtility::get_current_timezone() {
    return execute_cmd_get_single_info(GET_CURRENT_TIMEZONE_CMD);
}

pair<string, bool> TPCTimeUtility::get_ntp_server() {
    const auto ret = get_ntp_enabled();
    if (ret.first)
        return execute_cmd_get_single_info(GET_NTP_SERVER_CMD);
    else
        return make_pair(string(), ret.second);
}

pair<bool, bool> TPCTimeUtility::get_ntp_enabled() {
    const auto ret = execute_cmd_get_single_info(GET_NTP_ENABLED_CMD);
    bool ntp_enabled = (ret.first.compare(TYPE_ACTIVE) == 0);
    return make_pair(ntp_enabled, ret.second);
}

pair<string, bool> TPCTimeUtility::get_current_date() {
    return execute_cmd_get_single_info(GET_CURRENT_DATE_CMD);
}

pair<string, bool> TPCTimeUtility::get_current_time() {
    return execute_cmd_get_single_info(GET_CURRENT_TIME_CMD);
}

pair<string, bool> TPCTimeUtility::_set_time_info(const char* cmd) {
    bool result = false;
    char cmd_buff[CMD_SIZE]= {0};
    // check input
    if (!cmd || strlen(cmd) == 0) {
        qDebug("missing cmd");
        return make_pair("Missing command", result);
    }

    snprintf(cmd_buff, CMD_SIZE, "%s %s", DISABLE_COLOR_CODE_ENV, cmd);
    const auto ret = execute_cmd(cmd_buff);
    result = (ret.second == EXIT_SUCCESS);
    qDebug("cmd:%s ret:%d stderr:%s", cmd_buff, ret.second, ret.first.c_str());
    return make_pair(ret.first, result);
}

pair<string, bool> TPCTimeUtility::set_timezone(const char* timezone) {
    char cmd_buff[CMD_SIZE]= {0};
    // check input
    if (!timezone || strlen(timezone) == 0) {
        qDebug("missing timezone");
        return make_pair("Missing timezone", false);
    }

    snprintf(cmd_buff, CMD_SIZE, SET_TIMEZONE_CMD, timezone);
    return _set_time_info(cmd_buff);
}

pair<string, bool> TPCTimeUtility::set_manual_date_time(const char* datetime) {
    char cmd_buff[CMD_SIZE]= {0};
    // check input
    if (!datetime || strlen(datetime) == 0) {
        qDebug("missing datetime");
        return make_pair("Missing datetime", false);
    }

    // wait ntp client stopped to avoid "Failed to set time: Previous request is not finished, refusing."
    _wait_ntp_client_stopped();

    // set manual datetime
    snprintf(cmd_buff, CMD_SIZE, SET_DATETIME_CMD, datetime);
    return _set_time_info(cmd_buff);
}

pair<string, bool> TPCTimeUtility::set_sync_with_ntp_server(bool enabled, const char* ntpServer) {
    if (enabled) {
        // check input
        if (!ntpServer)
            return make_pair("Missing ntp server", false);

        // set ntp server
        if (strlen(ntpServer) > 0) {
            char file_buff[CMD_SIZE]= {0};
            char context_buff[CMD_SIZE]= {0};
            sys_mkdir(NTP_CONF_PATH);
            snprintf(file_buff, CMD_SIZE, "%s/%s", NTP_CONF_PATH, NTP_CONF_FILENAME);
            snprintf(context_buff, CMD_SIZE, NTP_CONF_TEMPLATE, ntpServer);
            write_file(file_buff, context_buff);
            // restart timesync daemon
            _set_time_info(RESTART_TIMESYNCD_CMD);
        }
        // enable ntp
        return _set_time_info(ENABLE_NTP_CMD);
    }
    else {
        // disable ntp
        return _set_time_info(DISABLE_NTP_CMD);
    }
}

void TPCTimeUtility::_wait_ntp_client_stopped() {
#ifdef _WIN32
    return;
#else
    int seconds = 0;
    auto ret = execute_cmd_get_single_info(GET_TIMESYNCD_STATUS_CMD);
    while (ret.first.compare(TYPE_INACTIVE) != 0 && seconds < TIMESYNCD_SERVICE_TIMEOUT) {
        sleep(1);
        ret = execute_cmd_get_single_info(GET_TIMESYNCD_STATUS_CMD);
        if (ret.first.compare(TYPE_INACTIVE) == 0)
            break;
        seconds++;
    }
#endif
}
