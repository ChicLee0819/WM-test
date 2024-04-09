// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#ifndef QMLWINDOW_H
#define QMLWINDOW_H

#include <QObject>

#define COMMON_TIMEOUT 5
#ifdef _WIN32
#define LOCK_FILE_NAME "settings.lock"
#else
#define LOCK_FILE_NAME "/tmp/settings.lock"
#endif

class PollingThread;
class WorkerThread;
class ConfigUtility;
class RestoreUtility;
class QFileSystemWatcher;
class IDeviceInfoUtility;
class INetworkUtility;
class IScreenUtility;
class ISystemUtility;
class IStorageUtility;
class ITimeUtility;
class IUpdateUtility;
class IVersionUtility;
class IFTPUtility;

class QMLWindow : public QObject
{
    Q_OBJECT

public:
    explicit QMLWindow(QObject *parent = nullptr);
    ~QMLWindow();
    void initWindow(QObject *rootObject);
    // run these functions in background thread
    std::pair<std::string, bool> bg_applyTimeSetting(QObject *rootObject, ITimeUtility *pTimeUtil, 
        ConfigUtility* pConfigUtil);

private:
    bool m_inPortrait;
    bool m_isLoginEnabled;
    std::map<std::string, bool> m_wizardStatusMap;
    std::map<std::string, bool> m_wizardPageShowedMap;
    std::string m_loginPassword;
    std::string m_loginType;
    std::string m_loginIdentity;
    std::string m_loginAliasName;
    std::string m_appMode;
    std::string m_keyboardLocale;
    int m_pageIndex;
    std::string m_threadParameter;
    QStringList m_timezones;
    RestoreUtility *m_restoreUtility;
    QFileSystemWatcher *m_watcher;
    PollingThread *m_pollingThread;
    WorkerThread *m_workThread;
    ConfigUtility *m_configUtil;
    IDeviceInfoUtility *m_deviceInfoUtil;
    INetworkUtility *m_networkUtil;
    IScreenUtility *m_screenUtil;
    ISystemUtility *m_systemUtil;
    IStorageUtility *m_storageUtil;
    ITimeUtility *m_timeUtil;
    IUpdateUtility *m_updateUtil;
    IVersionUtility *m_versionUtil;
    IFTPUtility *m_ftpUtil;

    QObject *m_rootObject;
    void initGlobalHandler(QObject *rootObject);
    void initGlobalWindow(QObject *rootObject);
    void initSideBarHandler(QObject *rootObject);
    void initCredentialsWindowHandler(QObject *rootObject);
    void initWizardNetworkWindowHandler(QObject *rootObject);
    void initWizardTimeWindowHandler(QObject *rootObject, QStringList &timezones);
    void initWizardScreenWindowHandler(QObject *rootObject);
    void initWizardStartupWindowHandler(QObject *rootObject);
    void initScreenWindowValue(QObject *rootObject);
    void initScreenWindowHandler(QObject *rootObject);
    void initNetworkWindowValue(QObject *rootObject);
    void initNetworkWindowWired1Value(QObject *rootObject);
    void initNetworkWindowWired2Value(QObject *rootObject);
    void initNetworkWindowFirewallValue(QObject *rootObject);
    void initNetworkWindowHandler(QObject *rootObject);
    void initTimeWindowValue(QObject *rootObject, QStringList &timezones);
    void initTimeWindowHandler(QObject *rootObject);
    void initStorageWindowValue(QObject *rootObject);
    void initSystemWindowValue(QObject *rootObject);
    void initSystemWindowHandler(QObject *rootObject);
    void initSecurityWindowValue(QObject *rootObject);
    void initSecurityWindowHandler(QObject *rootObject);
    void initFTPWindowValue(QObject *rootObject);
    void initFTPWindowHandler(QObject *rootObject);
    void initLogoWindowHandler(QObject *rootObject);
    void initUpdateWindowHandler(QObject *rootObject);
    void initUpdateWindowValue(QObject *rootObject);
    void initOperateWindowHandler(QObject *rootObject);
    void initOperateWindowValue(QObject *rootObject);
    void initPasswordWindowHandler(QObject *rootObject);
    void initPasswordWindowValue(QObject *rootObject);
    void initAboutWindowHandler(QObject *rootObject);
    void initAboutWindowValue(QObject *rootObject);
    void initLoginFlow(QObject *rootObject);
    void initTimezones(QStringList &timezones);
    void startWorker();

    void waitNetworkSettingIsReady(QObject *rootObject, const char* ethernet);
    void waitNetworkIPIsReady(QObject *rootObject, const char* ethernet);
    void applyNetworkSetting(QObject *rootObject, const char* ethernet);
    void applyNetworkFirewallSetting(QObject *rootObject);
    void applyTimeSetting(QObject *rootObject);
    void applyScreenSetting(QObject *rootObject);
    void applySystemStartupSetting(QObject *rootObject);
    void applySystemGeneralSetting(QObject *rootObject);
    void applySystemCOMSetting(QObject *rootObject);
    void applySecuritySetting(QObject *rootObject);
    void applyLogoSetting(QObject *rootObject);
    void applyPasswordSetting(QObject *rootObject);
    bool applyCredentialsSetting(QObject *rootObject);
    bool applyUserCredentialsSetting(QObject *rootObject, const char *username);
    bool applyWizardNetworkSetting(QObject *rootObject);
    bool applyWizardEthernetNetworkSetting(QObject *rootObject, const char* ethernet);
    bool applyWizardTimeSetting(QObject *rootObject);
    bool applyWizardScreenSetting(QObject *rootObject);
    bool applyWizardStartupSetting(QObject *rootObject);
    bool applyWizard(QObject *rootObject);
    bool checkCredentialsRequirement(QObject *rootObject);
    bool checkCredentialsFields(QObject *rootObject, const char *username);
    bool checkWizardEthernetNetworkFields(QObject *rootObject, const char *ethernet);
    void moveToNextPage(QObject *rootObject, const std::string &currentPageName);
    bool downloadFTPFile(QObject *rootObject);
    void startNetworkMonitor();
    void stopNetworkMonitor();

    // message box dialog
    void showMessageDialog(QObject *rootObject, bool isSuccess, std::string *customMessage, int handlerIndex);
    void showQuestionDialog(QObject *rootObject, std::string message, int handlerIndex);
    // loading indicator
    void showLoadingIndicator(QObject *rootObject, bool isShow);
    // login dialog
    void showLoginDialog(QObject *rootObject, bool isLogin);

private slots:
    void pollingNetworkSettingIsReady(bool isSuccess, bool isWiredOnline);
    void pollingNetworkIPIsReady(bool isSuccess, bool isWiredOnline);
    void ipMonitorFileChangedEvent(const QString & path);
    void importConfigIsFinished(QString customMessage, bool isSuccess);
    void downloadIsFinished(bool isSuccess);
    void applyTimeSettingIsFinished(QString customMessage, bool isSuccess);

public slots:
    // sidebar handler
    void on_screen_toggled();
    void on_network_toggled();
    void on_time_toggled();
    void on_storage_toggled();
    void on_system_toggled();
    void on_security_toggled();
    void on_ftp_toggled();
    void on_logo_toggled();
    void on_update_toggled();
    void on_operate_toggled();
    void on_password_toggled();
    void on_about_toggled();
    // network window handler
    void on_networkWindow_swipeView_changed();
    void on_networkWindow_applyButton1_clicked();
    void on_networkWindow_applyButton2_clicked();
    void on_networkWindow_firewallApplyButton_clicked();
    // credentials window handler
    void on_credentialsWindow_nextButton_clicked();
    // wizard window handler
    void on_wizardNetworkWindow_nextButton1_clicked();
    void on_wizardNetworkWindow_nextButton2_clicked();
    void on_wizardTimeWindow_nextButton_clicked();
    void on_wizardScreenWindow_nextButton_clicked();
    void on_wizardStartupWindow_nextButton_clicked();
    // screen window handler
    void on_screenWindow_brightChanged_signal(int value);
    void on_screenWindow_applyButton_clicked();
    void on_screenWindow_questionDialog_okButton_clicked();
    // system window handler
    void on_systemWindow_swipeView_changed();
    void on_systemWindow_startupApplyButton_clicked();
    void on_systemWindow_generalApplyButton_clicked();
    void on_systemWindow_comApplyButton_clicked();
    void on_systemWindow_questionDialog_readonly_okButton_clicked();
    void on_systemWindow_questionDialog_user_login_okButton_clicked();
    // security window handler
    void on_securityWindow_applyButton_clicked();
    // time window handler
    void on_timeWindow_applyButton_clicked();
    // ftp window handler
    void on_ftpWindow_downloadButton_clicked();
    // logo window handler
    void on_logoWindow_applyButton_clicked();
    // password window handler
    void on_passwordWindow_applyButton_clicked();
    // update window handler
    void on_updateWindow_updateButton_clicked();
    void on_updateWindow_questionDialog_okButton_clicked();
    // operate window handler
    void on_operateWindow_importFileDialog_accepted();
    void on_operateWindow_exportFolderDialog_accepted();
    void on_operateWindow_rebootButton_clicked();
    void on_operateWindow_shutdownButton_clicked();
    void on_operateWindow_openTerminalButton_clicked();
    void on_operateWindow_factoryResetButton_clicked();
    void on_operateWindow_exportScreenshotsButton_clicked();
    void on_operateWindow_questionDialog_reboot_okButton_clicked();
    void on_operateWindow_questionDialog_shutdown_okButton_clicked();
    void on_operateWindow_questionDialog_deleteScreenshots_okButton_clicked();
    void on_operateWindow_questionDialog_factoryReset_okButton_clicked();
    // about window handler
    void on_aboutWindow_licenseButton_clicked();
    // login handler
    void on_loginDialog_okButton_clicked();
    void on_loginDialog_cancelButton_clicked();
    // dialog handler
    void on_questionDialog_cancelButton_clicked();
    void on_messageDialog_button_clicked();
    void on_messageDialog_button_clicked_relogin();

signals:
    void closeWindow();
};
#endif // QMLWINDOW_H
