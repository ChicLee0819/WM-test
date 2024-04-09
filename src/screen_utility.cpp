// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#ifdef _WIN32
#else
#include <unistd.h>
#endif
#include <QDebug>

#include "./include/utility.h"
#include "./include/screen_utility.h"

#define STRING_TOP  "top"
#define STRING_NONE "none"

const char *WESTON_CONFIG_FILE = "/etc/xdg/weston/weston.ini";
// core related
const char *WESTON_SECTION_CORE = "core";
const char *KEY_IDLE_TIME = "idle-time";
// libinput related
const char *WESTON_SECTION_LIBINPUT = "libinput";
const char *KEY_TOUCH_CALIBRATOR = "touchscreen_calibrator";
const char *KEY_HIDE_CURSOR = "hide-cursor";
// shell related
const char *WESTON_SECTION_SHELL = "shell";
// sets the position of the panel (string). Can be top, bottom, left, right, none.
const char *KEY_PANEL_POSITION = "panel-position";
const char *INSERT_SHELL_SETTING_TMPLATE="\n[%s]\n%s=%s\n";
// output related
const char *WESTON_SECTION_OUTPUT = "output";
const char *KEY_OUTPUT_NAME = "name";
const char *KEY_TRANSFORM = "transform";
const char *IMX_DISPLAY_NAME = "DSI-1";
const char *INSERT_OUTPUT_SETTING_TMPLATE="\n[%s]\n%s=%s\n%s=%s\n";

const char* GET_STRING_CMD = "cat %s | grep %s=%s";

const char *SET_INI_INT_VALUE_CMD = "sed -i '/^%s=/s/=.*/=%d/' %s";
const char *SET_INI_STRING_VALUE_CMD = "sed -i '/^%s=/s/=.*/=%s/' %s";
// save the value in a variable and then print it after processing the whole file
const char *GET_INI_VALUE_CMD = "awk -F '=' '/^%s/ {val=$2} END{print val}' %s | tr -d '\\n'";
// insert key/value after math property key (argument 1.match key 2.insert key 3.insert value 4.file)
const char *INSERT_INI_KEY_VALUE_CMD = "sed -i -r '/^[#]*\\s*%s=.*/a %s=%s' %s";
// insert section and key/value at file end
const char *INSERT_INI_SECTION_KEY_VALUE_CMD = "echo -en '%s' >>%s";

const char *RESTART_WESTON_CMD = "/usr/bin/start_weston.sh restart";

const char *DISPLAY_DEVICE_0 = "lvds_backlight@0";
const char *DISPLAY_DEVICE_TMPLATE = "/sys/class/backlight/%s/brightness";
// ex: cat /sys/class/backlight/lvds_backlight@0/brightness
const char *GET_BRIGHTNESS_CMD = "cat /sys/class/backlight/%s/brightness";
const char *SET_BRIGHTNESS_CMD = "echo %d >> /sys/class/backlight/%s/brightness";
const char *SAVE_BRIGHTNESS_CMD = "/lib/systemd/systemd-backlight save backlight:lvds_backlight@0; sync";

const char *GET_GESTURE_SERVICE_ENABLED_CMD = "systemctl is-enabled gester | grep enabled";
const char *OPERATE_GESTURE_SERVICE_CMD = "systemctl %s gester 2>&1";

const char *SCREENSHOTS_FOLDER = "/userdata";
const char *COUNT_SCREENSHOTS_CMD = "ls /userdata/wayland-screenshot-* 2>/dev/null | wc -l";
const char *COPY_SCREENSHOTS_CMD = "cp /userdata/wayland-screenshot-* %s 2>&1";
const char *RM_SCREENSHOTS_CMD = "rm -rf /userdata/wayland-screenshot-* 2>&1";

const char *GESTURE_CONFIG_FILE = "/etc/gester/gester.conf";
const char *GESTURE_SECTION = "gesture";
const char *KEY_GESTURE_TYPE = "type";
const char *KEY_GESTURE_LIST = "gesture_list";
const char *TWO_FINGER_GESTURE_SECTION = "2_finger_gesture";
const char *THREE_FINGER_GESTURE_SECTION = "3_finger_gesture";
const char *FOUR_FINGER_GESTURE_SECTION = "4_finger_gesture";
const char *FIVE_FINGER_GESTURE_SECTION = "5_finger_gesture";
const char *KEY_GESTURE_SWIPE_DOWN = "swipe_down";
const char *KEY_GESTURE_SWIPE_UP = "swipe_up";
const char *KEY_GESTURE_SWIPE_RIGHT = "swipe_right";
const char *KEY_GESTURE_SWIPE_LEFT = "swipe_left";
const char *GESTURE_ACTION_SECTION = "gesture_action";
const char *KEY_GESTURE_ACTION_LIST = "action_list";
const char *ACTION_OPEN_SETTINGS = "action_open_settings";
const char *ACTION_OPEN_CHROMIUM = "action_open_chromium";
const char *ACTION_CLOSE_WINDOW = "action_close_active_window";
const char *ACTION_SWITCH_WINDOW = "action_switch_active_window";
const char *ACTION_TAKE_SCREENSHOT = "action_take_screenshot";

const char *DELIMITER_STRING = " ";

using namespace std;

TPCScreenUtility::TPCScreenUtility()
{
    char value_buff[BUFF_SIZE]= {0};
    snprintf(value_buff, BUFF_SIZE, "%s.%s", TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_UP);
    m_gestureActionMap[value_buff] = ACTION_CLOSE_WINDOW;
    memset(value_buff, 0, BUFF_SIZE);
    snprintf(value_buff, BUFF_SIZE, "%s.%s", TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_DOWN);
    m_gestureActionMap[value_buff] = ACTION_OPEN_SETTINGS;
    memset(value_buff, 0, BUFF_SIZE);
    snprintf(value_buff, BUFF_SIZE, "%s.%s", TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_RIGHT);
    m_gestureActionMap[value_buff] = ACTION_SWITCH_WINDOW;
    memset(value_buff, 0, BUFF_SIZE);
    snprintf(value_buff, BUFF_SIZE, "%s.%s", FOUR_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_DOWN);
    m_gestureActionMap[value_buff] = ACTION_TAKE_SCREENSHOT;
}

bool TPCScreenUtility::_is_display_device_exist(const char *device)
{
    char path_buff[CMD_SIZE] = {0};
    snprintf(path_buff, CMD_SIZE, DISPLAY_DEVICE_TMPLATE, device);
    return is_file_exist(path_buff);
}

int TPCScreenUtility::get_brightness()
{
    int brightness = 0;
    if (!_is_display_device_exist(DISPLAY_DEVICE_0))
        return brightness;
    // execute command to get brightness
    const auto ret = execute_cmd_get_single_info(GET_BRIGHTNESS_CMD, DISPLAY_DEVICE_0);
    if (ret.first.empty())
        return brightness;
    brightness = std::stoi(ret.first);
    return brightness;
}

bool TPCScreenUtility::set_brightness(const int brightness)
{
    bool result = false;
    if (!_is_display_device_exist(DISPLAY_DEVICE_0))
        return result;
    if (brightness < SCREEN_BRIGHTNESS_MIN || brightness > SCREEN_BRIGHTNESS_MAX) {
        qDebug("brightness overflow:%d", brightness);
        return result;
    }
    result = execute_cmd_set_info(SET_BRIGHTNESS_CMD, brightness, DISPLAY_DEVICE_0);
    result &= execute_cmd_set_info(SAVE_BRIGHTNESS_CMD);
    return result;
}

int TPCScreenUtility::get_screensaver_idle_time()
{
    int idleTime = 0;
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return idleTime;
    const auto ret = execute_cmd_get_single_info(GET_INI_VALUE_CMD, KEY_IDLE_TIME, WESTON_CONFIG_FILE);
    if (ret.first.empty())
        return idleTime;
    idleTime = std::stoi(ret.first);
    return idleTime;
}

bool TPCScreenUtility::set_screensaver_idle_time(const int seconds)
{
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return false;

    return execute_cmd_set_info(SET_INI_INT_VALUE_CMD, KEY_IDLE_TIME, seconds, WESTON_CONFIG_FILE);
}

bool TPCScreenUtility::get_hide_cursor()
{
    bool hideCursor = false;
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return hideCursor;
    const auto ret = execute_cmd_get_single_info(GET_INI_VALUE_CMD, KEY_HIDE_CURSOR, WESTON_CONFIG_FILE);
    hideCursor = (ret.first.compare(0, strlen(STRING_BOOL_TRUE), STRING_BOOL_TRUE) == 0);
    return hideCursor;
}

bool TPCScreenUtility::set_hide_cursor(const bool hide)
{
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return false;
    const auto ret = execute_cmd_get_single_info(GET_INI_VALUE_CMD, KEY_HIDE_CURSOR, WESTON_CONFIG_FILE);
    if (!ret.first.empty()) {
        // update current value
        return execute_cmd_set_info(SET_INI_STRING_VALUE_CMD, KEY_HIDE_CURSOR, bool_cast(hide), WESTON_CONFIG_FILE);
    } else {
        // insert hide-cursor/value after idle-time
        return execute_cmd_set_info(INSERT_INI_KEY_VALUE_CMD, KEY_TOUCH_CALIBRATOR, KEY_HIDE_CURSOR, bool_cast(hide), WESTON_CONFIG_FILE);
    }
}

string TPCScreenUtility::get_top_bar_position()
{
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return STRING_TOP;
    const auto ret = execute_cmd_get_single_info(GET_INI_VALUE_CMD, KEY_PANEL_POSITION, WESTON_CONFIG_FILE);
    return ret.first;
}

bool TPCScreenUtility::set_top_bar_position(const char* position)
{
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return false;
    const auto ret = execute_cmd_get_single_info(GET_INI_VALUE_CMD, KEY_PANEL_POSITION, WESTON_CONFIG_FILE);
    if (ret.first.empty()) {
        char value_buff[BUFF_SIZE]= {0};
        snprintf(value_buff, BUFF_SIZE, INSERT_SHELL_SETTING_TMPLATE, WESTON_SECTION_SHELL, KEY_PANEL_POSITION, position);
        return execute_cmd_set_info(INSERT_INI_SECTION_KEY_VALUE_CMD, value_buff, WESTON_CONFIG_FILE);
    } else {
        return execute_cmd_set_info(SET_INI_STRING_VALUE_CMD, KEY_PANEL_POSITION, position, WESTON_CONFIG_FILE);
    }
}

string TPCScreenUtility::get_rotate_screen()
{
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return STRING_TOP;
    const auto ret = execute_cmd_get_single_info(GET_INI_VALUE_CMD, KEY_TRANSFORM, WESTON_CONFIG_FILE);
    return ret.first;
}

bool TPCScreenUtility::set_rotate_screen(const char* rotateDegree)
{
    if (!is_file_exist(WESTON_CONFIG_FILE))
        return false;
    const auto ret = execute_cmd_get_single_info(GET_STRING_CMD, WESTON_CONFIG_FILE, KEY_OUTPUT_NAME, IMX_DISPLAY_NAME);
    if (ret.first.empty()) {
        char value_buff[BUFF_SIZE]= {0};
        snprintf(value_buff, BUFF_SIZE, INSERT_OUTPUT_SETTING_TMPLATE, WESTON_SECTION_OUTPUT, KEY_OUTPUT_NAME, IMX_DISPLAY_NAME, KEY_TRANSFORM, rotateDegree);
        return execute_cmd_set_info(INSERT_INI_SECTION_KEY_VALUE_CMD, value_buff, WESTON_CONFIG_FILE);
    } else {
        return execute_cmd_set_info(SET_INI_STRING_VALUE_CMD, KEY_TRANSFORM, rotateDegree, WESTON_CONFIG_FILE);
    }
}

string TPCScreenUtility::get_gesture_type()
{
    if (!is_file_exist(GESTURE_CONFIG_FILE))
        return GESTURE_TYPE_GENERAL;
    const auto ret = execute_cmd_get_single_info(GET_INI_VALUE_CMD, KEY_GESTURE_TYPE, GESTURE_CONFIG_FILE);
    return ret.first;
}

pair<vector<string>, bool> TPCScreenUtility::get_gesture_list()
{
    const auto ret = execute_cmd_get_single_info(GENERAL_GET_INI_VALUE_CMD, GESTURE_CONFIG_FILE, GESTURE_SECTION, KEY_GESTURE_LIST);
    m_gestureList = split_string_to_vector(ret.first.c_str(), DELIMITER_STRING);
    return make_pair(m_gestureList, ret.second);
}

pair<vector<string>, bool> TPCScreenUtility::get_gesture_action_list()
{
    const auto ret = execute_cmd_get_single_info(GENERAL_GET_INI_VALUE_CMD, GESTURE_CONFIG_FILE, GESTURE_ACTION_SECTION, KEY_GESTURE_ACTION_LIST);
    m_gestureActionList = split_string_to_vector(ret.first.c_str(), DELIMITER_STRING);
    return make_pair(m_gestureActionList, ret.second);
}

string TPCScreenUtility::_get_gesture_action(const char *gestureType, const char *actionKey)
{
    char value_buff[BUFF_SIZE]= {0};
    snprintf(value_buff, BUFF_SIZE, "%s.%s", gestureType, actionKey);
    return m_gestureActionMap[value_buff];
}
bool TPCScreenUtility::_get_gesture_action_enabled(const char *gestureType, const char *actionKey)
{
    const auto ret = execute_cmd_get_single_info(GENERAL_GET_INI_VALUE_CMD, GESTURE_CONFIG_FILE, gestureType, actionKey);
    if (ret.first.empty())
        return false;
    else
        return true;
}
bool TPCScreenUtility::_set_gesture_action_enabled(const char *gestureType, const char *actionKey, bool enabled)
{
    if (enabled) {
        string actionValue = _get_gesture_action(gestureType, actionKey);
        return execute_cmd_set_info(GENERAL_SET_INI_VALUE_CMD, GESTURE_CONFIG_FILE, gestureType, actionKey, actionValue.c_str());
    }
    else {
        return execute_cmd_set_info(GENERAL_SET_INI_EMPTY_VALUE_CMD, GESTURE_CONFIG_FILE, gestureType, actionKey);
    }
}

bool TPCScreenUtility::get_2_finger_gesture_swipe_up_enabled()
{
    return _get_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_UP);
}
bool TPCScreenUtility::set_2_finger_gesture_swipe_up_enabled(bool enabled)
{
    return _set_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_UP, enabled);
}
bool TPCScreenUtility::get_2_finger_gesture_swipe_down_enabled()
{
    return _get_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_DOWN);
}
bool TPCScreenUtility::set_2_finger_gesture_swipe_down_enabled(const bool enabled)
{
    return _set_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_DOWN, enabled);
}
bool TPCScreenUtility::get_2_finger_gesture_swipe_right_enabled()
{
    return _get_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_RIGHT);
}
bool TPCScreenUtility::set_2_finger_gesture_swipe_right_enabled(const bool enabled)
{
    return _set_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_RIGHT, enabled);
}
bool TPCScreenUtility::get_2_finger_gesture_swipe_left_enabled()
{
    return _get_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_LEFT);
}
bool TPCScreenUtility::set_2_finger_gesture_swipe_left_enabled(const bool enabled)
{
    return _set_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_LEFT, enabled);
}
bool TPCScreenUtility::set_all_finger_gesture_enabled(const bool enabled)
{
    bool result = true;
    result &= _set_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_UP, enabled);
    result &= _set_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_DOWN, enabled);
    result &= _set_gesture_action_enabled(TWO_FINGER_GESTURE_SECTION, KEY_GESTURE_SWIPE_RIGHT, enabled);
    return result;
}

bool TPCScreenUtility::get_gesture_service_enabled()
{
    bool enabled = true;
    const auto ret = execute_cmd_get_single_info(GET_GESTURE_SERVICE_ENABLED_CMD);
    enabled = !ret.first.empty();
    return enabled;
}

bool TPCScreenUtility::set_gesture_service_enabled(const bool enabled)
{
    bool result = true;
    if (enabled) {
        result &= enable_gesture_service();
        result &= restart_gesture_service();
    }
    else {
        result &= stop_gesture_service();
        result &= disable_gesture_service();
    }
    return result;
}

void TPCScreenUtility::restart_desktop_service()
{
    execute_cmd(RESTART_WESTON_CMD);
}

bool TPCScreenUtility::start_gesture_service()
{
    return execute_cmd_set_info(OPERATE_GESTURE_SERVICE_CMD, STRING_START);
}
bool TPCScreenUtility::stop_gesture_service()
{
    return execute_cmd_set_info(OPERATE_GESTURE_SERVICE_CMD, STRING_STOP);
}
bool TPCScreenUtility::enable_gesture_service()
{
    return execute_cmd_set_info(OPERATE_GESTURE_SERVICE_CMD, STRING_ENABLE);
}
bool TPCScreenUtility::disable_gesture_service()
{
    return execute_cmd_set_info(OPERATE_GESTURE_SERVICE_CMD, STRING_DISABLE);
}
bool TPCScreenUtility::restart_gesture_service()
{
    return execute_cmd_set_info(OPERATE_GESTURE_SERVICE_CMD, STRING_RESTART);
}
int TPCScreenUtility::get_screenshots_count()
{
    const auto ret = execute_cmd_get_single_info(COUNT_SCREENSHOTS_CMD);
    return std::stoi(ret.first);
}
pair<string, bool> TPCScreenUtility::export_screenshots(const char* exportFolder)
{
    bool result = false;
    string screenshotFolder = SCREENSHOTS_FOLDER;
    if (screenshotFolder.rfind(exportFolder, 0) == 0) {
        return make_pair("Screenshots are under /userdata. Please select another folder.", result);
    }
    result = execute_cmd_set_info(COPY_SCREENSHOTS_CMD, exportFolder);
    // make sure file flush to disk
    sync();
    return make_pair("", result);
}
bool TPCScreenUtility::delete_screenshots()
{
    return execute_cmd_set_info(RM_SCREENSHOTS_CMD);
}
