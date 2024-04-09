// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef POLLING_THREAD_H
#define POLLING_THREAD_H

#include <QThread>

class INetworkUtility;
class ConfigUtility;
class TightVNCUtility;

enum class PollingType {
    NETWORK, TIME, VNC_SERVER
};

class PollingThread : public QThread 
{
    Q_OBJECT

public:
    explicit PollingThread(TightVNCUtility *tightVNCUtility);
    PollingThread(const char* ethernet, int timeout, INetworkUtility *networkUtil);
    void run() override;

private:
    void pollingNetwork();
    void pollingVNCServer();

    std::string m_ethernet;
    int m_timeout;
    PollingType m_type;
    INetworkUtility *m_networkUtil;
    TightVNCUtility *m_tightVNCUtility;

signals:
    void pollingFinishedSignal(bool, bool);
};

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    // function pointer
    using PBoolFunc = std::function<bool()>;
    using PPairFunc = std::function<std::pair<std::string, bool>()>;
    WorkerThread(PBoolFunc pWorkFunction, int delay);
    WorkerThread(PPairFunc pWorkFunction, int delay);
    void run() override;

signals:
    void workFinished(bool);
    void workFinishedWithResult(QString, bool);

private:
    PBoolFunc m_pWorkFunction;
    PPairFunc m_pWorkPairFunction;
    int m_delay;
};
#endif // POLLING_THREAD_H
