// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <QDebug>
#include <QSettings>
#include <QUuid>

#include "./include/utility.h"
#include "./include/config_utility.h"
#include "./include/network_utility.h"
#include "./include/startup_utility.h"

// common related
const char* KEY_IS_SHOWED = "is_showed";
const char* KEY_NEXT_PAGE = "next_page";

// UI related
const char* CONF_SECTION_UI = "ui";
const char* KEY_THEME =       "theme";
const char* KEY_KEYBOARD_LOCALE = "keyboard_locale";

// core related
const char* CONF_SECTION_CORE =   "core";
const char* KEY_UUID_NAME =       "uuid";
const char* KEY_APP_MODE =        "app_mode";
const char* KEY_PAGE_INDEX_PATTERN = "page_%d";

// credentials related
const char* CONF_SECTION_CREDENTIALS = WIZARD_CREDENTIALS;
const char* KEY_ROOT_PASSWORD_REQUIRED = "root_password_required";
const char* KEY_WESTON_PASSWORD_REQUIRED = "weston_password_required";

// wizard related
const char* CONF_SECTION_WIZARD_NETWORK = WIZARD_NETWORK;
const char* CONF_SECTION_WIZARD_TIME = WIZARD_TIME;
const char* CONF_SECTION_WIZARD_SCREEN = WIZARD_SCREEN;
const char* CONF_SECTION_WIZARD_STARTUP = WIZARD_STARTUP;

// screen related
const char* CONF_SECTION_SCREEN =    "screen";
const char* KEY_BRIGHTNESS =         "brightness";
const char* KEY_SCREENSAVER_ENABLE = "screensaver_enable";
const char* KEY_BLANK_AFTER_PERIOD = "blank_after_period";
const char* KEY_HIDE_CURSOR_ENABLE = "hide_cursor_enable";
const char* KEY_TOP_BAR_POSITION =   "top_bar_position";
const char* KEY_ROTATE_SCREEN =      "rotate_screen";
const char* KEY_GESTURE_ENABLE =     "gesture_enable";
const char* KEY_GESTURE_SWIPE_DOWN_ENABLE =  "gesture_swipe_down_enable";
const char* KEY_GESTURE_SWIPE_UP_ENABLE =    "gesture_swipe_up_enable";
const char* KEY_GESTURE_SWIPE_RIGHT_ENABLE = "gesture_swipe_right_enable";

// startup related
const char* CONF_SECTION_STARTUP = "startup";
const char* KEY_STARTUP_NAME =     "startup_name";
const char* KEY_STARTUP_COMMAND =  "startup_command";
const char* KEY_STARTUP_RESTART =  "startup_restart";
const char* KEY_STATIC_PAGE_TIMEOUT   = "static_page_timeout";
const char* KEY_STATIC_PAGE_URL       = "static_page_url";
const char* KEY_STATIC_PAGE_FILE_PATH = "static_page_file_path";

// web pages related
const char* CONF_SECTION_WEB_PAGES = "web_pages";
const char* KEY_COUNT =              "count";
const char* CONF_SECTION_WEB_PAGE =  "web_page_%d";
const char* KEY_PAGE =               "page";
const char* KEY_IS_STARTUP =         "is_startup";

// system related
const char* CONF_SECTION_SYSTEM =   "system";
const char* KEY_COM1_MODE =         "com1_mode";
const char* KEY_COM2_MODE =         "com2_mode";
const char* KEY_COM1_BAUDRATE =     "com1_baudrate";
const char* KEY_COM2_BAUDRATE =     "com2_baudrate";
const char* KEY_USER_LOGIN_DESKTOP = "user_login_desktop";
const char* KEY_RS_CRON_ENABLE =    "rs_cron_enable";
const char* KEY_RS_CRON_MODE =      "rs_cron_mode";
const char* KEY_RS_CRON_MINUTE =    "rs_cron_minute";
const char* KEY_RS_CRON_HOUR =      "rs_cron_hour";
const char* KEY_RS_CRON_DAYOFWEEK = "rs_cron_dayofweek";
const char* KEY_ETHERNET_ENABLE =   "ethernet_enable";
const char* KEY_USB_ENABLE =        "usb_enable";
const char* KEY_CHROMIUM_USE_SYS_VIRTUAL_KEYBOARD = "chromium_use_sys_vkb";
const char* KEY_CHROMIUM_USE_CUSTOM_VIRTUAL_KEYBOARD = "chromium_use_custom_vkb";
const char* KEY_COM_IS_SHOWED_FOR_USER = "com_is_showed_for_user";

// security related
const char* CONF_SECTION_SECURITY = "security";
const char* KEY_LOGIN_ENABLE =      "login_enable";
const char* KEY_LOGIN_PASSWORD =    "login_password";
const char* KEY_LOGIN_TYPE =        "login_type";
const char* CONF_SECTION_LOGIN =    "login_%s";
const char* KEY_ALIAS_NAME =        "alias_name";

// time related
const char* CONF_SECTION_TIME =    "time";
const char* KEY_TIMEZONE =         "timezone";
const char* KEY_NTP_ENABLE =       "ntp_enable";
const char* KEY_NTP_SERVER =       "ntp_server";
const char* KEY_DATE =             "date";
const char* KEY_HOUR =             "hour";
const char* KEY_MINUTE =           "minute";
const char* KEY_SECOND =           "second";

// VNC related
const char* CONF_SECTION_VNC =    "vnc";
const char* KEY_SERVER_ADDRESS =  "vnc_server_address";
const char* KEY_SERVER_PORT =     "vnc_server_port";
const char* KEY_SERVER_PASSWORD = "vnc_server_password";
const char* KEY_SERVER_VIEWONLY = "vnc_server_viewonly";
const char* KEY_SERVER_IMAGE_QUALITY = "vnc_server_image_quality";
const char* KEY_SERVER_FULLSCREEN = "vnc_server_fullscreen";
const char* KEY_SERVER_FIT_WINDOW = "vnc_server_fit_window";
const char* KEY_SERVER_POLLING_PERIOD = "vnc_server_polling_period";

// FTP related
const char* CONF_SECTION_FTP =        "ftp";
const char* KEY_FTP_SERVER_ADDRESS =  "ftp_server_address";
const char* KEY_FTP_SERVER_PORT =     "ftp_server_port";
const char* KEY_FTP_SERVER_USERNAME = "ftp_server_username";
const char* KEY_FTP_SERVER_PASSWORD = "ftp_server_password";
const char* KEY_FTP_SERVER_REMOTE_PATH = "ftp_server_remote_path";
const char* KEY_FTP_SERVER_LOCAL_PATH = "ftp_server_local_path";

// network related
const char* CONF_SECTION_NETWORK = "network";
const char* KEY_HAS_CONFIGURED = "has_configured";
const char* KEY_IPV4_METHOD =    "IPv4.method";
const char* KEY_IPV4_ADDRESS =   "IPv4.local_address";
const char* KEY_IPV4_GATEWAY =   "IPv4.gateway";
const char* KEY_IPV4_MASK =      "IPv4.mask";
const char* KEY_IPV6_ADDRESS =   "IPv6.local_address";
const char* KEY_NAMESERVERS =    "Nameservers";

const char* SPLIT_STRING =       ";";

// firewall rules related
const char* CONF_SECTION_FIREWALL_RULES = "firewall_rules";
const char* CONF_SECTION_FIREWALL_RULE =  "firewall_rule_%d";
const char* KEY_PROTOCOL =                PROTOCOL_STRING;
const char* KEY_PORT =                    PORT_STRING;
const char* KEY_IS_ALLOWED =              IS_ALLOWED_STRING;

// backup related
const char* CONF_SECTION_BACKUP =      "backup";
const char* KEY_BACKUP_CONFIG_ENABLE = "backup_config_enable";
const char* KEY_BACKUP_USER_ENABLE =   "backup_user_enable";

// storage related
const char* CONF_SECTION_STORAGE = "storage";
// update related
const char* CONF_SECTION_UPDATE =  "update";
// logo related
const char* CONF_SECTION_LOGO =    "logo";
// password related
const char* CONF_SECTION_PASSWORD ="password";
// operate related
const char* CONF_SECTION_OPERATE = "operate";
const char* KEY_EXPORT_SETTING_IS_SHOWED_FOR_USER = "export_setting_is_showed_for_user";
const char* KEY_EXPORT_SCREENSHOT_IS_SHOWED_FOR_USER = "export_screenshot_is_showed_for_user";
const char* KEY_IMPORT_SETTING_IS_SHOWED_FOR_USER = "import_setting_is_showed_for_user";
const char* KEY_REBOOT_IS_SHOWED_FOR_USER = "reboot_is_showed_for_user";
const char* KEY_SHUTDOWN_IS_SHOWED_FOR_USER = "shutdown_is_showed_for_user";
const char* KEY_OPEN_TERMIANL_IS_SHOWED_FOR_USER = "open_terminal_is_showed_for_user";
const char* KEY_FACTORY_RESET_IS_SHOWED_FOR_USER = "factory_reset_is_showed_for_user";
// about related
const char* CONF_SECTION_ABOUT =   "about";
// exit related
const char* CONF_SECTION_EXIT =    "exit";

ConfigUtility::ConfigUtility() : m_configFile(SETTINGS_CONFIG_FILE) {
    _generate_uuid();
}

ConfigUtility::ConfigUtility(const char* configFile) : m_configFile(configFile) {
    _generate_uuid();
}

void ConfigUtility::_generate_uuid() {
    string uuid = get_uuid();
    // generate uuid only once
    if (uuid.length() == 0) {
        uuid = QUuid().createUuid().toString().remove('{').remove('}').toStdString();
        const auto retuuid = base64_encode(uuid.c_str());
        set_uuid(retuuid.first.c_str());

        this->_clear_password_in_config();
    }
}

void ConfigUtility::_clear_password_in_config() {
    // clear password
    set_vnc_server_password("");
    set_login_password("");
}

string ConfigUtility::_get_config_value_decrypted_string(const char* section, const char* key)
{
    string encString = _get_config_value_string(section, key);
    string base64UUID = get_uuid();
    const auto retuuid = base64_decode(base64UUID.c_str());
    if (!retuuid.second || retuuid.first.length() == 0) {
        qDebug("missing uuid");
        return string();
    }
    const auto retDecstr = decrypt_text(encString.c_str(), retuuid.first.c_str());
    return retDecstr.first;
}

void ConfigUtility::_set_config_value_encrypted_string(const char* section, const char* key, const char* value)
{
    string base64UUID = get_uuid();
    const auto retuuid = base64_decode(base64UUID.c_str());
    if (!retuuid.second || retuuid.first.length() == 0) {
        qDebug("missing uuid");
        return;
    }
    const auto retEncrypted = encrypt_text(value, retuuid.first.c_str());
    _set_config_value_string(section, key, retEncrypted.first.c_str());
}

QVariant ConfigUtility::_get_config_value(const char* section, const char* key) {
    QVariant empty, variant;
    // check input
    if (!section || !key) {
        qDebug("missing parameter");
        return empty;
    }
    if (strlen(section) == 0 || strlen(key) == 0) {
        qDebug("empty section:%s key:%s", section, key);
        return empty;
    }

    QSettings settings(m_configFile.c_str(), QSettings::IniFormat);
    settings.beginGroup(section);
    variant = settings.value(key);
    settings.endGroup();
    return variant;
}

bool ConfigUtility::_get_page_is_showed(const char* section) {
    string appMode = get_app_mode();
    QVariant value = _get_config_value(section, KEY_IS_SHOWED);
    // init mode only show page which has set is_showed
    if (appMode.compare(APP_MODE_INIT) == 0) {
        if (value.toString().isEmpty())
            return false;
    } else {
        if (value.toString().isEmpty())
            return true;
    }
    return value.toBool();
}

bool ConfigUtility::_get_function_is_showed_for_user(const char* section, const char* key) {
    string appMode = get_app_mode();
    QVariant value = _get_config_value(section, key);
    if (value.toString().isEmpty())
        return true;
    return value.toBool();
}

string ConfigUtility::_get_config_value_string(const char* section, const char* key) {
    return _get_config_value(section, key).toString().toStdString();
}

void ConfigUtility::_set_config_value(const char* section, const char* key, QVariant value) {
    // check input
    if (!section || !key) {
        qDebug("missing parameter");
        return;
    }
    if (strlen(section) == 0 || strlen(key) == 0) {
        qDebug("empty section:%s key:%s", section, key);
        return;
    }

    QSettings settings(m_configFile.c_str(), QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    settings.setIniCodec(CONFIG_FILE_CODEC);
#endif
    settings.beginGroup(section);
    settings.setValue(key, value);
    settings.endGroup();
    settings.sync();
}

void ConfigUtility::_set_config_value_string(const char* section, const char* key, const char* value) {
    // check input
    if (!section || !key || !value) {
        qDebug("missing parameter");
        return;
    }
    if (strlen(section) == 0 || strlen(key) == 0) {
        qDebug("empty section:%s key:%s", section, key);
        return;
    }

    _set_config_value(section, key, QVariant(value));
}

// UI related
string ConfigUtility::get_ui_theme() {
    QSettings settings(SETTINGS_CONFIG_FILE, QSettings::IniFormat);
    settings.beginGroup(CONF_SECTION_UI);
    QVariant variant = settings.value(KEY_THEME);
    settings.endGroup();
    return variant.toString().toStdString();
}

string ConfigUtility::get_keyboard_locale() {
    return _get_config_value_string(CONF_SECTION_UI, KEY_KEYBOARD_LOCALE);
}

// core related
string ConfigUtility::get_uuid() {
    return _get_config_value_string(CONF_SECTION_CORE, KEY_UUID_NAME);
}

void ConfigUtility::set_uuid(const char* uuid) {
    _set_config_value_string(CONF_SECTION_CORE, KEY_UUID_NAME, uuid);
}

string ConfigUtility::get_app_mode() {
    return _get_config_value_string(CONF_SECTION_CORE, KEY_APP_MODE);
}

string ConfigUtility::get_next_page_by_index(int index) {
    char buff[BUFF_SIZE] = {0};
    snprintf(buff, BUFF_SIZE, KEY_PAGE_INDEX_PATTERN, index);
    return _get_config_value_string(CONF_SECTION_CORE, buff);
}

// credentials related
bool ConfigUtility::get_credentials_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_CREDENTIALS);
}

bool ConfigUtility::get_root_password_required() {
    return _get_config_value(CONF_SECTION_CREDENTIALS, KEY_ROOT_PASSWORD_REQUIRED).toBool();
}

bool ConfigUtility::get_weston_password_required() {
    return _get_config_value(CONF_SECTION_CREDENTIALS, KEY_WESTON_PASSWORD_REQUIRED).toBool();
}

// wizard related
bool ConfigUtility::get_wizard_network_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_WIZARD_NETWORK);
}

bool ConfigUtility::get_wizard_time_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_WIZARD_TIME);
}

bool ConfigUtility::get_wizard_screen_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_WIZARD_SCREEN);
}

bool ConfigUtility::get_wizard_startup_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_WIZARD_STARTUP);
}

// screen related
bool ConfigUtility::get_screen_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_SCREEN);
}

int ConfigUtility::get_brightness() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_BRIGHTNESS).toInt();
}

bool ConfigUtility::get_screensaver_enable() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_SCREENSAVER_ENABLE).toBool();
}

int ConfigUtility::get_blank_after() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_BLANK_AFTER_PERIOD).toInt();
}

bool ConfigUtility::get_hide_cursor_enable() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_HIDE_CURSOR_ENABLE).toBool();
}

string ConfigUtility::get_top_bar_position() {
    return _get_config_value_string(CONF_SECTION_SCREEN, KEY_TOP_BAR_POSITION);
}

string ConfigUtility::get_rotate_screen() {
    return _get_config_value_string(CONF_SECTION_SCREEN, KEY_ROTATE_SCREEN);
}

string ConfigUtility::get_gesture_enable_string() {
    return _get_config_value_string(CONF_SECTION_SCREEN, KEY_GESTURE_ENABLE);
}

bool ConfigUtility::get_gesture_enable() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_GESTURE_ENABLE).toBool();
}

bool ConfigUtility::get_gesture_swipe_down_enable() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_GESTURE_SWIPE_DOWN_ENABLE).toBool();
}

bool ConfigUtility::get_gesture_swipe_up_enable() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_GESTURE_SWIPE_UP_ENABLE).toBool();
}

bool ConfigUtility::get_gesture_swipe_right_enable() {
    return _get_config_value(CONF_SECTION_SCREEN, KEY_GESTURE_SWIPE_RIGHT_ENABLE).toBool();
}

void ConfigUtility::set_brightness(int brightness) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_BRIGHTNESS, std::to_string(brightness).c_str());
}

void ConfigUtility::set_screensaver_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_SCREENSAVER_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_blank_after(int blankPeriod) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_BLANK_AFTER_PERIOD, std::to_string(blankPeriod).c_str());
}

void ConfigUtility::set_hide_cursor_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_HIDE_CURSOR_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_top_bar_position(const char* position) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_TOP_BAR_POSITION, position);
}

void ConfigUtility::set_rotate_screen(const char* rotateDegree) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_ROTATE_SCREEN, rotateDegree);
}

void ConfigUtility::set_gesture_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_GESTURE_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_gesture_swipe_down_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_GESTURE_SWIPE_DOWN_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_gesture_swipe_up_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_GESTURE_SWIPE_UP_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_gesture_swipe_right_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SCREEN, KEY_GESTURE_SWIPE_RIGHT_ENABLE, bool_cast(enabled));
}

// startup related
string ConfigUtility::get_startup() {
    return _get_config_value_string(CONF_SECTION_STARTUP, KEY_STARTUP_NAME);
}

int ConfigUtility::get_static_page_timeout() {
    return _get_config_value(CONF_SECTION_STARTUP, KEY_STATIC_PAGE_TIMEOUT).toInt();
}

string ConfigUtility::get_static_page_url() {
    return _get_config_value_string(CONF_SECTION_STARTUP, KEY_STATIC_PAGE_URL);
}

string ConfigUtility::get_static_page_file_path() {
    return _get_config_value_string(CONF_SECTION_STARTUP, KEY_STATIC_PAGE_FILE_PATH);
}

bool ConfigUtility::get_startup_auto_restart() {
    return _get_config_value(CONF_SECTION_STARTUP, KEY_STARTUP_RESTART).toBool();;
}

void ConfigUtility::set_startup(const char* name) {
    // check input
    if (!name || strlen(name) == 0) {
        qDebug("missing parameter");
        return;
    }
    StartupApplication* app;
    if (strcmp(name, STARTUP_NAME_SETTINGS) == 0)
        app = new StartupSettings();
    else if (strcmp(name, STARTUP_NAME_CHROMIUM) == 0)
        app = new StartupChromium();
    else if (strcmp(name, STARTUP_NAME_CHROMIUM_KIOSK) == 0)
        app = new StartupKioskChromium();
    else if (strcmp(name, STARTUP_NAME_STATIC_PAGE) == 0)
        app = new StartupStaticPage();
    else if (strcmp(name, STARTUP_NAME_STATIC_PAGE_CUSTOM) == 0)
        app = new StartupStaticPageCustom();
    else if (strcmp(name, STARTUP_NAME_VNC_VIEWER) == 0)
        app = new StartupVNC();
    else
        app = new StartupNone();
    _set_config_value_string(CONF_SECTION_STARTUP, KEY_STARTUP_NAME, app->get_startup_name().c_str());
    _set_config_value_string(CONF_SECTION_STARTUP, KEY_STARTUP_COMMAND, app->get_startup_command().c_str());
    delete app;
}

void ConfigUtility::set_static_page_timeout(int timeout) {
    _set_config_value_string(CONF_SECTION_STARTUP, KEY_STATIC_PAGE_TIMEOUT, std::to_string(timeout).c_str());
}

void ConfigUtility::set_static_page_url(const char* url) {
    _set_config_value_string(CONF_SECTION_STARTUP, KEY_STATIC_PAGE_URL, url);
}

void ConfigUtility::set_static_page_file_path(const char* filePath) {
    _set_config_value_string(CONF_SECTION_STARTUP, KEY_STATIC_PAGE_FILE_PATH, filePath);
}

void ConfigUtility::set_startup_auto_restart(bool isAutoRestart) {
    _set_config_value_string(CONF_SECTION_STARTUP, KEY_STARTUP_RESTART, bool_cast(isAutoRestart));
}

// web pages related
int ConfigUtility::get_web_pages_count() {
    return _get_config_value(CONF_SECTION_WEB_PAGES, KEY_COUNT).toInt();
}

map<string, string> ConfigUtility::get_web_page_by_index(int index) {
    map<string, string> result;
    char section[BUFF_SIZE] = {0};
    snprintf(section, BUFF_SIZE, CONF_SECTION_WEB_PAGE, index);
    result[KEY_PAGE] = _get_config_value_string(section, KEY_PAGE);
    result[KEY_IS_STARTUP] = _get_config_value_string(section, KEY_IS_STARTUP);
    return result;
}

void ConfigUtility::set_web_pages_count(int count) {
    _set_config_value_string(CONF_SECTION_WEB_PAGES, KEY_COUNT, std::to_string(count).c_str());
}

void ConfigUtility::set_web_page_by_index(map<string, string> webPage, int index) {
    char section[BUFF_SIZE] = {0};
    snprintf(section, BUFF_SIZE, CONF_SECTION_WEB_PAGE, index);
    if (webPage.count(KEY_PAGE) > 0)
        _set_config_value_string(section, KEY_PAGE, webPage[KEY_PAGE].c_str());
    if (webPage.count(KEY_IS_STARTUP) > 0)
        _set_config_value_string(section, KEY_IS_STARTUP, webPage[KEY_IS_STARTUP].c_str());
}

// system related
bool ConfigUtility::get_system_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_SYSTEM);
}

string ConfigUtility::get_com1_mode() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_COM1_MODE);
}

string ConfigUtility::get_com2_mode() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_COM2_MODE);
}

string ConfigUtility::get_com1_baudrate() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_COM1_BAUDRATE);
}

string ConfigUtility::get_com2_baudrate() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_COM2_BAUDRATE);
}

string ConfigUtility::get_system_user_login_desktop_string() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_USER_LOGIN_DESKTOP);
}

bool ConfigUtility::get_system_user_login_desktop() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_USER_LOGIN_DESKTOP).toBool();
}

bool ConfigUtility::get_reboot_system_crontab_enabled() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_RS_CRON_ENABLE).toBool();
}

string ConfigUtility::get_reboot_system_crontab_mode() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_RS_CRON_MODE);
}

int ConfigUtility::get_reboot_system_crontab_minute() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_RS_CRON_MINUTE).toInt();
}

int ConfigUtility::get_reboot_system_crontab_hour() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_RS_CRON_HOUR).toInt();
}

int ConfigUtility::get_reboot_system_crontab_dayofweek() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_RS_CRON_DAYOFWEEK).toInt();
}

string ConfigUtility::get_ethernet_enable_string() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_ETHERNET_ENABLE);
}

string ConfigUtility::get_usb_enable_string() {
    return _get_config_value_string(CONF_SECTION_SYSTEM, KEY_USB_ENABLE);
}

bool ConfigUtility::get_ethernet_enable() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_ETHERNET_ENABLE).toBool();
}

bool ConfigUtility::get_usb_enable() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_USB_ENABLE).toBool();
}

bool ConfigUtility::get_chromium_use_sys_virtual_keyboard() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_CHROMIUM_USE_SYS_VIRTUAL_KEYBOARD).toBool();
}

bool ConfigUtility::get_chromium_use_custom_virtual_keyboard() {
    return _get_config_value(CONF_SECTION_SYSTEM, KEY_CHROMIUM_USE_CUSTOM_VIRTUAL_KEYBOARD).toBool();
}

void ConfigUtility::set_com1_mode(const char* mode) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_COM1_MODE, mode);
}

void ConfigUtility::set_com2_mode(const char* mode) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_COM2_MODE, mode);
}

void ConfigUtility::set_com1_baudrate(const char* baudrate) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_COM1_BAUDRATE, baudrate);
}

void ConfigUtility::set_com2_baudrate(const char* baudrate) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_COM2_BAUDRATE, baudrate);
}

void ConfigUtility::set_system_user_login_desktop(bool isUserLogin) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_USER_LOGIN_DESKTOP, bool_cast(isUserLogin));
}

void ConfigUtility::set_reboot_system_crontab_enabled(bool enabled) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_RS_CRON_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_reboot_system_crontab_mode(const char* mode) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_RS_CRON_MODE, mode);
}

void ConfigUtility::set_reboot_system_crontab_minute(int minute) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_RS_CRON_MINUTE, std::to_string(minute).c_str());
}

void ConfigUtility::set_reboot_system_crontab_hour(int hour) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_RS_CRON_HOUR, std::to_string(hour).c_str());
}

void ConfigUtility::set_reboot_system_crontab_dayofweek(int dayofweek) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_RS_CRON_DAYOFWEEK, std::to_string(dayofweek).c_str());
}

void ConfigUtility::set_ethernet_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_ETHERNET_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_usb_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_USB_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_chromium_use_sys_virtual_keyboard(bool enabled) {
    _set_config_value_string(CONF_SECTION_SYSTEM, KEY_CHROMIUM_USE_SYS_VIRTUAL_KEYBOARD, bool_cast(enabled));
}

bool ConfigUtility::get_com_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_SYSTEM, KEY_COM_IS_SHOWED_FOR_USER);
}

// security related
bool ConfigUtility::get_security_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_SECURITY);
}

bool ConfigUtility::get_login_enable() {
    return _get_config_value(CONF_SECTION_SECURITY, KEY_LOGIN_ENABLE).toBool();
}

string ConfigUtility::get_login_password() {
    return _get_config_value_decrypted_string(CONF_SECTION_SECURITY, KEY_LOGIN_PASSWORD);
}

string ConfigUtility::get_login_type() {
    return _get_config_value_string(CONF_SECTION_SECURITY, KEY_LOGIN_TYPE);
}

void ConfigUtility::set_login_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_SECURITY, KEY_LOGIN_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_login_password(const char* password) {
    _set_config_value_encrypted_string(CONF_SECTION_SECURITY, KEY_LOGIN_PASSWORD, password);
}

void ConfigUtility::set_login_type(const char* type) {
    _set_config_value_string(CONF_SECTION_SECURITY, KEY_LOGIN_TYPE, type);
}

string ConfigUtility::get_login_name_alias(const char* username) {
    char section[BUFF_SIZE] = {0};
    snprintf(section, BUFF_SIZE, CONF_SECTION_LOGIN, username);
    return _get_config_value_string(section, KEY_ALIAS_NAME);
}

// time related
bool ConfigUtility::get_time_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_TIME);
}

string ConfigUtility::get_timezone() {
    return _get_config_value_string(CONF_SECTION_TIME, KEY_TIMEZONE);
}

bool ConfigUtility::get_ntp_enable() {
    return _get_config_value(CONF_SECTION_TIME, KEY_NTP_ENABLE).toBool();
}

string ConfigUtility::get_ntp_server() {
    return _get_config_value_string(CONF_SECTION_TIME, KEY_NTP_SERVER);
}

string ConfigUtility::get_date() {
    return _get_config_value_string(CONF_SECTION_TIME, KEY_DATE);
}

int ConfigUtility::get_hour() {
    return _get_config_value(CONF_SECTION_TIME, KEY_HOUR).toInt();
}

int ConfigUtility::get_minute() {
    return _get_config_value(CONF_SECTION_TIME, KEY_MINUTE).toInt();
}

int ConfigUtility::get_second() {
    return _get_config_value(CONF_SECTION_TIME, KEY_SECOND).toInt();
}

void ConfigUtility::set_timezone(const char* timezone) {
    _set_config_value_string(CONF_SECTION_TIME, KEY_TIMEZONE, timezone);
}

void ConfigUtility::set_ntp_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_TIME, KEY_NTP_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_ntp_server(const char* server) {
    _set_config_value_string(CONF_SECTION_TIME, KEY_NTP_SERVER, server);
}

void ConfigUtility::set_date(const char* date) {
    _set_config_value_string(CONF_SECTION_TIME, KEY_DATE, date);
}

void ConfigUtility::set_hour(int hour) {
    _set_config_value_string(CONF_SECTION_TIME, KEY_HOUR, std::to_string(hour).c_str());
}

void ConfigUtility::set_minute(int minute) {
    _set_config_value_string(CONF_SECTION_TIME, KEY_MINUTE, std::to_string(minute).c_str());
}

void ConfigUtility::set_second(int second) {
    _set_config_value_string(CONF_SECTION_TIME, KEY_SECOND, std::to_string(second).c_str());
}

// VNC related
string ConfigUtility::get_vnc_server_address() {
    return _get_config_value_string(CONF_SECTION_VNC, KEY_SERVER_ADDRESS);
}

string ConfigUtility::get_vnc_server_port() {
    return _get_config_value_string(CONF_SECTION_VNC, KEY_SERVER_PORT);
}

string ConfigUtility::get_vnc_server_password() {
    return _get_config_value_decrypted_string(CONF_SECTION_VNC, KEY_SERVER_PASSWORD);
}

bool ConfigUtility::get_vnc_server_viewonly() {
    return _get_config_value(CONF_SECTION_VNC, KEY_SERVER_VIEWONLY).toBool();
}

int ConfigUtility::get_vnc_server_image_quality() {
    return _get_config_value(CONF_SECTION_VNC, KEY_SERVER_IMAGE_QUALITY).toInt();
}

bool ConfigUtility::get_vnc_server_fullscreen() {
    return _get_config_value(CONF_SECTION_VNC, KEY_SERVER_FULLSCREEN).toBool();
}

bool ConfigUtility::get_vnc_server_fit_window() {
    return _get_config_value(CONF_SECTION_VNC, KEY_SERVER_FIT_WINDOW).toBool();
}

int ConfigUtility::get_vnc_server_polling_period() {
    return _get_config_value(CONF_SECTION_VNC, KEY_SERVER_POLLING_PERIOD).toInt();
}

void ConfigUtility::set_vnc_server_address(const char* server) {
    _set_config_value_string(CONF_SECTION_VNC, KEY_SERVER_ADDRESS, server);
}

void ConfigUtility::set_vnc_server_port(const char* port) {
    _set_config_value_string(CONF_SECTION_VNC, KEY_SERVER_PORT, port);
}

void ConfigUtility::set_vnc_server_password(const char* password) {
    _set_config_value_encrypted_string(CONF_SECTION_VNC, KEY_SERVER_PASSWORD, password);
}

void ConfigUtility::set_vnc_server_viewonly(bool viewonly) {
    _set_config_value_string(CONF_SECTION_VNC, KEY_SERVER_VIEWONLY, bool_cast(viewonly));
}

void ConfigUtility::set_vnc_server_image_quality(int imageQuality) {
    _set_config_value_string(CONF_SECTION_VNC, KEY_SERVER_IMAGE_QUALITY, std::to_string(imageQuality).c_str());
}

void ConfigUtility::set_vnc_server_fullscreen(bool fullscreen) {
    _set_config_value_string(CONF_SECTION_VNC, KEY_SERVER_FULLSCREEN, bool_cast(fullscreen));
}

void ConfigUtility::set_vnc_server_fit_window(bool fitWindow) {
    _set_config_value_string(CONF_SECTION_VNC, KEY_SERVER_FIT_WINDOW, bool_cast(fitWindow));
}

void ConfigUtility::set_vnc_server_polling_period(int pollingPeriod) {
    _set_config_value_string(CONF_SECTION_VNC, KEY_SERVER_POLLING_PERIOD, std::to_string(pollingPeriod).c_str());
}

// network related
bool ConfigUtility::get_network_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_NETWORK);
}

bool ConfigUtility::get_net_has_configured(const char* ethernet) {
    return _get_config_value(ethernet, KEY_HAS_CONFIGURED).toBool();
}

string ConfigUtility::get_net_method(const char* ethernet) {
    return _get_config_value_string(ethernet, KEY_IPV4_METHOD);
}

string ConfigUtility::get_net_ip_address(const char* ethernet) {
    return _get_config_value_string(ethernet, KEY_IPV4_ADDRESS);
}

string ConfigUtility::get_net_gateway(const char* ethernet) {
    return _get_config_value_string(ethernet, KEY_IPV4_GATEWAY);
}

string ConfigUtility::get_net_subnet_mask(const char* ethernet) {
    return _get_config_value_string(ethernet, KEY_IPV4_MASK);
}

vector<string> ConfigUtility::get_net_dns_servers(const char* ethernet) {
    string nameservers = _get_config_value_string(ethernet, KEY_NAMESERVERS);
    return split_string_to_vector(nameservers.c_str(), SPLIT_STRING);
}

void ConfigUtility::set_net_has_configured(const char* ethernet, bool hasConfigured) {
    _set_config_value(ethernet, KEY_HAS_CONFIGURED, QVariant(hasConfigured));
}

void ConfigUtility::set_net_dhcp(const char* ethernet) {
    _set_config_value_string(ethernet, KEY_IPV4_METHOD, MODE_DHCP);
}

void ConfigUtility::set_net_static_ip(const char* ethernet, const char* ipv4, const char* ipv6, const char* subnetMask, const char* gateway) {
    // check input
    if (!ethernet || (!ipv4 && !ipv6) || !subnetMask || !gateway) {
        qDebug("missing parameter");
        return;
    }
    if (strlen(ethernet) == 0) {
        qDebug("empty ethernet");
        return;
    }

    QSettings settings(m_configFile.c_str(), QSettings::IniFormat);
#if (QT_VERSION < QT_VERSION_CHECK(6,0,0))
    settings.setIniCodec(CONFIG_FILE_CODEC);
#endif
    settings.beginGroup(ethernet);
    settings.setValue(KEY_IPV4_METHOD, MODE_MANUAL);
    settings.setValue(KEY_IPV4_ADDRESS, ipv4);
    settings.setValue(KEY_IPV4_GATEWAY, gateway);
    settings.setValue(KEY_IPV4_MASK, subnetMask);
    if (ipv6 && strlen(ipv6) > 0)
        settings.setValue(KEY_IPV6_ADDRESS, ipv6);
    settings.endGroup();
    settings.sync();
}

void ConfigUtility::set_net_dns_server(const char* ethernet, const char* dns1, const char* dns2) {
    string nameservers;
    // check input
    if (!ethernet || (!dns1 && !dns2)) {
        qDebug("missing parameter");
        return;
    }
    if (strlen(ethernet) == 0) {
        qDebug("empty ethernet");
        return;
    }

    if (dns1 && strlen(dns1) > 0) {
        nameservers.append(dns1);
        nameservers.append(SPLIT_STRING);
    }
    if (dns2 && strlen(dns2) > 0) {
        nameservers.append(dns2);
        nameservers.append(SPLIT_STRING);
    }
    _set_config_value_string(ethernet, KEY_NAMESERVERS, nameservers.c_str());
}

// firewall rules related
int ConfigUtility::get_firewall_rules_count() {
    return _get_config_value(CONF_SECTION_FIREWALL_RULES, KEY_COUNT).toInt();
}

map<string, string> ConfigUtility::get_firewall_rule_by_index(int index) {
    map<string, string> result;
    char section[BUFF_SIZE] = {0};
    snprintf(section, BUFF_SIZE, CONF_SECTION_FIREWALL_RULE, index);
    result[KEY_PROTOCOL] = _get_config_value_string(section, KEY_PROTOCOL);
    result[KEY_PORT] = _get_config_value_string(section, KEY_PORT);
    result[KEY_IS_ALLOWED] = _get_config_value_string(section, KEY_IS_ALLOWED);
    return result;
}

void ConfigUtility::set_firewall_rules_count(int count) {
    _set_config_value_string(CONF_SECTION_FIREWALL_RULES, KEY_COUNT, std::to_string(count).c_str());
}

void ConfigUtility::set_firewall_rule_by_index(map<string, string> rule, int index) {
    char section[BUFF_SIZE] = {0};
    snprintf(section, BUFF_SIZE, CONF_SECTION_FIREWALL_RULE, index);
    if (rule.count(KEY_PROTOCOL) > 0)
        _set_config_value_string(section, KEY_PROTOCOL, rule[KEY_PROTOCOL].c_str());
    if (rule.count(KEY_PORT) > 0)
        _set_config_value_string(section, KEY_PORT, rule[KEY_PORT].c_str());
    if (rule.count(KEY_IS_ALLOWED) > 0)
        _set_config_value_string(section, KEY_IS_ALLOWED, rule[KEY_IS_ALLOWED].c_str());
}

// FTP related
bool ConfigUtility::get_ftp_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_FTP);
}

string ConfigUtility::get_ftp_server_address() {
    return _get_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_ADDRESS);
}

string ConfigUtility::get_ftp_server_port() {
    return _get_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_PORT);
}

string ConfigUtility::get_ftp_server_username() {
    return _get_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_USERNAME);
}

string ConfigUtility::get_ftp_server_password() {
    return _get_config_value_decrypted_string(CONF_SECTION_FTP, KEY_FTP_SERVER_PASSWORD);
}

string ConfigUtility::get_ftp_server_remote_path() {
    return _get_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_REMOTE_PATH);
}

string ConfigUtility::get_ftp_server_local_path() {
    return _get_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_LOCAL_PATH);
}

void ConfigUtility::set_ftp_server_address(const char* server) {
    _set_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_ADDRESS, server);
}

void ConfigUtility::set_ftp_server_port(const char* port) {
    _set_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_PORT, port);
}

void ConfigUtility::set_ftp_server_username(const char* username) {
    _set_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_USERNAME, username);
}

void ConfigUtility::set_ftp_server_password(const char* password) {
    _set_config_value_encrypted_string(CONF_SECTION_FTP, KEY_FTP_SERVER_PASSWORD, password);
}

void ConfigUtility::set_ftp_server_remote_path(const char* remotePath) {
    _set_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_REMOTE_PATH, remotePath);
}

void ConfigUtility::set_ftp_server_local_path(const char* localPath) {
    _set_config_value_string(CONF_SECTION_FTP, KEY_FTP_SERVER_LOCAL_PATH, localPath);
}

// backup related
const char* ConfigUtility::get_config_path() {
    return SETTINGS_CONFIG_FILE;
}

const char* ConfigUtility::get_backup_config_path() {
    return BACKUP_CONFIG_FILE;
}

const char* ConfigUtility::get_user_config_path() {
    return USER_CONFIG_FILE;
}

const char* ConfigUtility::get_backup_user_config_path() {
    return BACKUP_USER_CONFIG_FILE;
}

const char* ConfigUtility::get_export_config_filename() {
    return EXPORT_CONFIG_FILENAME;
}

bool ConfigUtility::get_backup_config_enable() {
    return _get_config_value(CONF_SECTION_BACKUP, KEY_BACKUP_CONFIG_ENABLE).toBool();
}

bool ConfigUtility::get_backup_user_enable() {
    return _get_config_value(CONF_SECTION_BACKUP, KEY_BACKUP_USER_ENABLE).toBool();
}

void ConfigUtility::set_backup_config_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_BACKUP, KEY_BACKUP_CONFIG_ENABLE, bool_cast(enabled));
}

void ConfigUtility::set_backup_user_enable(bool enabled) {
    _set_config_value_string(CONF_SECTION_BACKUP, KEY_BACKUP_USER_ENABLE, bool_cast(enabled));
}

bool ConfigUtility::_backup_config(const char* source, const char* target) {
    return cp_file(source, target);
}

bool ConfigUtility::_restore_config(const char* source, const char* target) {
    return mv_file(source, target);
}

bool ConfigUtility::backup_config() {
    return _backup_config(m_configFile.c_str(), get_backup_config_path());
}

bool ConfigUtility::backup_config_to_custom_path(const char* targetPath) {
    return _backup_config(m_configFile.c_str(), targetPath);
}

bool ConfigUtility::restore_config_from_backup() {
    return _restore_config(get_backup_config_path(), get_config_path());
}

bool ConfigUtility::restore_config_from_custom_path(const char* sourcePath) {
    return _restore_config(sourcePath, get_config_path());
}

bool ConfigUtility::backup_user_config() {
    return _backup_config(get_user_config_path(), get_backup_user_config_path());
}

bool ConfigUtility::backup_user_config_to_custom_path(const char* targetPath) {
    return _backup_config(get_user_config_path(), targetPath);
}

bool ConfigUtility::restore_user_config_from_backup() {
    bool result = sys_restore_system_user(get_backup_user_config_path(), get_user_config_path(), ROOT_USER);
    result &= sys_restore_system_user(get_backup_user_config_path(), get_user_config_path(), WESTON_USER);
    if (result)
        remove(get_backup_user_config_path());
    return result;
}

bool ConfigUtility::restore_user_config_from_custom_path(const char* sourcePath) {
    return sys_restore_system_user(sourcePath, get_user_config_path(), ROOT_USER);
}

// storage related
bool ConfigUtility::get_storage_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_STORAGE);
}
// update related
bool ConfigUtility::get_update_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_UPDATE);
}
// logo related
bool ConfigUtility::get_logo_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_LOGO);
}
// password related
bool ConfigUtility::get_password_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_PASSWORD);
}
// operate related
bool ConfigUtility::get_operate_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_OPERATE);
}
bool ConfigUtility::get_export_setting_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_OPERATE, KEY_EXPORT_SETTING_IS_SHOWED_FOR_USER);
}
bool ConfigUtility::get_export_screenshot_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_OPERATE, KEY_EXPORT_SCREENSHOT_IS_SHOWED_FOR_USER);
}
bool ConfigUtility::get_import_setting_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_OPERATE, KEY_IMPORT_SETTING_IS_SHOWED_FOR_USER);
}
bool ConfigUtility::get_reboot_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_OPERATE, KEY_REBOOT_IS_SHOWED_FOR_USER);
}
bool ConfigUtility::get_shutdown_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_OPERATE, KEY_SHUTDOWN_IS_SHOWED_FOR_USER);
}
bool ConfigUtility::get_open_terminal_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_OPERATE, KEY_OPEN_TERMIANL_IS_SHOWED_FOR_USER);
}
bool ConfigUtility::get_factory_reset_function_is_showed_for_user() {
    return _get_function_is_showed_for_user(CONF_SECTION_OPERATE, KEY_FACTORY_RESET_IS_SHOWED_FOR_USER);
}

// about related
bool ConfigUtility::get_about_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_ABOUT);
}

// exit related
bool ConfigUtility::get_exit_page_is_showed() {
    return _get_page_is_showed(CONF_SECTION_EXIT);
}

// common functions
bool ConfigUtility::get_page_is_showed_by_name(const char* name) {
    return _get_page_is_showed(name);
}

string ConfigUtility::get_next_page_by_name(const char* name) {
    return _get_config_value_string(name, KEY_NEXT_PAGE);
}
