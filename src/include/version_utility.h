// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef VERSION_UTILITY_H
#define VERSION_UTILITY_H

#include <string>

using namespace std;

class IVersionUtility {
public:
    virtual ~IVersionUtility() {}
    pair<string, bool> get_app_version();
    pair<string, bool> get_qt_runtime_version();

    virtual pair<string, bool> get_image_version() = 0;
    virtual pair<string, bool> get_kernel_version() = 0;
    virtual pair<string, bool> get_uboot_version() = 0;
    virtual pair<string, bool> get_openssl_version() = 0;
    virtual pair<string, bool> get_java_runtime_version() = 0;
    virtual pair<string, bool> get_chromium_version() = 0;
};

class TPCVersionUtility: public IVersionUtility {
public:
    pair<string, bool> get_image_version() override;
    pair<string, bool> get_kernel_version() override;
    pair<string, bool> get_uboot_version() override;
    pair<string, bool> get_openssl_version() override;
    pair<string, bool> get_java_runtime_version() override;
    pair<string, bool> get_chromium_version() override;
};

#endif // VERSION_UTILITY_H
