// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef APP_UTILITY_H
#define APP_UTILITY_H

#include <string>

#define MAX_UNAVAILABLE_LIMIT 3

using namespace std;

class PollingThread;

class IAppUtility {
public:
    virtual ~IAppUtility() {}
    virtual pair<string, int> start() = 0;
    virtual pair<string, int> stop() = 0;
};

class TightVNCUtility: public IAppUtility
{
public:
    TightVNCUtility();
    ~TightVNCUtility();
    void start_monitoring();
    bool is_configuration_ready();
    pair<string, int> start() override;
    pair<string, int> stop() override;
    void pollingVNCServerIsReady(bool isSuccess, bool isServerOnline);

private:
    bool m_is_started;
    bool m_is_server_online;
    int m_unavailable_count;
    PollingThread *m_pollingThread;
};

#endif // APP_UTILITY_H
