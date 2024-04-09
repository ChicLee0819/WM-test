// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <array>
#include <set>
#include <sstream>
#include <regex>
#ifdef _WIN32
#else
#include <arpa/inet.h>
#endif
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

#include "./include/utility.h"

using namespace std;

#define MAX_DEBUG_OUTPUT_LINE 10
#define MIN_PASSWORD_LENGTH 7

#define STRING_NO_PASSWORD "NP"

const char *BASE64_ENCODE_TEXT_CMD = "echo '%s' | base64 | tr -d '\\n'";
const char *BASE64_DECODE_TEXT_CMD = "echo '%s' | base64 --decode | tr -d '\\n'";
const char *ENCRYPT_TEXT_CMD = "echo '%s' | openssl enc -aes-256-cbc -md sha512 -pbkdf2 -iter 100000 -base64 -k %s | tr -d '\\n'";
const char *DECRYPT_TEXT_CMD = "echo '%s' | openssl enc -aes-256-cbc -md sha512 -pbkdf2 -iter 100000 -base64 -k %s -d | tr -d '\\n'";
const char *BASE64_TEXT = "base64";
const char *SSH_CONNECT_CMD = "ssh -o ConnectTimeout=10 user@%s -v -p %s 2>&1";
const char *CONNECTION_ESTABLISHED_TEXT = "Connection established";
const char *CHECK_PORT_IN_USED = "netstat -tulpn | grep ':%s '";
const char *CP_CMD = "cp %s %s";
const char *MV_CMD = "mv %s %s";
const char *TAR_TO_GZ = "tar -zcf \"%s\" --directory %s . 2>&1";
const char *UNTAR_FROM_GZ = "tar -xzf \"%s\" --directory %s 2>&1";
const char *TAR_TEXT = "tar -";
const char *CHPASSWD_TEXT = "chpasswd 2>&1";
const char *CHANGE_PASSWORD_CMD = "echo \"%s:%s\" | chpasswd 2>&1";
const char *DOUBLE_QUOTE_CHAR = "\"";
const char *ESCAPE_CHAR = "\\";
const char *GET_USER_PASSWORD_STATUS_CMD = "passwd --status \"%s\" | awk -F ' ' '{print $2}' | tr -d '\\n'";
const char *GET_USER_HASH_PASSWORD_FROM_FILE_CMD = "grep \"^%s:\" \"%s\" | cut -f2 -d\":\" | tr -d '\\n'";
const char *SET_USER_HASH_PASSWORD_CMD = "usermod --password '%s' '%s'";

pair<string, int> execute_cmd(const char *cmd)
{
    array<char, 256> buffer;
    string result;
#ifdef _WIN32
    return make_pair(result, EXIT_FAILURE);
#else
    auto pipe = popen(cmd, "r");
    if (!pipe)
    {
        qDebug("popen() failed!");
        return make_pair(result, EXIT_FAILURE);
    }
    while (!feof(pipe))
    {
        if (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
            result += buffer.data();
    }
    auto rc = pclose(pipe);
    return make_pair(result, rc);
#endif
}

pair<string, int> execute_cmd_without_read(const char *cmd)
{
    string result;
#ifdef _WIN32
    return make_pair(result, EXIT_FAILURE);
#else
    auto pipe = popen(cmd, "r");
    if (!pipe)
    {
        qDebug("popen() failed!");
        return make_pair(result, EXIT_FAILURE);
    }
    auto rc = pclose(pipe);
    return make_pair(result, rc);
#endif
}

pair<vector<string>, bool> execute_cmd_get_vector(const char *cmd, ...)
{
    bool result = false;
    vector<string> values;
    char cmd_buff[CMD_SIZE] = {0};
    // check input
    if (!cmd || strlen(cmd) == 0)
        return make_pair(values, result);

    // format command with args
    va_list args;
    va_start(args, cmd);
    vsnprintf(cmd_buff, CMD_SIZE, cmd, args);
    va_end(args);

    const auto ret = execute_cmd(cmd_buff);
    // spit string by '\n' character
    stringstream valuesStream(ret.first.c_str());
    string value;
    while (getline(valuesStream, value, '\n'))
    {
        if (value.length() > 0)
            values.push_back(value);
    }
    result = (ret.second == EXIT_SUCCESS);
#ifdef _WIN32
#else
    // limit debug output
    if ((int)values.size() > MAX_DEBUG_OUTPUT_LINE)
    {
        qDebug("cmd:%s values.at(0):%s size:%d ret:%d", cmd_buff, values.at(0).c_str(), (int)values.size(), ret.second);
    }
    else
    {
        qDebug("cmd:%s value:%s size:%d ret:%d", cmd_buff, ret.first.c_str(), (int)values.size(), ret.second);
    }
#endif
    return make_pair(values, result);
}

pair<string, bool> execute_cmd_get_single_info(const char *cmd, ...)
{
    bool result = false;
    char cmd_buff[CMD_SIZE] = {0};
    // check input
    if (!cmd || strlen(cmd) == 0) {
        string value_buff;
        return make_pair(value_buff, result);
    }

    // format command with args
    va_list args;
    va_start(args, cmd);
    vsnprintf(cmd_buff, CMD_SIZE, cmd, args);
    va_end(args);

    const auto ret = execute_cmd(cmd_buff);
    result = (ret.second == EXIT_SUCCESS);
#ifdef _WIN32
#else
    // print log except encrypt/decrypt/chpasswd
    if (strstr(cmd, BASE64_TEXT) == NULL && strstr(cmd, CHPASSWD_TEXT) == NULL)
    {
        qDebug("cmd:%s value:%s ret:%d", cmd_buff, ret.first.c_str(), ret.second);
    }
#endif
    return make_pair(ret.first, result);
}

bool execute_cmd_set_info(const char *cmd, ...)
{
    bool result = false;
    char cmd_buff[CMD_SIZE] = {0};
    // check input
    if (!cmd || strlen(cmd) == 0)
        return result;

    // format command with args
    va_list args;
    va_start(args, cmd);
    vsnprintf(cmd_buff, CMD_SIZE, cmd, args);
    va_end(args);

    const auto ret = execute_cmd(cmd_buff);
    result = (ret.second == EXIT_SUCCESS);
#ifdef _WIN32
#else
    // print log except tar/chpasswd
    if (strstr(cmd, TAR_TEXT) == NULL && strstr(cmd, CHPASSWD_TEXT) == NULL)
    {
        qDebug("cmd:%s value:%s ret:%d", cmd_buff, ret.first.c_str(), ret.second);
    }
#endif
    return result;
}

bool cp_file(const char *source, const char *target)
{
    if (source && !is_file_exist(source)) {
        qDebug("file:%s not exist", source);
        return false;
    }
    return execute_cmd_set_info(CP_CMD, source, target);
}

bool mv_file(const char *source, const char *target)
{
    if (source && !is_file_exist(source)) {
        qDebug("file:%s not exist", source);
        return false;
    }
    return execute_cmd_set_info(MV_CMD, source, target);
}

bool tar_folder_to_gz(const char *gzFilename, const char *sourceFolder)
{
    if (sourceFolder && !is_folder_exist(sourceFolder)) {
        qDebug("folder:%s not exist", sourceFolder);
        return false;
    }
    return execute_cmd_set_info(TAR_TO_GZ, gzFilename, sourceFolder);
}

bool untar_gz_to_folder(const char *gzFilename, const char *targetFolder)
{
    if (gzFilename && !is_file_exist(gzFilename)) {
        qDebug("file:%s not exist", gzFilename);
        return false;
    }
    return execute_cmd_set_info(UNTAR_FROM_GZ, gzFilename, targetFolder);
}

bool is_file_exist(const char *path)
{
#ifdef _WIN32
    return true;
#else
    // check input
    if (!path)
        return false;
    QFileInfo check_file(path);
    // check if file exists
    if (check_file.exists() && check_file.isFile())
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
}

bool is_folder_exist(const char *path)
{
#ifdef _WIN32
    return true;
#else
    // check input
    if (!path)
        return false;
    QFileInfo check_folder(path);
    // check if folder exists
    if (check_folder.exists() && check_folder.isDir())
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
}

string get_filename_from_fullpath(const char *path)
{
#ifdef _WIN32
    return "";
#else
    // check input
    if (!path)
        return "";
    QFileInfo fullPath(path);
    return fullPath.fileName().toStdString();
#endif
}

bool write_file(const char *filename, const char *context)
{
#ifdef _WIN32
    return true;
#else
    // check input
    if (!filename || !context)
        return false;
    if (strlen(filename) == 0)
        return false;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug("Cannot open file for writing::%s", filename);
        return false;
    }

    QTextStream out(&file);
    QString str(context);
    out << str << "\n";
    file.close();
    return true;
#endif
}

void sys_mkdir(const char *path)
{
#ifdef _WIN32
#else
    QDir dir(path);
    if (!dir.exists())
    {
        dir.mkpath(path);
    }
#endif
}

bool sys_restore_system_user(const char *source, const char *target, const char *sysUser)
{
    // check input
    if (!source || !target)
        return false;
    if (strlen(source) == 0 || strlen(target) == 0)
        return false;
    if (!is_file_exist(source)) {
        qDebug("file:%s not exist", source);
        return false;
    }

    const auto retPass = execute_cmd_get_single_info(GET_USER_HASH_PASSWORD_FROM_FILE_CMD, sysUser, source);
    const auto ret = execute_cmd_get_single_info(SET_USER_HASH_PASSWORD_CMD, retPass.first.c_str(), sysUser);
    return ret.second;
}

pair<string, bool> sys_change_user_password(const char *username, const char *password)
{
    bool result = false;
    string message;
    // check input
    if (!username || !password)
        return make_pair(message, result);
    if (strlen(username) == 0 || strlen(password) == 0)
        return make_pair(message, result);

    // check password contain " char
    // add escape char for it
    size_t found = 0;
    string new_password = password;
    while ( (found = new_password.find(DOUBLE_QUOTE_CHAR, found)) != string::npos )
    {
        new_password.insert(found, ESCAPE_CHAR);
        found += strlen(DOUBLE_QUOTE_CHAR) + strlen(ESCAPE_CHAR);
    }
    const auto ret = execute_cmd_get_single_info(CHANGE_PASSWORD_CMD, username, new_password.c_str());
    if (!ret.first.empty())
    {
        // get first line error message
        // ex: BAD PASSWORD: The password is a palindrome
        istringstream f(ret.first);
        std::getline(f, message);
    }
    result = true;
    return make_pair(message, result);
}

vector<string> split_string_to_vector(const char *input, const char *token)
{
    vector<string> result;
    string::size_type begin, end;
    // check input
    if (!input || !token)
        return result;

    string inputStr(input);
    string splitToken(token);

    end = inputStr.find(splitToken);
    begin = 0;

    while (end != string::npos)
    {
        if (end - begin != 0)
        {
            result.push_back(inputStr.substr(begin, end - begin));
        }
        begin = end + splitToken.size();
        end = inputStr.find(splitToken, begin);
    }
    // add if split token is not at end
    if (begin != inputStr.length())
    {
        result.push_back(inputStr.substr(begin));
    }
    return result;
}

pair<string, bool> _process_base64_text(const char *cmd, const char *input)
{
    bool result = false;
    string value_buff;
    // check input
    if (!cmd || !input)
        return make_pair(value_buff, result);
    if (strlen(cmd) == 0 || strlen(input) == 0)
        return make_pair(value_buff, result);

    return execute_cmd_get_single_info(cmd, input);
}

pair<string, bool> base64_encode(const char *input)
{
    return _process_base64_text(BASE64_ENCODE_TEXT_CMD, input);
}

pair<string, bool> base64_decode(const char *input)
{
    return _process_base64_text(BASE64_DECODE_TEXT_CMD, input);
}

pair<string, bool> _process_secret_text(const char *cmd, const char *input, const char *key)
{
    bool result = false;
    string value_buff;
    // check input
    if (!cmd || !input || !key)
        return make_pair(value_buff, result);
    if (strlen(cmd) == 0 || strlen(input) == 0 || strlen(key) == 0)
        return make_pair(value_buff, result);

    return execute_cmd_get_single_info(cmd, input, key);
}

pair<string, bool> encrypt_text(const char *input, const char *key)
{
    return _process_secret_text(ENCRYPT_TEXT_CMD, input, key);
}

pair<string, bool> decrypt_text(const char *input, const char *key)
{
    return _process_secret_text(DECRYPT_TEXT_CMD, input, key);
}

const char *bool_cast(const bool value)
{
    return value ? "true" : "false";
}

bool is_server_available(const char *address, const char *port)
{
#ifdef _WIN32
    return true;
#else
    bool result = false;
    char cmd_buff[CMD_SIZE] = {0};
    // check input
    if (!address || !port)
    {
        qDebug("Missing address or port");
        return result;
    }

    snprintf(cmd_buff, CMD_SIZE, SSH_CONNECT_CMD, address, port);
    const auto ret = execute_cmd(cmd_buff);
    qDebug("cmd:%s output:%s ret:%d", cmd_buff, ret.first.c_str(), ret.second);
    if (strstr(ret.first.c_str(), CONNECTION_ESTABLISHED_TEXT) != NULL)
    {
        result = true;
    }
    return result;
#endif
}

bool is_local_port_available(const char *port)
{
#ifdef _WIN32
    return true;
#else
    bool result = true;
    char cmd_buff[CMD_SIZE] = {0};
    // check input
    if (!port)
    {
        qDebug("Missing port");
        return result;
    }

    snprintf(cmd_buff, CMD_SIZE, CHECK_PORT_IN_USED, port);
    const auto ret = execute_cmd(cmd_buff);
    qDebug("cmd:%s output:%s ret:%d", cmd_buff, ret.first.c_str(), ret.second);
    if (strlen(ret.first.c_str()) > 0)
    {
        result = false;
    }
    return result;
#endif
}

static bool _is_contains_characters(const char *input, const set<string> &characters)
{
#ifdef _WIN32
    return true;
#else
    bool is_contains = false;
    // check input
    if (!input)
    {
        qDebug("Missing input");
        return is_contains;
    }
    string value(input);
    // iterate through all the elements in a set
    for (set<string>::iterator it = characters.begin(); it != characters.end(); ++it) {
        if (value.find(*it) != string::npos) {
            is_contains = true;
            break;
        }
    }
    return is_contains;
#endif
}

bool is_contains_special_characters(const char *input)
{
    set<string> special_chars;
    // linux command special characters & | ; $ > < ` \ !
    special_chars.insert("&");
    special_chars.insert("|");
    special_chars.insert(";");
    special_chars.insert("$");
    special_chars.insert(">");
    special_chars.insert("<");
    special_chars.insert("`");
    special_chars.insert("\\");
    special_chars.insert("!");
    return _is_contains_characters(input, special_chars);
}

bool is_contains_xml_special_characters(const char *input)
{
    set<string> special_chars;
    // xml special characters > < & ' "
    special_chars.insert(">");
    special_chars.insert("<");
    special_chars.insert("&");
    special_chars.insert("'");
    special_chars.insert("\"");
    return _is_contains_characters(input, special_chars);
}

bool is_valid_domain(const char *domain)
{
#ifdef _WIN32
    return true;
#else
    // check input
    if (!domain)
    {
        qDebug("Missing domain");
        return false;
    }
    // regex to check valid domain name.
    const regex pattern("^(?!-)[A-Za-z0-9-]+([\\-\\.]{1}[a-z0-9]+)*\\.[A-Za-z]{2,6}$");
    string value(domain);
    return regex_match(value, pattern);
#endif
}

bool is_valid_ip_address(const char *address)
{
#ifdef _WIN32
    return true;
#else
    struct sockaddr_in sa;
    int result = 0;
    // check input
    if (!address)
    {
        qDebug("Missing address");
        return false;
    }
    result = inet_pton(AF_INET, address, &(sa.sin_addr));
    return (result != 0);
#endif
}

bool is_valid_user_password(const char *password)
{
#ifdef _WIN32
    return true;
#else
    bool is_valid = false;
    // check input
    if (!password)
    {
        qDebug("Missing password");
        return is_valid;
    }
    string input = password;
    bool upper_case = false;
    bool lower_case = false;
    bool number_case = false;
    bool special_char = false;
    regex upper_case_expression{ "[A-Z]+" };
    regex lower_case_expression{ "[a-z]+" };
    regex number_expression{ "[0-9]+" };
    regex special_char_expression{ "[!#$%&=?@\"]+"};
    // at least 7 digits 
    if (input.length() < MIN_PASSWORD_LENGTH) {
        return is_valid;
    }
    upper_case = regex_search(input, upper_case_expression);
    lower_case = regex_search(input, lower_case_expression);
    number_case = regex_search(input, number_expression);
    special_char = regex_search(input, special_char_expression);
    // containing at least 1 capital letter
    is_valid = upper_case && (lower_case || number_case || special_char);
    return is_valid;
#endif
}

bool is_user_password_exists(const char *username)
{
    bool result = true;
    // check input
    if (!username)
    {
        qDebug("Missing username");
        return result;
    }

    const auto ret = execute_cmd_get_single_info(GET_USER_PASSWORD_STATUS_CMD, username);
    if (ret.first.compare(STRING_NO_PASSWORD) == 0)
    {
        result = false;
    }
    return result;
}

bool is_numbers(const char *input)
{
#ifdef _WIN32
    return true;
#else
    // check input
    if (!input)
    {
        qDebug("Missing input");
        return false;
    }
    string value(input);
    string::const_iterator it = value.begin();
    while (it != value.end() && isdigit(*it)) {
        ++it;
    }
    return (!value.empty() && it == value.end());
#endif
}

bool is_hex_color(const char *color)
{
#ifdef _WIN32
    return true;
#else
    // check input
    if (!color)
    {
        qDebug("Missing input");
        return false;
    }
    // ex: 0xffffff
    static const string HEX_PREFIX = "0x";
    static const int HEX_COLOR_LENGTH = 6;
    string value(color);
    string hexValue = value.substr(HEX_PREFIX.length());
    // check start with 0x
    if (value.rfind(HEX_PREFIX, 0) != 0)
    {
        qDebug("color is not start with 0x");
        return false;
    }
    // check length
    if (hexValue.length() != HEX_COLOR_LENGTH)
    {
        qDebug("hex color length is not 6");
        return false;
    }
    // check is hex
    string::const_iterator it = hexValue.begin();
    while (it != hexValue.end() && isxdigit(*it)) {
        ++it;
    }
    return (it == hexValue.end());
#endif
}
