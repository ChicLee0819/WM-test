// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef UTILITY_H
#define UTILITY_H

#include <cstdarg>
#include <string>
#include <vector>

#define ROOT_USER "root"
#define WESTON_USER "weston"
#define USER_JOINT_NAME "user"

#define STRING_BOOL_TRUE "true"
#define STRING_BOOL_FALSE "false"

#define STRING_START "start"
#define STRING_STOP "stop"
#define STRING_ENABLE "enable"
#define STRING_DISABLE "disable"
#define STRING_RESTART "restart"

#define STRING_UP "up"
#define STRING_DOWN "down"

#define TMP_FOLDER "/tmp"

#define BUFF_SIZE 1024
#define CMD_SIZE 1024

// NOTE: atcc.ini cannot handle duplicate section names
#define GENERAL_SET_INI_VALUE_CMD "/usr/local/bin/atcc.ini -f %s -a write -s %s -k %s -v %s"
#define GENERAL_SET_INI_EMPTY_VALUE_CMD "/usr/local/bin/atcc.ini -f %s -a write -s %s -k %s"
#define GENERAL_GET_INI_VALUE_CMD "/usr/local/bin/atcc.ini -f %s -a read -s %s -k %s"

std::pair<std::string, int> execute_cmd(const char *cmd);
std::pair<std::string, int> execute_cmd_without_read(const char *cmd);
std::pair<std::vector<std::string>, bool> execute_cmd_get_vector(const char *cmd, ...);
std::pair<std::string, bool> execute_cmd_get_single_info(const char *cmd, ...);
bool execute_cmd_set_info(const char *cmd, ...);
bool cp_file(const char *source, const char *target);
bool mv_file(const char *source, const char *target);
bool tar_folder_to_gz(const char *gzFilename, const char *sourceFolder);
bool untar_gz_to_folder(const char *gzFilename, const char *targetFolder);
bool is_file_exist(const char *path);
bool is_folder_exist(const char *path);
bool write_file(const char *filename, const char *context);
void sys_mkdir(const char *path);
bool sys_restore_system_user(const char *source, const char *target, const char *sysUser);
std::pair<std::string, bool> sys_change_user_password(const char *username, const char *password);
std::vector<std::string> split_string_to_vector(const char *input, const char *token);
std::pair<std::string, bool> base64_encode(const char *input);
std::pair<std::string, bool> base64_decode(const char *input);
std::pair<std::string, bool> encrypt_text(const char *input, const char *key);
std::pair<std::string, bool> decrypt_text(const char *input, const char *key);
std::string get_filename_from_fullpath(const char *path);
const char *bool_cast(const bool value);
bool is_server_available(const char *address, const char *port);
bool is_local_port_available(const char *port);
bool is_contains_special_characters(const char *input);
bool is_contains_xml_special_characters(const char *input);
bool is_valid_domain(const char *domain);
bool is_valid_ip_address(const char *address);
bool is_valid_user_password(const char *password);
bool is_user_password_exists(const char *username);
bool is_numbers(const char *input);
bool is_hex_color(const char *color);
#endif // UTILITY_H
