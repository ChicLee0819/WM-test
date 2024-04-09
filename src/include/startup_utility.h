// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef STARTUP_UTILITY_H
#define STARTUP_UTILITY_H

#include <string>

#define STARTUP_NAME_SETTINGS      "Settings"
#define STARTUP_COMMAND_SETTINGS   "/usr/bin/start_settings_keyshortcuts.sh"

#define STARTUP_NAME_CHROMIUM      "Chromium"
#define STARTUP_COMMAND_CHROMIUM   "/usr/bin/start_chromium.sh"

#define STARTUP_NAME_CHROMIUM_KIOSK    "Chromium_kiosk"
#define STARTUP_COMMAND_CHROMIUM_KIOSK "/usr/bin/start_kiosk_chromium.sh"

#define STARTUP_NAME_STATIC_PAGE    "StaticPage"
#define STARTUP_COMMAND_STATIC_PAGE "/usr/bin/start_static_page.sh"

#define STARTUP_NAME_STATIC_PAGE_CUSTOM    "StaticPage_custom"
#define STARTUP_COMMAND_STATIC_PAGE_CUSTOM "/usr/bin/start_static_page.sh --custom"

#define STARTUP_NAME_VNC_VIEWER    "VNCViewer"
#define STARTUP_COMMAND_VNC_VIEWER "/usr/bin/start_vnc_viewer.sh"

#define STARTUP_NAME_NONE    "None"
#define STARTUP_COMMAND_NONE ""

using namespace std;

class StartupApplication {
public:
    virtual ~StartupApplication() {}

    string get_startup_name();
    string get_startup_command();
    void set_startup_name(const char* name);
    void set_startup_command(const char* command);

private:
    string m_name;
    string m_command;
};

class StartupSettings : public StartupApplication {
public:
    StartupSettings();
    ~StartupSettings();
};

class StartupChromium : public StartupApplication {
public:
    StartupChromium();
    ~StartupChromium();
};

class StartupKioskChromium : public StartupApplication {
public:
    StartupKioskChromium();
    ~StartupKioskChromium();
};

class StartupStaticPage : public StartupApplication {
public:
    StartupStaticPage();
    ~StartupStaticPage();
};

class StartupStaticPageCustom : public StartupApplication {
public:
    StartupStaticPageCustom();
    ~StartupStaticPageCustom();
};

class StartupVNC : public StartupApplication {
public:
    StartupVNC();
    ~StartupVNC();
};

class StartupNone : public StartupApplication {
public:
    StartupNone();
    ~StartupNone();
};

#endif // STARTUP_UTILITY_H
