// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef RESTORE_UTILITY_H
#define RESTORE_UTILITY_H

#define RESTORE_LOCK_FILE "/tmp/.restore.lock"

class ConfigUtility;

class RestoreUtility {
public:
    // import/export from other device
    std::pair<std::string, bool> export_config(const char* exportFile, ConfigUtility* pLocalConfig);
    std::pair<std::string, bool> import_config(const char* filepath, ConfigUtility* pLocalConfig);

    // restore from local backup
    bool restore_user_config_from_backup();
    bool restore_config_from_backup();

private:
    std::pair<std::string, bool> _extract_file(const char* filepath);
    void _backup_system(ConfigUtility* pConfigUtil);
    bool _restore_screen(ConfigUtility* pConfigUtil);
    void _restore_time(ConfigUtility* pConfigUtil);
    void _restore_opcua(ConfigUtility* pConfigUtil);
    void _restore_network(ConfigUtility* pConfigUtil);
    bool _restore_system(ConfigUtility* pConfigUtil);
    bool _is_readonly_mode();
};

#endif // RESTORE_UTILITY_H
