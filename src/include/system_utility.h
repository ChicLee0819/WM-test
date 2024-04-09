// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef SYSTEM_UTILITY_H
#define SYSTEM_UTILITY_H

#include <string>

#define COM1_NAME "com1"
#define COM2_NAME "com2"

#define BOOT_LOGO_PATH "/etc/boot_logo.bmp"

using namespace std;

class ISystemUtility {
public:
    virtual ~ISystemUtility() {}
    virtual bool is_boot_from_sd_card() = 0;
    virtual bool get_readonly_mode() = 0;
    virtual bool get_system_user_login_desktop() = 0;
    virtual bool get_system_custom_user_login_desktop() = 0;
    virtual bool get_usb_enable() = 0;

    virtual bool set_readonly_mode(const bool readonly) = 0;
    virtual bool set_system_user_login_desktop(bool isUserLogin, bool isReboot) = 0;
    virtual bool set_usb_enable(const bool enabled) = 0;
    virtual bool set_reboot_system_crontab(bool enabled, const char* mode, 
                                            int minute, int hour, int dayofweek) = 0;
    virtual bool set_boot_logo_background_color(const char* color) = 0;

    virtual bool do_restart_crond_service() = 0;
    virtual bool do_init_ethernet() = 0;
    virtual bool do_init_com_port() = 0;
    virtual bool do_reboot() = 0;
    virtual bool do_shutdown() = 0;
    virtual bool open_terminal() = 0;
    virtual bool open_license_page() = 0;
};

class TPCSystemUtility: public ISystemUtility {
public:
    bool is_boot_from_sd_card() override;
    bool get_readonly_mode() override;
    bool get_system_user_login_desktop() override;
    bool get_system_custom_user_login_desktop() override;
    bool get_usb_enable() override;

    bool set_readonly_mode(const bool readonly) override;
    bool set_system_user_login_desktop(bool isUserLogin, bool isReboot) override;
    bool set_usb_enable(const bool enabled) override;
    bool set_reboot_system_crontab(bool enabled, const char* mode, 
                                    int minute, int hour, int dayofweek) override;
    bool set_boot_logo_background_color(const char* color) override;

    bool do_restart_crond_service() override;
    bool do_init_ethernet() override;
    bool do_init_com_port() override;
    bool do_reboot() override;
    bool do_shutdown() override;
    bool open_terminal() override;
    bool open_license_page() override;

private:
    pair<string, bool> _get_com_mode(const char* com);
    pair<string, bool> _get_com_baudrate(const char* com);
    bool _set_com_mode(const char* com, const char* mode);
    bool _set_com_baudrate(const char* com, const char* baudrate);
};
#endif // SYSTEM_UTILITY_H
