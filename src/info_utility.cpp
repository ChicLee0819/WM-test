// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <string>
#include <array>
#include <QDebug>

#include "./include/utility.h"
#include "./include/info_utility.h"

const int TEMPERATURE_UNIT = 1000;
const char* GET_TEMPERATURE_CMD = "cat /sys/class/thermal/thermal_zone0/temp";

float TPCDeviceInfoUtility::get_temperature() {
    float temperature = 0;
    // execute command to get brightness
    const auto ret = execute_cmd_get_single_info(GET_TEMPERATURE_CMD);
    temperature = std::stof(ret.first) / TEMPERATURE_UNIT;
    return temperature;
}
