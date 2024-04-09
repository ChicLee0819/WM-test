// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <string>
#include <cstring>
#ifdef _WIN32
#else
#include <unistd.h>
#endif
#include <errno.h>
#include <QDebug>
#include <QString>

#include "./include/utility.h"
#include "./include/config_utility.h"
#include "./include/restore_utility.h"
#include "./include/screen_utility.h"
#include "./include/network_utility.h"
#include "./include/time_utility.h"
#include "./include/system_utility.h"
#include "./include/log_utility.h"

using namespace std;

pair<string, bool> RestoreUtility::_extract_file(const char* filepath) {
    bool result = false;
    char importFile[BUFF_SIZE]= {0};
    string msg;
    // check input
    if (!filepath) {
        qDebug("missing parameter");
        return make_pair(msg, result);
    }
    if (!is_file_exist(filepath)) {
        qDebug("import file:%s is not exist", filepath);
        msg = QString("Import file:%1 is not exist.").arg(filepath).toStdString();
        return make_pair(msg, result);
    }
    // untar import file
    result = untar_gz_to_folder(filepath, TMP_FOLDER);
    if (!result) {
        qDebug("incorrect format of import file:%s", filepath);
        msg = QString("Incorrect format of import file:%1").arg(filepath).toStdString();
        return make_pair(msg, result);
    }
    snprintf(importFile, BUFF_SIZE, "%s/%s", TMP_FOLDER, SETTINGS_CONFIG_FILENAME);
    return make_pair(importFile, result);
}

bool RestoreUtility::_restore_screen(ConfigUtility* pConfigUtil) {
    bool isNeedReboot = false;
    IScreenUtility* pScreenUtil = new TPCScreenUtility();
    bool currentIsScreensaver = (pScreenUtil->get_screensaver_idle_time() > 0);
    bool currentIsHideCursor = pScreenUtil->get_hide_cursor();
    string currentTopBarPosition = pScreenUtil->get_top_bar_position();
    string currentRotateScreen = pScreenUtil->get_rotate_screen();
    string currentGestureType = pScreenUtil->get_gesture_type();
    int brightness = pConfigUtil->get_brightness();
    bool isScreensaver = pConfigUtil->get_screensaver_enable();
    int blankAfter = pConfigUtil->get_blank_after();
    bool isHideCursor = pConfigUtil->get_hide_cursor_enable();
    string topBarPositionString = pConfigUtil->get_top_bar_position();
    string rotateScreenString = pConfigUtil->get_rotate_screen();
    string gestureEnableString = pConfigUtil->get_gesture_enable_string();
    bool isGestureEnable = pConfigUtil->get_gesture_enable();
    bool isGestureSwipeDownEnable = pConfigUtil->get_gesture_swipe_down_enable();
    bool isGestureSwipeUpEnable = pConfigUtil->get_gesture_swipe_up_enable();
    bool isGestureSwipeRightEnable = pConfigUtil->get_gesture_swipe_right_enable();
    int setMinutes = 0;
    if (brightness < SCREEN_BRIGHTNESS_MIN || brightness > SCREEN_BRIGHTNESS_MAX) {
        qDebug("there is no screen setting");
        delete pScreenUtil;
        return isNeedReboot;
    }
    if (isScreensaver)
    {
        setMinutes = blankAfter;
    }
    pScreenUtil->set_brightness(brightness);
    pScreenUtil->set_screensaver_idle_time(setMinutes * MINUTE_OF_SECONDS);
    pScreenUtil->set_hide_cursor(isHideCursor);
    // when config is old verion then gesture enable/top bar position string are empty
    // set gesture only if setting is exist
    if (!gestureEnableString.empty() && 
        (currentGestureType.empty() || currentGestureType.compare(GESTURE_TYPE_GENERAL) == 0))
    {
        pScreenUtil->set_all_finger_gesture_enabled(isGestureEnable);
        pScreenUtil->set_2_finger_gesture_swipe_down_enabled(isGestureSwipeDownEnable);
        pScreenUtil->set_2_finger_gesture_swipe_up_enabled(isGestureSwipeUpEnable);
        pScreenUtil->set_2_finger_gesture_swipe_right_enabled(isGestureSwipeRightEnable);
    }
    if (!topBarPositionString.empty()) {
        pScreenUtil->set_top_bar_position(topBarPositionString.c_str());
    }
    if (!rotateScreenString.empty()) {
        pScreenUtil->set_rotate_screen(rotateScreenString.c_str());
    }
    // restart gesture service if rotate screen or gesture changed
    pScreenUtil->restart_gesture_service();

    delete pScreenUtil;
    
    isNeedReboot = (currentIsScreensaver != isScreensaver);
    isNeedReboot |= (currentIsHideCursor != isHideCursor);
    isNeedReboot |= (currentTopBarPosition.compare(topBarPositionString) != 0);
    isNeedReboot |= (currentRotateScreen.compare(rotateScreenString) != 0);
    return isNeedReboot;
}

void RestoreUtility::_restore_network(ConfigUtility* pConfigUtil) {
#ifdef _WIN32
#else
    INetworkUtility* pNetworkUtil = new TPCNetworkUtility();
    bool eth0HasConfigured = pConfigUtil->get_net_has_configured(ETHERNET_0);
    bool eth1HasConfigured = pConfigUtil->get_net_has_configured(ETHERNET_1);
    string method, ip, networkMask, defaultGateway;
    string dns1, dns2, empty;
    vector<string> nameservers;
    pair<string, bool> reteth0, reteth1;
    if (eth0HasConfigured) {
        // get value from config
        method = pConfigUtil->get_net_method(ETHERNET_0);
        ip = pConfigUtil->get_net_ip_address(ETHERNET_0);
        networkMask = pConfigUtil->get_net_subnet_mask(ETHERNET_0);
        defaultGateway = pConfigUtil->get_net_gateway(ETHERNET_0);
        nameservers = pConfigUtil->get_net_dns_servers(ETHERNET_0);
        if (nameservers.size() > 0)
            dns1 = nameservers.at(0);
        if (nameservers.size() > 1)
            dns2 = nameservers.at(1);

        if (method.empty()) {
            qDebug("there is no eth0 setting");
        }
        else {
            // connmanctl service need some time to be ready
            reteth0 = pNetworkUtil->get_ethernet_status_until_timeout(ETHERNET_0);
            if (!reteth0.second) {
                qDebug("connmanctl eth0 is not online");
                // save to config
                pConfigUtil->set_net_has_configured(ETHERNET_0, reteth0.second);
                if (method.compare(MODE_MANUAL) == 0) {
                    // set static ip
                    pNetworkUtil->set_static_ip_address_offline(ETHERNET_0,
                                                                ip.c_str(),
                                                                nullptr,
                                                                networkMask.c_str(),
                                                                defaultGateway.c_str());
                    pNetworkUtil->set_dns_server_offline(ETHERNET_0,
                                                        dns1.c_str(),
                                                        dns2.c_str());
                }
                else if (method.compare(MODE_DHCP) == 0) {
                    // set dhcp
                    pNetworkUtil->set_dhcp_offline(ETHERNET_0, true);
                }
                // create provisioning file
                pNetworkUtil->create_offline_provisioning_file(ETHERNET_0);
            }
            else {
                if (method.compare(MODE_MANUAL) == 0) {
                    // set static ip
                    pNetworkUtil->set_static_ip_address(ETHERNET_0,
                                                        ip.c_str(),
                                                        nullptr,
                                                        networkMask.c_str(),
                                                        defaultGateway.c_str());
                    pNetworkUtil->set_dns_server(ETHERNET_0,
                                                dns1.c_str(),
                                                dns2.c_str());
                }
                else if (method.compare(MODE_DHCP) == 0) {
                    // clear static dns server for getting from dhcp
                    pNetworkUtil->set_dns_server(ETHERNET_0,
                                                empty.c_str(),
                                                empty.c_str());
                    // set dhcp
                    pNetworkUtil->set_dhcp(ETHERNET_0, true);
                }
            }
        }
    }
    if (eth1HasConfigured) {
        // get value from config
        method = pConfigUtil->get_net_method(ETHERNET_1);
        ip = pConfigUtil->get_net_ip_address(ETHERNET_1);
        networkMask = pConfigUtil->get_net_subnet_mask(ETHERNET_1);
        defaultGateway = pConfigUtil->get_net_gateway(ETHERNET_1);
        nameservers = pConfigUtil->get_net_dns_servers(ETHERNET_1);
        if (nameservers.size() > 0)
            dns1 = nameservers.at(0);
        if (nameservers.size() > 1)
            dns2 = nameservers.at(1);

        if (method.empty()) {
            qDebug("there is no eth1 setting");
        }
        else {
            // connmanctl service need some time to be ready
            reteth1 = pNetworkUtil->get_ethernet_status_until_timeout(ETHERNET_1);
            if (!reteth1.second) {
                qDebug("connmanctl eth1 is not online");
                // save to config
                pConfigUtil->set_net_has_configured(ETHERNET_1, reteth1.second);
                if (method.compare(MODE_MANUAL) == 0) {
                    // set static ip
                    pNetworkUtil->set_static_ip_address_offline(ETHERNET_1,
                                                                ip.c_str(),
                                                                nullptr,
                                                                networkMask.c_str(),
                                                                defaultGateway.c_str());
                    pNetworkUtil->set_dns_server_offline(ETHERNET_1,
                                                        dns1.c_str(),
                                                        dns2.c_str());
                }
                else if (method.compare(MODE_DHCP) == 0) {
                    // set dhcp
                    pNetworkUtil->set_dhcp_offline(ETHERNET_1, true);
                }
                // create provisioning file
                pNetworkUtil->create_offline_provisioning_file(ETHERNET_1);
            }
            else {
                if (method.compare(MODE_MANUAL) == 0) {
                    // set static ip
                    pNetworkUtil->set_static_ip_address(ETHERNET_1,
                                                        ip.c_str(),
                                                        nullptr,
                                                        networkMask.c_str(),
                                                        defaultGateway.c_str());
                    pNetworkUtil->set_dns_server(ETHERNET_1,
                                                dns1.c_str(),
                                                dns2.c_str());
                }
                else if (method.compare(MODE_DHCP) == 0) {
                    // clear static dns server for getting from dhcp
                    pNetworkUtil->set_dns_server(ETHERNET_1,
                                                empty.c_str(),
                                                empty.c_str());
                    // set dhcp
                    pNetworkUtil->set_dhcp(ETHERNET_1, true);
                }
            }
        }
    }

    // firewall related
    int rulesCount = pConfigUtil->get_firewall_rules_count();
    vector<map<string, string>> ruleList;
    map<string, string> ruleMap;
    for (int i = 0; i < rulesCount; i++) {
        ruleMap = pConfigUtil->get_firewall_rule_by_index(i);
        ruleList.push_back(ruleMap);
    }
    // set firewall rules
    pNetworkUtil->set_firewall_accept_ports(ruleList);

    delete pNetworkUtil;
#endif
}

void RestoreUtility::_restore_time(ConfigUtility* pConfigUtil) {
    ITimeUtility* pTimeUtil = new TPCTimeUtility();
    string timezone = pConfigUtil->get_timezone();
    bool isNTP = pConfigUtil->get_ntp_enable();
    string ntpServer = pConfigUtil->get_ntp_server();
    string date = pConfigUtil->get_date();
    int hour = pConfigUtil->get_hour();
    int minute = pConfigUtil->get_minute();
    int second = pConfigUtil->get_second();
    QString datetime;
    if (timezone.empty()) {
        qDebug("there is no time setting");
        delete pTimeUtil;
        return;
    }
    datetime = QString("%1 %2:%3:%4").arg(date.c_str()).arg(
        QString::number(hour), QString::number(minute), QString::number(second));
    // set timezone
    pTimeUtil->set_timezone(timezone.c_str());
    // set sync with ntp
    pTimeUtil->set_sync_with_ntp_server(isNTP, ntpServer.c_str());
    if (!isNTP)
    {
        // set manual time
        pTimeUtil->set_manual_date_time(datetime.toStdString().c_str());
    }

    delete pTimeUtil;
}

bool RestoreUtility::_restore_system(ConfigUtility* pConfigUtil) {
    bool isNeedReboot = false;
    ISystemUtility* pSystemUtil = new TPCSystemUtility();
    bool currentIsUserLogin = pSystemUtil->get_system_user_login_desktop();
    bool currentIsCustomUserLogin = pSystemUtil->get_system_custom_user_login_desktop();
    string userLoginString = pConfigUtil->get_system_user_login_desktop_string();
    bool isUserLogin = pConfigUtil->get_system_user_login_desktop();
    string usbEnableString = pConfigUtil->get_usb_enable_string();
    string ethernetEnableString = pConfigUtil->get_ethernet_enable_string();
    bool isUSBEnabled = pConfigUtil->get_usb_enable();
    bool isEthernetEnabled = pConfigUtil->get_ethernet_enable();
    bool isRSCronEnable = pConfigUtil->get_reboot_system_crontab_enabled();
    const auto cronMode = pConfigUtil->get_reboot_system_crontab_mode();
    int cronMinute = pConfigUtil->get_reboot_system_crontab_minute();
    int cronHour = pConfigUtil->get_reboot_system_crontab_hour();
    int cronDayofweek = pConfigUtil->get_reboot_system_crontab_dayofweek();
    string com1Mode = pConfigUtil->get_com1_mode();
    string com1BaudRate = pConfigUtil->get_com1_baudrate();
    string com2BaudRate = pConfigUtil->get_com2_baudrate();
    // set com port
    if (com1Mode.empty()) {
        qDebug("there is no COM setting");
        delete pSystemUtil;
        return isNeedReboot;
    }
    pSystemUtil->do_init_com_port();
    // set restart system crontab
    pSystemUtil->set_reboot_system_crontab(isRSCronEnable, cronMode.c_str(),
                                            cronMinute, cronHour, cronDayofweek);
    // set usb if value exists
    if (!usbEnableString.empty()) {
        pSystemUtil->set_usb_enable(isUSBEnabled);
    }
    // set ethernet if value exists
    if (!ethernetEnableString.empty()) {
        pSystemUtil->do_init_ethernet();
    }
    // NOTE: if desktop use custom user login
    // do not overwrite because the setting is set by manual
    if (!userLoginString.empty() && !currentIsCustomUserLogin) {
        pSystemUtil->set_system_user_login_desktop(isUserLogin, false);
    }

    delete pSystemUtil;
    
    // need reboot for different user login
    isNeedReboot = (currentIsUserLogin != isUserLogin);
    // need reboot for crontab
    isNeedReboot |= isRSCronEnable;
    return isNeedReboot;
}

bool RestoreUtility::_is_readonly_mode() {
    ISystemUtility* pSystemUtil = new TPCSystemUtility();
    bool is_readonly_mode = pSystemUtil->get_readonly_mode();
    delete pSystemUtil;
    return is_readonly_mode;
}

bool RestoreUtility::restore_config_from_backup() {
    const char* backupFile = ConfigUtility::get_backup_config_path();
    if (!is_file_exist(backupFile)) {
        qDebug("backup config is not exist");
        return false;
    }
    // untar import file
    auto retResult = _extract_file(backupFile);
    string importFile = retResult.first;
    if (!retResult.second) {
        return retResult.second;
    }
    ConfigUtility configUtil(importFile.c_str());
    // restore settings
    _restore_screen(&configUtil);
    _restore_time(&configUtil);
    _restore_network(&configUtil);
    _restore_system(&configUtil);
    // restore config
    configUtil.restore_config_from_custom_path(importFile.c_str());
    // delete backup file
    remove(backupFile);
    return true;
}

bool RestoreUtility::restore_user_config_from_backup() {
    if (!is_file_exist(ConfigUtility::get_backup_user_config_path())) {
        qDebug("user backup config is not exist");
        return false;
    }
    ConfigUtility configUtil;
    return configUtil.restore_user_config_from_backup();
}

pair<string, bool> RestoreUtility::export_config(const char* exportFile, ConfigUtility* pLocalConfig) {
#ifdef _WIN32
    return make_pair("", EXIT_FAILURE);
#else
    bool result = false;
    if (!exportFile || !pLocalConfig) {
        qDebug("missing parameter");
        return make_pair(exportFile, result);
    }
    // create export file in tar.gz format
    result = tar_folder_to_gz(exportFile, SETTINGS_CONFIG_FOLDER);
    if (!result) {
        qDebug("create export file failed");
        return make_pair(exportFile, result);
    }
    // make sure file flush to disk
    sync();
    return make_pair(exportFile, result);
#endif
}

pair<string, bool> RestoreUtility::import_config(const char* filepath, ConfigUtility* pLocalConfig) {
    bool result = false;
    bool isNeedReboot = false;
    string msg;
    // check input
    if (!filepath || !pLocalConfig) {
        qDebug("missing parameter");
        return make_pair(msg, result);
    }
    // untar import file
    auto retResult = _extract_file(filepath);
    string importFile = retResult.first;
    result = retResult.second;
    if (!result) {
        msg = retResult.first;
        return make_pair(msg, result);
    }
    ConfigUtility configUtil(importFile.c_str());
    // restore settings
    isNeedReboot = _restore_screen(&configUtil);
    _restore_time(&configUtil);
    _restore_network(&configUtil);
    isNeedReboot |= _restore_system(&configUtil);

    // get decrypted data
    string importLoginPassword = configUtil.get_login_password();
    string importVncPassword = configUtil.get_vnc_server_password();
    string importFtpPassword = configUtil.get_ftp_server_password();

    // encrypted key should be indenpendent from each device
    // keep original encrypted key
    string originUuid = pLocalConfig->get_uuid();
    // replace config file
    result = pLocalConfig->restore_config_from_custom_path(importFile.c_str());
    if (!result) {
        qDebug("import config failed");
        msg = "Import config failed.";
        return make_pair(msg, result);
    }
    // restore original encrypted key
    pLocalConfig->set_uuid(originUuid.c_str());

    // re-encrypt with original encrypted key
    pLocalConfig->set_login_password(importLoginPassword.c_str());
    pLocalConfig->set_vnc_server_password(importVncPassword.c_str());
    pLocalConfig->set_ftp_server_password(importFtpPassword.c_str());

    msg = "Success.";
    if (_is_readonly_mode()) {
        msg.append(" But the system is readonly mode. If you want to keep import settings, you should disable readonly mode first.");
    }
    if (isNeedReboot) {
        msg.append(" Please reboot your device for these changes to take effect.");
    }
    return make_pair(msg, result);
}
