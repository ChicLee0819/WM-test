// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef STORAGE_UTILITY_H
#define STORAGE_UTILITY_H

#include <string>
#include <vector>

#define STORAGE_LABEL        "userdata"

using namespace std;

class BlockDeviceData
{
public:
    BlockDeviceData();
    void setName(const string& name);
    void setSize(const string& size);
    void setFSType(const string& fstype);
    void setLabel(const string& label);
    void setUUID(const string& uuid);
    void setMountPoint(const string& mountpoint);
    string getName();
    string getSize();
    string getFSType();
    string getLabel();
    string getUUID();
    string getMountPoint();

    // usage
    void setAvailable(const string& avail);
    void setUsed(const string& used);
    void setUsedPercent(const string& used_percent);
    string getAvailable();
    string getUsed();
    string getUsedPercent();

private:
    string m_name;
    string m_size;
    string m_fstype;
    string m_label;
    string m_uuid;
    string m_mount_point;

    // usage
    string m_avail;
    string m_used;
    string m_used_percent;
};

class IStorageUtility {
public:
    virtual ~IStorageUtility() {}
    virtual pair<string, bool> get_emmc_size() = 0;
    virtual pair<string, bool> get_sd_card_size() = 0;
    virtual pair<vector<BlockDeviceData>, bool> get_emmc_parts() = 0;
    virtual pair<vector<BlockDeviceData>, bool> get_sd_card_parts() = 0;

    pair<string, bool> get_partition_fs_all(const string& disk);
    pair<string, bool> get_partition_block(const string& disk);

    pair<string, bool> get_partition_size(const string& disk, const string& partition);
    pair<string, bool> get_partition_fstype(const string& disk, const string& partition);
    pair<string, bool> get_partition_label(const string& disk, const string& partition);
    pair<string, bool> get_partition_uuid(const string& disk, const string& partition);
    pair<string, bool> get_partition_mount_point(const string& disk, const string& partition);
    pair<string, bool> get_partition_available(const string& disk, const string& partition);
    pair<string, bool> get_partition_used_percent(const string& partition, const string& mount_point);
    pair<string, bool> get_partition_used(const string& partition, const string& mount_point);

    bool _is_device_exist(const char *device);
    pair<vector<BlockDeviceData>, bool> _get_device_parts(const char* device_name);
    pair<string, bool> _get_device_info(const char* cmd, const char* disk);
};

class TPCStorageUtility: public IStorageUtility {
public:
    pair<string, bool> get_emmc_size() override;
    pair<string, bool> get_sd_card_size() override;
    pair<vector<BlockDeviceData>, bool> get_emmc_parts() override;
    pair<vector<BlockDeviceData>, bool> get_sd_card_parts() override;
};

#endif // STORAGE_UTILITY_H
