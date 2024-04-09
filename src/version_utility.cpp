// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <QDebug>

#include "./include/utility.h"
#include "./include/version_utility.h"

// ex: atcc.info
/*
Board Information:
       BOARD VERSION: Ver1.0test
       SERIAL NUMBER:
  MANUFACTURING TIME: 2023/05/08 02:59:15
Image Information:
                NAME: "NXP i.MX Release Distro"
             VERSION: "5.15-kirkstone (kirkstone)"
          VERSION_ID: 5.15-kirkstone
         PRETTY_NAME: "NXP i.MX Release Distro 5.15-kirkstone (kirkstone)"
     DISTRO_CODENAME: "kirkstone"
       IMAGE_VERSION: "TPC-100W-4g-v2.1"
      KERNEL_VERSION: "Linux version 5.15.71"
       UBOOT_VERSION: "2021.04-lf_v2021.04+ga3cae489e9"
       YOCTO_VERSION: 4.0.4
     UBOOT_BUILDDATE: "Sep 19 2022 - 03:30:41 +0000"
*/
const char* GET_IMAGE_VERSION_CMD = "atcc.info | grep -a IMAGE_VERSION |  tr -d '\"' | awk -F ': ' '{print $2}' | tr -d '\\n'";
const char* GET_KERNEL_VERSION_CMD = "atcc.info | grep -a KERNEL_VERSION |  tr -d '\"' | awk -F ': ' '{print $2}' | tr -d '\\n'";
const char* GET_UBOOT_VERSION_CMD = "atcc.info | grep -a UBOOT_VERSION |  tr -d '\"' | awk -F ': ' '{print $2}' | tr -d '\\n'";
// ex: openssl version
/*
OpenSSL 1.1.1g  21 Apr 2020
*/
const char* GET_OPENSSL_VERSION_CMD = "openssl version | awk -F ' ' '{print $2}' | tr -d '\\n'";
// ex: java -version
/*
java version "1.8.0_362"
Java(TM) SE Runtime Environment (build 1.8.0_362-b09)
Java HotSpot(TM) 64-Bit Server VM (build 25.362-b09, mixed mode)
*/
const char* GET_JAVA_VERSION_CMD = "java -version 2>&1 | grep version | tr -d '\"' | awk -F ' ' '{print $3}' | tr -d '\\n'";
// ex: chromium --version
/*
Chromium 101.0.4951.54 stable
*/
const char* GET_CHROMIUM_VERSION_CMD = "chromium --version | awk -F ' ' '{print $2}' | tr -d '\\n'";

pair<string, bool> IVersionUtility::get_app_version() {
    // app version is defined in .pro file
    string version = APP_VERSION;
    return make_pair(version, true);
}

pair<string, bool> IVersionUtility::get_qt_runtime_version() {
    // qt runtime version
    string qt_version = qVersion();
    return make_pair(qt_version, true);
}

pair<string, bool> TPCVersionUtility::get_image_version() {
    return execute_cmd_get_single_info(GET_IMAGE_VERSION_CMD);
}

pair<string, bool> TPCVersionUtility::get_kernel_version() {
    return execute_cmd_get_single_info(GET_KERNEL_VERSION_CMD);
}

pair<string, bool> TPCVersionUtility::get_uboot_version() {
    return execute_cmd_get_single_info(GET_UBOOT_VERSION_CMD);
}

pair<string, bool> TPCVersionUtility::get_openssl_version() {
    return execute_cmd_get_single_info(GET_OPENSSL_VERSION_CMD);
}

pair<string, bool> TPCVersionUtility::get_java_runtime_version() {
    return execute_cmd_get_single_info(GET_JAVA_VERSION_CMD);
}

pair<string, bool> TPCVersionUtility::get_chromium_version() {
    return execute_cmd_get_single_info(GET_CHROMIUM_VERSION_CMD);
}
