// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef PAM_UTILITY_H
#define PAM_UTILITY_H

#include <cstdarg>
#include <string>
#include <vector>

#define LOGIN_TYPE_PASSWORD "password"
#define LOGIN_TYPE_SYS_USER "sys_user"

std::pair<std::string, int> sys_auth_user(const char *username, const char *password);
#endif // PAM_UTILITY_H
