// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <QDebug>

#include "./include/utility.h"
#include "./include/system_utility.h"

#define READONLY_ON_OPTION "-install"
#define READONLY_OFF_OPTION "-uninstall"
#define APP_RS_CRON_MODE_DAILY "daily"
#define APP_RS_CRON_MODE_WEEKLY "weekly"

#define SETTINGS_CRONTAB_FILE "/etc/cron.d/settings"

const char *BOOT_FROM_SD_CARD_CMD = "cat /proc/cmdline | grep 'root=/dev/mmcblk1'";
const char *GET_READONLY_MODE_CMD = "atcc.rofs -show | grep 'ReadOnly enable'";
const char *SET_READONLY_MODE_CMD = "atcc.rofs %s";
const char *GET_ROOT_USER_LOGIN_WESTON_CMD = "pgrep -u root weston";
const char *GET_WESTON_USER_LOGIN_WESTON_CMD = "pgrep -u weston weston";
const char *SET_USER_LOGIN_WESTON_CMD = "/usr/bin/adv_run_weston_as_user.sh %s";
const char *GET_USB_STATE_CMD = "/usr/bin/adv_usb_device.sh | tr -d '\\n'";
const char *SET_USB_STATE_CMD = "/usr/bin/adv_usb_device.sh %s";
const char *SET_REBOOT_SYSTEM_CRON_DAILY_CMD = 
    "echo $'%d %d * * * root /sbin/reboot\n' > %s";
const char *SET_REBOOT_SYSTEM_CRON_WEEKLY_CMD = 
    "echo $'%d %d * * %d root /sbin/reboot\n' > %s";
const char *RESTART_CROND_SERVICE_CMD = "systemctl restart crond.service";
const char *INIT_COM_PORT_CMD = "/usr/bin/init_com_port.sh";
const char *INIT_ETHERNET_CMD = "/usr/bin/init_ethernet.sh";
const char *REBOOT_CMD = "( /bin/sleep 1; /sbin/reboot ) &";
const char *SHUTDOWN_CMD = "( /bin/sleep 1; /sbin/shutdown -h now ) &";
const char *OPEN_TERMINAL_CMD = "/usr/bin/weston-terminal --maximized --shell=/bin/sh &";
const char *OPEN_LICENSE_PAGE_CMD = "/usr/bin/chromium --no-sandbox --test-type --start-maximized --hide-crash-restore-bubble /usr/share/html/license_page.html &";
const char *SET_PSPLASH_BG_COLOR_CMD = "sed -i '/^BACKGROUND_COLOR=/s/=.*/=%s/' /etc/psplash.conf";

bool TPCSystemUtility::is_boot_from_sd_card()
{
    bool isBootFromSD = false;
    // execute command to get boot from sd card
    const auto ret = execute_cmd_get_single_info(BOOT_FROM_SD_CARD_CMD);
    isBootFromSD = (!ret.first.empty());
    return isBootFromSD;
}

bool TPCSystemUtility::get_readonly_mode()
{
    bool isReadonly = false;
    const auto ret = execute_cmd_get_single_info(GET_READONLY_MODE_CMD);
    isReadonly = (!ret.first.empty());
    return isReadonly;
}

bool TPCSystemUtility::set_readonly_mode(const bool readonly)
{
    const char *option = readonly ? READONLY_ON_OPTION : READONLY_OFF_OPTION;
    return execute_cmd_set_info(SET_READONLY_MODE_CMD, option);
}

bool TPCSystemUtility::get_system_user_login_desktop()
{
    bool isRootUserLogin = false;
    const auto ret = execute_cmd_get_single_info(GET_ROOT_USER_LOGIN_WESTON_CMD);
    isRootUserLogin = (!ret.first.empty());
    return !isRootUserLogin;
}

bool TPCSystemUtility::get_system_custom_user_login_desktop()
{
    bool isCustomUserLogin = false;
    const auto retRoot = execute_cmd_get_single_info(GET_ROOT_USER_LOGIN_WESTON_CMD);
    bool isRootUserLogin = (retRoot.first.length() > 0);
    if (isRootUserLogin)
        return isCustomUserLogin;
    const auto retWeston = execute_cmd_get_single_info(GET_WESTON_USER_LOGIN_WESTON_CMD);
    bool isWestonUserLogin = (retWeston.first.length() > 0);
    if (isWestonUserLogin)
        return isCustomUserLogin;
    // not root and weston means custom user
    isCustomUserLogin = true;
    return isCustomUserLogin;
}

bool TPCSystemUtility::get_usb_enable()
{
    bool isDisabled = false;
    const auto ret = execute_cmd_get_single_info(GET_USB_STATE_CMD);
    isDisabled = (ret.first.compare(STRING_DISABLE) == 0);
    return !isDisabled;
}

bool TPCSystemUtility::set_system_user_login_desktop(bool isUserLogin, bool isReboot)
{
    const char *option = isUserLogin ? WESTON_USER : ROOT_USER;
    bool result = execute_cmd_set_info(SET_USER_LOGIN_WESTON_CMD, option);
    if (result && isReboot) {
        return do_reboot();
    }
    return result;
}

bool TPCSystemUtility::set_usb_enable(const bool enabled)
{
    if (enabled)
        return execute_cmd_set_info(SET_USB_STATE_CMD, STRING_ENABLE);
    else
        return execute_cmd_set_info(SET_USB_STATE_CMD, STRING_DISABLE);
}

bool TPCSystemUtility::set_reboot_system_crontab(bool enabled, const char* mode, 
                                                  int minute, int hour, int dayofweek)
{
    bool result = false;
    // check input
    if (!mode) {
        qDebug("missing parameter");
        return result;
    }
    if (strlen(mode) == 0) {
        qDebug("empty mode:%s", mode);
        return result;
    }
    // remove last crontab file
    if (is_file_exist(SETTINGS_CRONTAB_FILE)) {
        remove(SETTINGS_CRONTAB_FILE);
    }
    if (enabled) {
        if (strcmp(mode, APP_RS_CRON_MODE_DAILY) == 0) {
            result = execute_cmd_set_info(SET_REBOOT_SYSTEM_CRON_DAILY_CMD, minute, hour, SETTINGS_CRONTAB_FILE);
        } else if (strcmp(mode, APP_RS_CRON_MODE_WEEKLY) == 0) {
            result = execute_cmd_set_info(SET_REBOOT_SYSTEM_CRON_WEEKLY_CMD, minute, hour, dayofweek, SETTINGS_CRONTAB_FILE);
        } else {
            qDebug("unknown mode:%s", mode);
        }
    } else {
        result = true;
    }
    return result;
}

bool TPCSystemUtility::set_boot_logo_background_color(const char* color)
{
    // check input
    if (!color || strlen(color) == 0) {
        qDebug("missing parameter");
        return false;
    }
    return execute_cmd_set_info(SET_PSPLASH_BG_COLOR_CMD, color);
}

bool TPCSystemUtility::do_restart_crond_service()
{
    return execute_cmd_set_info(RESTART_CROND_SERVICE_CMD);
}

bool TPCSystemUtility::do_init_com_port()
{
    return execute_cmd_set_info(INIT_COM_PORT_CMD);
}

bool TPCSystemUtility::do_init_ethernet()
{
    return execute_cmd_set_info(INIT_ETHERNET_CMD);
}

bool TPCSystemUtility::do_reboot()
{
    return execute_cmd_set_info(REBOOT_CMD);
}

bool TPCSystemUtility::do_shutdown()
{
    return execute_cmd_set_info(SHUTDOWN_CMD);
}

bool TPCSystemUtility::open_terminal()
{
    return execute_cmd_set_info(OPEN_TERMINAL_CMD);
}

bool TPCSystemUtility::open_license_page()
{
    return execute_cmd_set_info(OPEN_LICENSE_PAGE_CMD);
}
