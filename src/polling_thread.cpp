// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include <QVariant>

#include "./include/polling_thread.h"
#include "./include/utility.h"
#include "./include/app_utility.h"
#include "./include/config_utility.h"
#include "./include/network_utility.h"

#include <QDebug>

PollingThread::PollingThread(TightVNCUtility *tightVNCUtility) : m_tightVNCUtility(tightVNCUtility) {
    this->m_timeout = 0;
    this->m_type = PollingType::VNC_SERVER;
    this->m_networkUtil = nullptr;
}

PollingThread::PollingThread(const char* ethernet, int timeout, INetworkUtility *networkUtil) : m_ethernet(ethernet), m_timeout(timeout), m_networkUtil(networkUtil) {
    this->m_type = PollingType::NETWORK;
    this->m_tightVNCUtility = nullptr;
}

void PollingThread::pollingNetwork() {
    bool isSuccess = false;
    bool isWiredOnline = false;
    int waitTimeout = this->m_timeout;
    auto retIPv4 = m_networkUtil->get_ip_address(this->m_ethernet.c_str(), true);
    std::string ip = retIPv4.first;
    // wait setting get ready
    while (ip.length() == 0 && waitTimeout > 0) {
        sleep(1);
        waitTimeout--;
        // get ip
        retIPv4 = m_networkUtil->get_ip_address(this->m_ethernet.c_str(), true);
        ip = retIPv4.first;

        // check still online
        isWiredOnline = m_networkUtil->is_network_available(this->m_ethernet);
        if (!isWiredOnline) {
            qDebug("%s is not connected!", this->m_ethernet.c_str());
            break;
        }
    }
    if (ip.length() == 0 && waitTimeout == 0) {
        qDebug("Cannot get %s ip address! Timeout!", this->m_ethernet.c_str());
    }
    isSuccess = (ip.length() > 0);
    // trigger signal 
    emit pollingFinishedSignal(isSuccess, isWiredOnline);
}

void PollingThread::pollingVNCServer() {
    ConfigUtility configUtil;
    bool isSuccess = true;
    bool isServerOnline;
    string address = configUtil.get_vnc_server_address();
    string port = configUtil.get_vnc_server_port();
    int pollingPeriod = configUtil.get_vnc_server_polling_period();
    // polling all the time
    while (address.length() > 0 && port.length() > 0) {
        // check connection established
        isServerOnline = is_server_available(address.c_str(), port.c_str());
        if (isServerOnline) {
            qDebug("%s:%s vnc server is available!", address.c_str(), port.c_str());
        } else {
            qDebug("%s:%s vnc server is not available!", address.c_str(), port.c_str());
        }
        // call callback function
        this->m_tightVNCUtility->pollingVNCServerIsReady(isSuccess, isServerOnline);
        // sleep polling period 30 seconds
        sleep(pollingPeriod);
    }
}

void PollingThread::run() {
    switch(this->m_type) {
        case PollingType::NETWORK:
            this->pollingNetwork();
            break;
        case PollingType::VNC_SERVER:
            this->pollingVNCServer();
            break;
        case PollingType::TIME:
            break;
    }
}

WorkerThread::WorkerThread(PBoolFunc pWorkFunction, int delay) : m_pWorkFunction(pWorkFunction), m_delay(delay) {
}

WorkerThread::WorkerThread(PPairFunc pWorkFunction, int delay) : m_pWorkPairFunction(pWorkFunction), m_delay(delay) {
}

void WorkerThread::run() {
    bool result = true;
    string msg;
    qDebug("delay %d", m_delay);
    sleep(m_delay);
    qDebug("start worker");
    if (m_pWorkFunction) {
        result = m_pWorkFunction();
        emit workFinished(result);
    }
    else if (m_pWorkPairFunction) {
        const auto ret = m_pWorkPairFunction();
        msg = ret.first;
        result = ret.second;
        emit workFinishedWithResult(QString::fromStdString(msg), result);
    }
}
