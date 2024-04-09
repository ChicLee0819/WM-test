// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>

#include "./include/utility.h"
#include "./include/config_utility.h"
#include "./include/startup_utility.h"

string StartupApplication::get_startup_name() {
    return this->m_name;
}

string StartupApplication::get_startup_command() {
    return this->m_command;
}

void StartupApplication::set_startup_name(const char* name) {
    this->m_name = string(name);
}

void StartupApplication::set_startup_command(const char* command) {
    this->m_command = string(command);
}

StartupSettings::StartupSettings() {
    this->set_startup_name(STARTUP_NAME_SETTINGS);
    this->set_startup_command(STARTUP_COMMAND_SETTINGS);
}

StartupSettings::~StartupSettings() {
}

StartupChromium::StartupChromium() {
    this->set_startup_name(STARTUP_NAME_CHROMIUM);
    this->set_startup_command(STARTUP_COMMAND_CHROMIUM);
}

StartupChromium::~StartupChromium() {
}

StartupKioskChromium::StartupKioskChromium() {
    this->set_startup_name(STARTUP_NAME_CHROMIUM_KIOSK);
    this->set_startup_command(STARTUP_COMMAND_CHROMIUM_KIOSK);
}

StartupKioskChromium::~StartupKioskChromium() {
}

StartupStaticPage::StartupStaticPage() {
    this->set_startup_name(STARTUP_NAME_STATIC_PAGE);
    this->set_startup_command(STARTUP_COMMAND_STATIC_PAGE);
}

StartupStaticPage::~StartupStaticPage() {
}

StartupStaticPageCustom::StartupStaticPageCustom() {
    this->set_startup_name(STARTUP_NAME_STATIC_PAGE_CUSTOM);
    this->set_startup_command(STARTUP_COMMAND_STATIC_PAGE_CUSTOM);
}

StartupStaticPageCustom::~StartupStaticPageCustom() {
}

StartupVNC::StartupVNC() {
    this->set_startup_name(STARTUP_NAME_VNC_VIEWER);
    this->set_startup_command(STARTUP_COMMAND_VNC_VIEWER);
}

StartupVNC::~StartupVNC() {
}

StartupNone::StartupNone() {
    this->set_startup_name(STARTUP_NAME_NONE);
    this->set_startup_command(STARTUP_COMMAND_NONE);
}

StartupNone::~StartupNone() {
}
