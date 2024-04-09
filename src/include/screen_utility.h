// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SCREEN_UTILITY_H
#define SCREEN_UTILITY_H

#include <string>
#include <vector>
#include <map>

#define GESTURE_TYPE_GENERAL "general"
#define GESTURE_TYPE_CUSTOM "custom"

#define SCREEN_BRIGHTNESS_MIN   1
#define SCREEN_BRIGHTNESS_MAX   100

class IScreenUtility {
public:
    virtual ~IScreenUtility() {}
    virtual int get_brightness() = 0;
    virtual bool set_brightness(const int brightness) = 0;
    virtual int get_screensaver_idle_time() = 0;
    virtual bool set_screensaver_idle_time(const int seconds) = 0;
    virtual bool get_hide_cursor() = 0;
    virtual bool set_hide_cursor(const bool hide) = 0;
    virtual std::string get_top_bar_position() = 0;
    virtual bool set_top_bar_position(const char* position) = 0;
    virtual std::string get_rotate_screen() = 0;
    virtual bool set_rotate_screen(const char* rotateDegree) = 0;
    virtual void restart_desktop_service() = 0;
    virtual std::string get_gesture_type() = 0;
    virtual std::pair<std::vector<std::string>, bool> get_gesture_list() = 0;
    virtual std::pair<std::vector<std::string>, bool> get_gesture_action_list() = 0;
    virtual bool get_2_finger_gesture_swipe_up_enabled() = 0;
    virtual bool set_2_finger_gesture_swipe_up_enabled(const bool enabled) = 0;
    virtual bool get_2_finger_gesture_swipe_down_enabled() = 0;
    virtual bool set_2_finger_gesture_swipe_down_enabled(const bool enabled) = 0;
    virtual bool get_2_finger_gesture_swipe_right_enabled() = 0;
    virtual bool set_2_finger_gesture_swipe_right_enabled(const bool enabled) = 0;
    virtual bool get_2_finger_gesture_swipe_left_enabled() = 0;
    virtual bool set_2_finger_gesture_swipe_left_enabled(const bool enabled) = 0;
    virtual bool set_all_finger_gesture_enabled(const bool enabled) = 0;
    virtual bool get_gesture_service_enabled() = 0;
    virtual bool set_gesture_service_enabled(const bool enabled) = 0;
    virtual bool start_gesture_service() = 0;
    virtual bool stop_gesture_service() = 0;
    virtual bool enable_gesture_service() = 0;
    virtual bool disable_gesture_service() = 0;
    virtual bool restart_gesture_service() = 0;
    virtual int get_screenshots_count() = 0;
    virtual std::pair<std::string, bool> export_screenshots(const char* exportFolder) = 0;
    virtual bool delete_screenshots() = 0;
};

class TPCScreenUtility: public IScreenUtility {
public:
    TPCScreenUtility();
    int get_brightness() override;
    bool set_brightness(const int brightness) override;
    int get_screensaver_idle_time() override;
    bool set_screensaver_idle_time(const int seconds) override;
    bool get_hide_cursor() override;
    bool set_hide_cursor(const bool hide) override;
    std::string get_top_bar_position() override;
    bool set_top_bar_position(const char* position) override;
    std::string get_rotate_screen() override;
    bool set_rotate_screen(const char* rotateDegree) override;
    void restart_desktop_service() override;
    std::string get_gesture_type() override;
    std::pair<std::vector<std::string>, bool> get_gesture_list() override;
    std::pair<std::vector<std::string>, bool> get_gesture_action_list() override;
    bool get_2_finger_gesture_swipe_up_enabled() override;
    bool set_2_finger_gesture_swipe_up_enabled(const bool enabled) override;
    bool get_2_finger_gesture_swipe_down_enabled() override;
    bool set_2_finger_gesture_swipe_down_enabled(const bool enabled) override;
    bool get_2_finger_gesture_swipe_right_enabled() override;
    bool set_2_finger_gesture_swipe_right_enabled(const bool enabled) override;
    bool get_2_finger_gesture_swipe_left_enabled() override;
    bool set_2_finger_gesture_swipe_left_enabled(const bool enabled) override;
    bool set_all_finger_gesture_enabled(const bool enabled) override;
    bool get_gesture_service_enabled() override;
    bool set_gesture_service_enabled(const bool enabled) override;
    bool start_gesture_service() override;
    bool stop_gesture_service() override;
    bool enable_gesture_service() override;
    bool disable_gesture_service() override;
    bool restart_gesture_service() override;
    int get_screenshots_count() override;
    std::pair<std::string, bool> export_screenshots(const char* exportFolder) override;
    bool delete_screenshots() override;

private:
    bool _is_display_device_exist(const char* device);
    std::string _get_gesture_action(const char *gestureType, const char *actionKey);
    bool _get_gesture_action_enabled(const char *gestureType, const char *actionKey);
    bool _set_gesture_action_enabled(const char *gestureType, const char *actionKey, bool enabled);

    std::vector<std::string> m_gestureList;
    std::vector<std::string> m_gestureActionList;
    std::map<std::string, std::string> m_gestureActionMap;
};
#endif // SCREEN_UTILITY_H
