// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef CONFIG_UTILITY_H
#define CONFIG_UTILITY_H

#include <string>
#include <vector>
#include <map>

#define APP_MODE_INIT "init"
#define APP_MODE_GENERAL "general"
#define APP_THEME_GREEN "green"
#define APP_THEME_ORANGE "orange"

#define WIZARD_CREDENTIALS "credentials"
#define WIZARD_NETWORK     "wiznetwork"
#define WIZARD_TIME        "wiztime"
#define WIZARD_SCREEN      "wizscreen"
#define WIZARD_STARTUP     "wizstartup"

#define SETTINGS_CONFIG_FILE "/etc/settings/settings_config.ini"
#define BACKUP_CONFIG_FILE "/userdata/.backup_settings_config.tpc"
#define SETTINGS_CONFIG_FOLDER "/etc/settings"
#define SETTINGS_CONFIG_FILENAME "settings_config.ini"
#define EXPORT_CONFIG_FILENAME "export_settings_config.tpc"
#define CONFIG_FILE_CODEC "UTF-8"

#define USER_CONFIG_FILE "/etc/shadow"
#define BACKUP_USER_CONFIG_FILE "/userdata/.backup_shadow"

using namespace std;

class QVariant;

class ConfigUtility {
public:
    ConfigUtility();
    explicit ConfigUtility(const char* configFile);

    // UI related
    static string get_ui_theme();
    string get_keyboard_locale();

    // core related
    string get_uuid();
    void set_uuid(const char* uuid);
    string get_app_mode();
    string get_next_page_by_index(int index);

    // credentials related
    bool get_credentials_page_is_showed();
    bool get_root_password_required();
    bool get_weston_password_required();

    // wizard related
    bool get_wizard_network_page_is_showed();
    bool get_wizard_time_page_is_showed();
    bool get_wizard_screen_page_is_showed();
    bool get_wizard_startup_page_is_showed();

    // screen related
    bool get_screen_page_is_showed();
    int get_brightness();
    bool get_screensaver_enable();
    int get_blank_after();
    bool get_hide_cursor_enable();
    string get_top_bar_position();
    string get_rotate_screen();
    string get_gesture_enable_string();
    bool get_gesture_enable();
    bool get_gesture_swipe_down_enable();
    bool get_gesture_swipe_up_enable();
    bool get_gesture_swipe_right_enable();
    void set_brightness(int brightness);
    void set_screensaver_enable(bool enabled);
    void set_blank_after(int blankPeriod);
    void set_hide_cursor_enable(bool enabled);
    void set_top_bar_position(const char* position);
    void set_rotate_screen(const char* rotateDegree);
    void set_gesture_enable(bool enabled);
    void set_gesture_swipe_down_enable(bool enabled);
    void set_gesture_swipe_up_enable(bool enabled);
    void set_gesture_swipe_right_enable(bool enabled);

    // startup related
    string get_startup();
    int get_static_page_timeout();
    string get_static_page_url();
    string get_static_page_file_path();
    bool get_startup_auto_restart();
    void set_startup(const char* name);
    void set_static_page_timeout(int timeout);
    void set_static_page_url(const char* url);
    void set_static_page_file_path(const char* filePath);
    void set_startup_auto_restart(bool isAutoRestart);

    // web pages related
    int get_web_pages_count();
    map<string, string> get_web_page_by_index(int index);
    void set_web_pages_count(int count);
    void set_web_page_by_index(map<string, string> webPage, int index);

    // system related
    bool get_system_page_is_showed();
    string get_com1_mode();
    string get_com2_mode();
    string get_com1_baudrate();
    string get_com2_baudrate();
    string get_system_user_login_desktop_string();
    bool get_system_user_login_desktop();
    bool get_reboot_system_crontab_enabled();
    string get_reboot_system_crontab_mode();
    int get_reboot_system_crontab_minute();
    int get_reboot_system_crontab_hour();
    int get_reboot_system_crontab_dayofweek();
    string get_ethernet_enable_string();
    string get_usb_enable_string();
    bool get_ethernet_enable();
    bool get_usb_enable();
    bool get_chromium_use_sys_virtual_keyboard();
    bool get_chromium_use_custom_virtual_keyboard();
    void set_com1_mode(const char* mode);
    void set_com2_mode(const char* mode);
    void set_com1_baudrate(const char* baudrate);
    void set_com2_baudrate(const char* baudrate);
    void set_system_user_login_desktop(bool isUserLogin);
    void set_reboot_system_crontab_enabled(bool enabled);
    void set_reboot_system_crontab_mode(const char* mode);
    void set_reboot_system_crontab_minute(int minute);
    void set_reboot_system_crontab_hour(int hour);
    void set_reboot_system_crontab_dayofweek(int dayofweek);
    void set_ethernet_enable(bool enabled);
    void set_usb_enable(bool enabled);
    void set_chromium_use_sys_virtual_keyboard(bool enabled);
    bool get_com_function_is_showed_for_user();

    // security related
    bool get_security_page_is_showed();
    bool get_login_enable();
    string get_login_password();
    string get_login_type();
    void set_login_enable(bool enabled);
    void set_login_password(const char* password);
    void set_login_type(const char* type);
    string get_login_name_alias(const char* username);

    // time related
    bool get_time_page_is_showed();
    string get_timezone();
    bool get_ntp_enable();
    string get_ntp_server();
    string get_date();
    int get_hour();
    int get_minute();
    int get_second();
    void set_timezone(const char* timezone);
    void set_ntp_enable(bool enabled);
    void set_ntp_server(const char* server);
    void set_date(const char* date);
    void set_hour(int hour);
    void set_minute(int minute);
    void set_second(int second);

    // VNC related
    string get_vnc_server_address();
    string get_vnc_server_port();
    string get_vnc_server_password();
    bool get_vnc_server_viewonly();
    int get_vnc_server_image_quality();
    bool get_vnc_server_fullscreen();
    bool get_vnc_server_fit_window();
    int get_vnc_server_polling_period();
    void set_vnc_server_address(const char* server);
    void set_vnc_server_port(const char* port);
    void set_vnc_server_password(const char* password);
    void set_vnc_server_viewonly(bool viewonly);
    void set_vnc_server_image_quality(int imageQuality);
    void set_vnc_server_fullscreen(bool fullscreen);
    void set_vnc_server_fit_window(bool fitWindow);
    void set_vnc_server_polling_period(int pollingPeriod);

    // OPC UA server related
    bool get_opcua_page_is_showed();
    bool get_opcua_server_enable();
    string get_opcua_server_port();
    string get_opcua_server_identity_token_type();
    map<string, string> get_opcua_server_user_identity();
    void set_opcua_server_enable(bool enabled);
    void set_opcua_server_port(const char* port);
    void set_opcua_server_identity_token_type(const char* identityType);
    void set_opcua_server_user_identity(map<string, string> userAttributes);

    // FTP related
    bool get_ftp_page_is_showed();
    string get_ftp_server_address();
    string get_ftp_server_port();
    string get_ftp_server_username();
    string get_ftp_server_password();
    string get_ftp_server_remote_path();
    string get_ftp_server_local_path();
    void set_ftp_server_address(const char* server);
    void set_ftp_server_port(const char* port);
    void set_ftp_server_username(const char* username);
    void set_ftp_server_password(const char* password);
    void set_ftp_server_remote_path(const char* remotePath);
    void set_ftp_server_local_path(const char* localPath);

    // network related
    bool get_network_page_is_showed();
    bool get_net_has_configured(const char* ethernet);
    string get_net_method(const char* ethernet);
    string get_net_ip_address(const char* ethernet);
    string get_net_gateway(const char* ethernet);
    string get_net_subnet_mask(const char* ethernet);
    vector<string> get_net_dns_servers(const char* ethernet);
    void set_net_has_configured(const char* ethernet, bool hasConfigured);
    void set_net_dhcp(const char* ethernet);
    void set_net_static_ip(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway);
    void set_net_dns_server(const char* ethernet, const char* dns1, const char* dns2);

    // firewall rules related
    int get_firewall_rules_count();
    map<string, string> get_firewall_rule_by_index(int index);
    void set_firewall_rules_count(int count);
    void set_firewall_rule_by_index(map<string, string> rule, int index);

    // storage related
    bool get_storage_page_is_showed();
    // update related
    bool get_update_page_is_showed();
    // logo related
    bool get_logo_page_is_showed();
    // password related
    bool get_password_page_is_showed();
    // operate related
    bool get_operate_page_is_showed();
    bool get_export_setting_function_is_showed_for_user();
    bool get_export_screenshot_function_is_showed_for_user();
    bool get_import_setting_function_is_showed_for_user();
    bool get_reboot_function_is_showed_for_user();
    bool get_shutdown_function_is_showed_for_user();
    bool get_open_terminal_function_is_showed_for_user();
    bool get_factory_reset_function_is_showed_for_user();
    // about related
    bool get_about_page_is_showed();
    // exit related
    bool get_exit_page_is_showed();

    // backup related
    static const char* get_config_path();
    static const char* get_backup_config_path();
    static const char* get_user_config_path();
    static const char* get_backup_user_config_path();
    static const char* get_export_config_filename();
    bool get_backup_config_enable();
    bool get_backup_user_enable();
    void set_backup_config_enable(bool enabled);
    void set_backup_user_enable(bool enabled);
    bool backup_config();
    bool backup_config_to_custom_path(const char* targetPath);
    bool restore_config_from_backup();
    bool restore_config_from_custom_path(const char* sourcePath);
    bool backup_user_config();
    bool backup_user_config_to_custom_path(const char* targetPath);
    bool restore_user_config_from_backup();
    bool restore_user_config_from_custom_path(const char* sourcePath);

    // common functions
    bool get_page_is_showed_by_name(const char* name);
    string get_next_page_by_name(const char* name);

private:
    string m_configFile;

    void _generate_uuid();
    void _clear_password_in_config();
    bool _get_page_is_showed(const char* section);
    bool _get_function_is_showed_for_user(const char* section, const char* key);
    QVariant _get_config_value(const char* section, const char* key);
    string _get_config_value_string(const char* section, const char* key);
    string _get_config_value_decrypted_string(const char* section, const char* key);
    void _set_config_value(const char* section, const char* key, QVariant value);
    void _set_config_value_string(const char* section, const char* key, const char* value);
    void _set_config_value_encrypted_string(const char* section, const char* key, const char* value);
    bool _backup_config(const char* source, const char* target);
    bool _restore_config(const char* source, const char* target);
};

#endif // CONFIG_UTILITY_H
