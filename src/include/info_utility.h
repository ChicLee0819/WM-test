// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef INFO_UTILITY_H
#define INFO_UTILITY_H

class IDeviceInfoUtility {
public:
    virtual ~IDeviceInfoUtility() {}
    virtual float get_temperature() = 0;
};

class TPCDeviceInfoUtility: public IDeviceInfoUtility {
public:
    float get_temperature() override;
};
#endif // INFO_UTILITY_H
