// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <cstdio>
#include <cstring>
#include <array>
#include <sstream>
#include <fstream>
#include <QDebug>

#include "./include/utility.h"
#include "./include/storage_utility.h"

#define STORAGE_TYPE_DISK    "disk"
#define STORAGE_TYPE_PART    "part"

#define STORAGE_NAME_SD_CARD "mmcblk1"
#define STORAGE_NAME_EMMC    "mmcblk2"
#define STORAGE_SD_CARD      "/dev/mmcblk1"
#define STORAGE_EMMC         "/dev/mmcblk2"

#define ROOT_MOUNT_POINT     "/"
#define ROOT_FILE_SYSTEM     "/dev/root"

// ex: lsblk
/*
NAME         MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
mtdblock0     31:0    0    8M  0 disk
mmcblk2      179:0    0 14.7G  0 disk
|-mmcblk2p1  179:1    0  580M  0 part /run/media/mmcblk2p1
|-mmcblk2p2  179:2    0  400M  0 part /run/media/mmcblk2p2
|-mmcblk2p3  179:3    0    8G  0 part /
`-mmcblk2p4  179:4    0  5.7G  0 part /userdata
mmcblk2boot0 179:32   0    4M  1 disk
mmcblk2boot1 179:64   0    4M  1 disk
*/
const char* GET_DISK_SIZE_CMD =            "lsblk /dev/%s | grep disk | awk -F ' ' '{print $4}' | tr -d '\\n'";

// NR variable contains the line number of the file when awk command parses the file
// NR > 2 is skip two first lines
// ex: lsblk -p /dev/mmcblk2
/*
NAME             MAJ:MIN RM  SIZE RO TYPE MOUNTPOINT
/dev/mmcblk2     179:0    0 14.7G  0 disk
|-/dev/mmcblk2p1 179:1    0  580M  0 part /run/media/mmcblk2p1
|-/dev/mmcblk2p2 179:2    0  400M  0 part /run/media/mmcblk2p2
|-/dev/mmcblk2p3 179:3    0    8G  0 part /
`-/dev/mmcblk2p4 179:4    0  5.7G  0 part /userdata
*/
const char* GET_PARTITION_BLOCK_CMD =      "lsblk -p /dev/%s > /tmp/.%s_blk";
const char* GET_DISK_PARTIONS_CMD =        "cat /tmp/.%s_blk | awk 'NR>2 {print $1}' | awk -F '-' '{print $2}'";
const char* GET_PARTITION_SIZE_CMD =       "cat /tmp/.%s_blk | grep %s | awk -F ' ' '{print $4}' | tr -d '\\n'";

// ex: lsblk -fp /dev/mmcblk2
/*
NAME             FSTYPE LABEL    UUID                                 FSAVAIL FSUSE% MOUNTPOINT
/dev/mmcblk2
|-/dev/mmcblk2p1 vfat   boot     1EEE-477A                             551.4M     5% /run/media/mmcblk2p1
|-/dev/mmcblk2p2 vfat   recovery 1EF0-6C50                               367M     8% /run/media/mmcblk2p2
|-/dev/mmcblk2p3 ext4   rootfs   c7938351-ac5a-4ff0-ab46-6eb9426d7fc5    4.1G    43% /
`-/dev/mmcblk2p4 ext4   userdata f87194b6-dd6a-4f3d-962c-f59b78aa1cc6    5.3G     0% /userdata
*/
const char* GET_PARTITION_FS_ALL_CMD =     "lsblk -fp /dev/%s > /tmp/.%s_fs_all";
const char* GET_PARTITION_FSTYPE_CMD =     "cat /tmp/.%s_fs_all | grep %s | awk -F ' ' '{print $2}' | tr -d '\\n'";
const char* GET_PARTITION_LABEL_CMD =      "cat /tmp/.%s_fs_all | grep %s | awk -F ' ' '{print $3}' | tr -d '\\n'";
const char* GET_PARTITION_UUID_CMD =       "cat /tmp/.%s_fs_all | grep %s | awk -F ' ' '{print $4}' | tr -d '\\n'";
const char* GET_PARTITION_FSAVAIL_CMD =    "cat /tmp/.%s_fs_all | grep %s | awk -F ' ' '{print $5}' | tr -d '\\n'";
const char* GET_PARTITION_FSUSE_CMD =      "cat /tmp/.%s_fs_all | grep %s | awk -F ' ' '{print $6}' | tr -d '\\n'";
const char* GET_PARTITION_MOUNTPOINT_CMD = "cat /tmp/.%s_fs_all | grep %s | awk -F ' ' '{print $7}' | tr -d '\\n'";

// ex: df -h
/*
Filesystem      Size  Used Avail Use% Mounted on
/dev/root       7.9G  3.4G  4.1G  46% /
devtmpfs        669M  4.0K  669M   1% /dev
tmpfs           990M  8.0K  990M   1% /tmp
/dev/mmcblk2p1  579M   28M  552M   5% /run/media/mmcblk2p1
/dev/mmcblk2p2  400M   33M  367M   9% /run/media/mmcblk2p2
/dev/mmcblk2p4  5.6G   23M  5.3G   1% /userdata
tmpfs           198M  8.0K  198M   1% /run/user/0
*/
const char* GET_PARTITION_USED_CMD =         "df -h | grep %s | awk -F ' ' '{print $3}' | tr -d '\\n'";
const char* GET_PARTITION_USED_PERCENT_CMD = "df -h | grep %s | awk -F ' ' '{print $5}' | tr -d '\\n'";

BlockDeviceData::BlockDeviceData() {
    this->m_name = "";
    this->m_size = "";
    this->m_fstype = "";
    this->m_label = "";
    this->m_uuid = "";
    this->m_mount_point = "";

    // usage
    this->m_avail = "";
    this->m_used = "";
    this->m_used_percent = "";
}
void BlockDeviceData::setName(const string& name) {
    this->m_name = name;
}
void BlockDeviceData::setSize(const string& size) {
    this->m_size = size;
}
void BlockDeviceData::setFSType(const string& fstype) {
    this->m_fstype = fstype;
}
void BlockDeviceData::setLabel(const string& label) {
    this->m_label = label;
}
void BlockDeviceData::setUUID(const string& uuid) {
    this->m_uuid = uuid;
}
void BlockDeviceData::setMountPoint(const string& mountpoint) {
    this->m_mount_point = mountpoint;
}
string BlockDeviceData::getName() {
    return this->m_name;
}
string BlockDeviceData::getSize() {
    return this->m_size;
}
string BlockDeviceData::getFSType() {
    return this->m_fstype;
}
string BlockDeviceData::getLabel() {
    return this->m_label;
}
string BlockDeviceData::getUUID() {
    return this->m_uuid;
}
string BlockDeviceData::getMountPoint() {
    return this->m_mount_point;
}
void BlockDeviceData::setAvailable(const string& avail) {
    this->m_avail = avail;
}
void BlockDeviceData::setUsed(const string& used) {
    this->m_used = used;
}
void BlockDeviceData::setUsedPercent(const string& used_percent) {
    this->m_used_percent = used_percent;
}
string BlockDeviceData::getAvailable() {
    return this->m_avail;
}
string BlockDeviceData::getUsed() {
    return this->m_used;
}
string BlockDeviceData::getUsedPercent() {
    return this->m_used_percent;
}

pair<string, bool> IStorageUtility::_get_device_info(const char* cmd, const char* disk) {
    bool result = false;
    char cmd_buff[CMD_SIZE]= {0};
    // check input
    if (!disk)
        return make_pair("Missing disk", result);

    snprintf(cmd_buff, CMD_SIZE, cmd, disk, disk);
    const auto ret = execute_cmd_without_read(cmd_buff);
    result = (ret.second == EXIT_SUCCESS);
    return make_pair(ret.first, result);
}

pair<string, bool> IStorageUtility::get_partition_fs_all(const string& disk) {
    return _get_device_info(GET_PARTITION_FS_ALL_CMD, disk.c_str());
}

pair<string, bool> IStorageUtility::get_partition_block(const string& disk) {
    return _get_device_info(GET_PARTITION_BLOCK_CMD, disk.c_str());
}

pair<string, bool> IStorageUtility::get_partition_size(const string& disk, const string& partition) {
    return execute_cmd_get_single_info(GET_PARTITION_SIZE_CMD, disk.c_str(), partition.c_str());
}

pair<string, bool> IStorageUtility::get_partition_fstype(const string& disk, const string& partition) {
    return execute_cmd_get_single_info(GET_PARTITION_FSTYPE_CMD, disk.c_str(), partition.c_str());
}

pair<string, bool> IStorageUtility::get_partition_label(const string& disk, const string& partition) {
    return execute_cmd_get_single_info(GET_PARTITION_LABEL_CMD, disk.c_str(), partition.c_str());
}

pair<string, bool> IStorageUtility::get_partition_uuid(const string& disk, const string& partition) {
    return execute_cmd_get_single_info(GET_PARTITION_UUID_CMD, disk.c_str(), partition.c_str());
}

pair<string, bool> IStorageUtility::get_partition_mount_point(const string& disk, const string& partition) {
    return execute_cmd_get_single_info(GET_PARTITION_MOUNTPOINT_CMD, disk.c_str(), partition.c_str());
}

pair<string, bool> IStorageUtility::get_partition_available(const string& disk, const string& partition) {
    return execute_cmd_get_single_info(GET_PARTITION_FSAVAIL_CMD, disk.c_str(), partition.c_str());
}

pair<string, bool> IStorageUtility::get_partition_used_percent(const string& partition, const string& mount_point) {
    string rootMountPoint(ROOT_MOUNT_POINT);
    if (rootMountPoint.compare(mount_point) == 0) {
        // if mount point is root, change partition (/dev/mmcblk2p3) to /dev/root for df
        return execute_cmd_get_single_info(GET_PARTITION_USED_PERCENT_CMD, ROOT_FILE_SYSTEM);
    } else {
        return execute_cmd_get_single_info(GET_PARTITION_USED_PERCENT_CMD, partition.c_str());
    }
}

pair<string, bool> IStorageUtility::get_partition_used(const string& partition, const string& mount_point) {
    string rootMountPoint(ROOT_MOUNT_POINT);
    if (rootMountPoint.compare(mount_point) == 0) {
        // if mount point is root, change partition (/dev/mmcblk2p3) to /dev/root for df
        return execute_cmd_get_single_info(GET_PARTITION_USED_CMD, ROOT_FILE_SYSTEM);
    } else {
        return execute_cmd_get_single_info(GET_PARTITION_USED_CMD, partition.c_str());
    }
}

bool IStorageUtility::_is_device_exist(const char *device) {
    ifstream indev(device);
    return indev.good();
}

pair<vector<BlockDeviceData>, bool> IStorageUtility::_get_device_parts(const char* device_name) {
    vector<BlockDeviceData> values;
    const auto retFSAll = get_partition_fs_all(device_name);
    const auto retBlock = get_partition_block(device_name);
    if (!retFSAll.second) {
        qDebug("get partition fs all failed");
        return make_pair(values, retFSAll.second);
    }
    if (!retBlock.second) {
        qDebug("get partition block failed");
        return make_pair(values, retBlock.second);
    }
    const auto retParts = execute_cmd_get_vector(GET_DISK_PARTIONS_CMD, device_name);
    for (int i = 0; i < (int)retParts.first.size(); i ++) {
        const auto retSize = get_partition_size(device_name, retParts.first.at(i));
        const auto retFSType = get_partition_fstype(device_name, retParts.first.at(i));
        const auto retLabel = get_partition_label(device_name, retParts.first.at(i));
        const auto retUUID = get_partition_uuid(device_name, retParts.first.at(i));
        const auto retMountPoint = get_partition_mount_point(device_name, retParts.first.at(i));
        const auto retAvailable = get_partition_available(device_name, retParts.first.at(i));
        const auto retUsedPercent = get_partition_used_percent(retParts.first.at(i), retMountPoint.first);
        const auto retUsed = get_partition_used(retParts.first.at(i), retMountPoint.first);
        BlockDeviceData deviceData;
        deviceData.setName(retParts.first.at(i));
        deviceData.setSize(retSize.first);
        deviceData.setFSType(retFSType.first);
        deviceData.setLabel(retLabel.first);
        deviceData.setUUID(retUUID.first);
        deviceData.setMountPoint(retMountPoint.first);
        deviceData.setAvailable(retAvailable.first);
        deviceData.setUsedPercent(retUsedPercent.first);
        deviceData.setUsed(retUsed.first);

        values.push_back(deviceData);
    }
    return make_pair(values, retParts.second);
}

pair<string, bool> TPCStorageUtility::get_emmc_size() {
    if (!_is_device_exist(STORAGE_EMMC))
        return make_pair(string(), false);
    return execute_cmd_get_single_info(GET_DISK_SIZE_CMD, STORAGE_NAME_EMMC);
}

pair<string, bool> TPCStorageUtility::get_sd_card_size() {
    if (!_is_device_exist(STORAGE_SD_CARD))
        return make_pair(string(), false);
    return execute_cmd_get_single_info(GET_DISK_SIZE_CMD, STORAGE_NAME_SD_CARD);
}

pair<vector<BlockDeviceData>, bool> TPCStorageUtility::get_emmc_parts() {
    return _get_device_parts(STORAGE_NAME_EMMC);
}

pair<vector<BlockDeviceData>, bool> TPCStorageUtility::get_sd_card_parts() {
    return _get_device_parts(STORAGE_NAME_SD_CARD);
}


