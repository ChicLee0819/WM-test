// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

#include "./include/qmlwindow.h"
#include "./include/utility.h"
#include "./include/log_utility.h"
#include "./include/config_utility.h"
#include "./include/network_utility.h"
#include "./include/screen_utility.h"
#include "./include/system_utility.h"
#include "./include/storage_utility.h"
#include "./include/time_utility.h"
#include "./include/startup_utility.h"
#include "./include/update_utility.h"
#include "./include/info_utility.h"
#include "./include/version_utility.h"
#include "./include/ftp_utility.h"
#include "./include/restore_utility.h"
#include "./include/pam_utility.h"
#include "./include/polling_thread.h"

#include <QVariant>
#include <QFileSystemWatcher>
#include <QQuickItem>
#include <QMessageBox>
#include <QScreen>
#include <QGuiApplication>
#include <QWindow>
#include <QStringList>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#define NONE_HANDLER_INDEX 0
#define SCREEN_HANDLER_INDEX 1
#define SYSTEM_READONLY_HANDLER_INDEX 2
#define SYSTEM_USER_LOGIN_HANDLER_INDEX 3
#define UPDATE_HANDLER_INDEX 4
#define OPERATE_REBOOT_HANDLER_INDEX 5
#define OPERATE_SHUTDOWN_HANDLER_INDEX 6
#define OPERATE_DELETE_SCREENSHOTS_HANDLER_INDEX 7
#define OPERATE_FACTORY_RESET_HANDLER_INDEX 8
#define SHOW_LOGIN_HANDLER_INDEX 10
#define NEXT_PAGE_HANDLER_INDEX 11

using namespace std;

QMLWindow::QMLWindow(QObject *parent)
    : QObject(parent)
{
    this->m_rootObject = nullptr;
    this->m_watcher = nullptr;
    this->m_pollingThread = nullptr;
    this->m_workThread = nullptr;
    this->m_restoreUtility = new RestoreUtility();
    this->m_configUtil = new ConfigUtility();
    this->m_deviceInfoUtil = new TPCDeviceInfoUtility();
    this->m_networkUtil = new TPCNetworkUtility();
    this->m_screenUtil = new TPCScreenUtility();
    this->m_systemUtil = new TPCSystemUtility();
    this->m_storageUtil = new TPCStorageUtility();
    this->m_timeUtil = new TPCTimeUtility();
    this->m_updateUtil = new TPCUpdateUtility();
    this->m_versionUtil = new TPCVersionUtility();
    this->m_ftpUtil = new TPCFTPUtility();
    this->m_pageIndex = 0;
    this->m_inPortrait = false;
    this->m_appMode = this->m_configUtil->get_app_mode();
    this->m_isLoginEnabled = this->m_configUtil->get_login_enable();
    this->m_loginPassword = this->m_configUtil->get_login_password();
    this->m_loginType = this->m_configUtil->get_login_type();
    this->m_loginIdentity = ROOT_USER;
    this->m_keyboardLocale = this->m_configUtil->get_keyboard_locale();
    this->m_wizardStatusMap = {
        {WIZARD_CREDENTIALS, false},
        {WIZARD_NETWORK, false},
        {WIZARD_TIME, false},
        {WIZARD_SCREEN, false},
        {WIZARD_STARTUP, false}
    };
    this->m_wizardPageShowedMap = {
        {WIZARD_CREDENTIALS, false},
        {WIZARD_NETWORK, false},
        {WIZARD_TIME, false},
        {WIZARD_SCREEN, false},
        {WIZARD_STARTUP, false}
    };
    // cleanup update files
    this->m_updateUtil->cleanup_files();
}

QMLWindow::~QMLWindow()
{
    this->stopNetworkMonitor();

    delete this->m_restoreUtility;
    delete this->m_configUtil;
    delete this->m_deviceInfoUtil;
    delete this->m_networkUtil;
    delete this->m_screenUtil;
    delete this->m_systemUtil;
    delete this->m_storageUtil;
    delete this->m_timeUtil;
    delete this->m_updateUtil;
    delete this->m_versionUtil;
    delete this->m_ftpUtil;
}

void QMLWindow::initWindow(QObject *rootObject)
{
    this->m_rootObject = rootObject;
    this->initGlobalHandler(rootObject);
    this->initGlobalWindow(rootObject);
    this->initSideBarHandler(rootObject);
    this->initScreenWindowHandler(rootObject);
    this->initNetworkWindowHandler(rootObject);
    this->initTimeWindowHandler(rootObject);
    this->initSystemWindowHandler(rootObject);
    this->initSecurityWindowHandler(rootObject);
    this->initFTPWindowHandler(rootObject);
    this->initLogoWindowHandler(rootObject);
    this->initUpdateWindowHandler(rootObject);
    this->initOperateWindowHandler(rootObject);
    this->initPasswordWindowHandler(rootObject);
    this->initAboutWindowHandler(rootObject);

    if (this->m_appMode.compare(APP_MODE_INIT) == 0)
    {
        this->initCredentialsWindowHandler(rootObject);
        this->initWizardNetworkWindowHandler(rootObject);
        this->initWizardTimeWindowHandler(rootObject, this->m_timezones);
        this->initWizardScreenWindowHandler(rootObject);
        this->initWizardStartupWindowHandler(rootObject);
    } 
    else
    {
        // first page initialization
        this->initScreenWindowValue(rootObject);
        // login flow
        this->initLoginFlow(rootObject);
    }
}

void QMLWindow::initTimezones(QStringList &timezones)
{
    // initialize timezone model when first time
    if (timezones.size() == 0)
    {
        const auto retTimezones = this->m_timeUtil->get_timezones();
        for (auto &itr : retTimezones.first)
        {
            timezones << itr.c_str();
        }
    }
}

void QMLWindow::initLoginFlow(QObject *rootObject)
{
    // show login if enable
    if (this->m_isLoginEnabled)
    {
        bool isLogin = true;
        // force to setup password
        if ((this->m_loginType.compare(LOGIN_TYPE_PASSWORD) == 0 || this->m_loginType.empty()) &&
            this->m_loginPassword.empty()) {
            isLogin = false;
        }
        this->showLoginDialog(rootObject, isLogin);
    }
    else 
    {
        this->startWorker();
    }
}

void QMLWindow::startWorker()
{
    this->startNetworkMonitor();
}

void QMLWindow::initGlobalHandler(QObject *rootObject)
{
    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    this->m_inPortrait = screenGeometry.width() < screenGeometry.height();
    qDebug("width:%d height:%d", screenGeometry.width(), screenGeometry.height());

    QObject *msgbox = rootObject->findChild<QObject *>("questionPopup");
    QObject *questionPopupCancelButton = msgbox->findChild<QObject *>("questionPopupCancelButton");
    QObject::connect(questionPopupCancelButton, SIGNAL(clicked()),
                     this, SLOT(on_questionDialog_cancelButton_clicked()));
}

void QMLWindow::initGlobalWindow(QObject *rootObject)
{
    QWindow *window = qobject_cast<QWindow *>(rootObject);
    QObject *mainPage = rootObject->findChild<QObject *>("mainPage");
    QObject *sideBar = rootObject->findChild<QObject *>("sideBar");
    QObject *drawerSideBar = rootObject->findChild<QObject *>("drawerSideBar");
    // use drawer side bar in portrait mode
    // NOTE: separate two types of side bar because drawer is on top of virtual keyboard
    // drawer would overlap virtual keyboard when both are showed
    if (this->m_inPortrait) {
        sideBar = drawerSideBar;
    }
    QObject *credentialsButton = sideBar->findChild<QObject *>("credentialsButton");
    QObject *wiznetworkButton = sideBar->findChild<QObject *>("wiznetworkButton");
    QObject *wiztimeButton = sideBar->findChild<QObject *>("wiztimeButton");
    QObject *wizscreenButton = sideBar->findChild<QObject *>("wizscreenButton");
    QObject *wizstartupButton = sideBar->findChild<QObject *>("wizstartupButton");
    QObject *screenButton = sideBar->findChild<QObject *>("screenButton");
    QObject *networkButton = sideBar->findChild<QObject *>("networkButton");
    QObject *timeButton = sideBar->findChild<QObject *>("timeButton");
    QObject *storageButton = sideBar->findChild<QObject *>("storageButton");
    QObject *systemButton = sideBar->findChild<QObject *>("systemButton");
    QObject *securityButton = sideBar->findChild<QObject *>("securityButton");
    QObject *ftpButton = sideBar->findChild<QObject *>("ftpButton");
    QObject *logoButton = sideBar->findChild<QObject *>("logoButton");
    QObject *updateButton = sideBar->findChild<QObject *>("updateButton");
    QObject *operateButton = sideBar->findChild<QObject *>("operateButton");
    QObject *passwordButton = sideBar->findChild<QObject *>("passwordButton");
    QObject *aboutButton = sideBar->findChild<QObject *>("aboutButton");
    QObject *exitButton = sideBar->findChild<QObject *>("exitButton");
    bool credentialsPageShowed = this->m_configUtil->get_credentials_page_is_showed();
    bool wiznetworkPageShowed = this->m_configUtil->get_wizard_network_page_is_showed();
    bool wiztimePageShowed = this->m_configUtil->get_wizard_time_page_is_showed();
    bool wizscreenPageShowed = this->m_configUtil->get_wizard_screen_page_is_showed();
    bool wizstartupPageShowed = this->m_configUtil->get_wizard_startup_page_is_showed();
    bool screenPageShowed = this->m_configUtil->get_screen_page_is_showed();
    bool networkPageShowed = this->m_configUtil->get_network_page_is_showed();
    bool timePageShowed = this->m_configUtil->get_time_page_is_showed();
    bool storagePageShowed = this->m_configUtil->get_storage_page_is_showed();
    bool systemPageShowed = this->m_configUtil->get_system_page_is_showed();
    bool securityPageShowed = this->m_configUtil->get_security_page_is_showed();
    bool ftpPageShowed = this->m_configUtil->get_ftp_page_is_showed();
    bool logoPageShowed = this->m_configUtil->get_logo_page_is_showed();
    bool updatePageShowed = this->m_configUtil->get_update_page_is_showed();
    bool operatePageShowed = this->m_configUtil->get_operate_page_is_showed();
    bool passwordPageShowed = this->m_configUtil->get_password_page_is_showed();
    bool aboutPageShowed = this->m_configUtil->get_about_page_is_showed();
    bool exitPageShowed = this->m_configUtil->get_exit_page_is_showed();
    const char* backupFile = ConfigUtility::get_backup_config_path();
    if (this->m_appMode.compare(APP_MODE_INIT) == 0)
    {
        // credentials page is first page
        // so it should always be visible
        credentialsPageShowed = true;
        // hide wizard pages if backup config is exist
        if (is_file_exist(backupFile)) {
            wiznetworkPageShowed = false;
            wiztimePageShowed = false;
            wizscreenPageShowed = false;
            wizstartupPageShowed = false;
        }
        // do not show exit for wizard
        // wizard is closed by last page be applied
        exitPageShowed = false;
        // show first page
        QMetaObject::invokeMethod(sideBar, "initModeUI");
        // show maximized for top bar visible
        window->showMaximized();
    }
    else
    {
        // wizard page is only visible on init mode
        credentialsPageShowed = false;
        wiznetworkPageShowed = false;
        wiztimePageShowed = false;
        wizscreenPageShowed = false;
        wizstartupPageShowed = false;
        // screen page is first page
        // so it should always be visible
        screenPageShowed = true;
        // show first page
        QMetaObject::invokeMethod(sideBar, "initAppModeUI");
        // show full screen to hide top bar
        window->showFullScreen();
    }
    this->m_wizardPageShowedMap[WIZARD_CREDENTIALS] = credentialsPageShowed;
    this->m_wizardPageShowedMap[WIZARD_NETWORK] = wiznetworkPageShowed;
    this->m_wizardPageShowedMap[WIZARD_TIME] = wiztimePageShowed;
    this->m_wizardPageShowedMap[WIZARD_SCREEN] = wizscreenPageShowed;
    this->m_wizardPageShowedMap[WIZARD_STARTUP] = wizstartupPageShowed;
    if (!credentialsPageShowed)
    {
        credentialsButton->setProperty("visible", QVariant(credentialsPageShowed));
    }
    if (!wiznetworkPageShowed)
    {
        wiznetworkButton->setProperty("visible", QVariant(wiznetworkPageShowed));
    }
    if (!wiztimePageShowed)
    {
        wiztimeButton->setProperty("visible", QVariant(wiztimePageShowed));
    }
    if (!wizscreenPageShowed)
    {
        wizscreenButton->setProperty("visible", QVariant(wizscreenPageShowed));
    }
    if (!wizstartupPageShowed)
    {
        wizstartupButton->setProperty("visible", QVariant(wizstartupPageShowed));
    }
    if (!screenPageShowed)
    {
        screenButton->setProperty("visible", QVariant(screenPageShowed));
    }
    if (!networkPageShowed)
    {
        networkButton->setProperty("visible", QVariant(networkPageShowed));
    }
    if (!timePageShowed)
    {
        timeButton->setProperty("visible", QVariant(timePageShowed));
    }
    if (!storagePageShowed)
    {
        storageButton->setProperty("visible", QVariant(storagePageShowed));
    }
    if (!systemPageShowed)
    {
        systemButton->setProperty("visible", QVariant(systemPageShowed));
    }
    if (!securityPageShowed)
    {
        securityButton->setProperty("visible", QVariant(securityPageShowed));
    }
    if (!ftpPageShowed)
    {
        ftpButton->setProperty("visible", QVariant(ftpPageShowed));
    }
    if (!logoPageShowed)
    {
        logoButton->setProperty("visible", QVariant(logoPageShowed));
    }
    if (!updatePageShowed)
    {
        updateButton->setProperty("visible", QVariant(updatePageShowed));
    }
    if (!operatePageShowed)
    {
        operateButton->setProperty("visible", QVariant(operatePageShowed));
    }
    if (!passwordPageShowed)
    {
        passwordButton->setProperty("visible", QVariant(passwordPageShowed));
    }
    if (!aboutPageShowed)
    {
        aboutButton->setProperty("visible", QVariant(aboutPageShowed));
    }
    if (!exitPageShowed)
    {
        exitButton->setProperty("visible", QVariant(exitPageShowed));
    }
    QMetaObject::invokeMethod(mainPage, "initAppKeyboardLocale",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(this->m_keyboardLocale))));
}

void QMLWindow::initSideBarHandler(QObject *rootObject)
{
    QObject *sideBar = rootObject->findChild<QObject *>("sideBar");
    QObject *drawerSideBar = rootObject->findChild<QObject *>("drawerSideBar");
    // use drawer side bar in portrait mode
    // NOTE: separate two types of side bar because drawer is on top of virtual keyboard
    // drawer would overlap virtual keyboard when both are showed
    if (this->m_inPortrait) {
        sideBar = drawerSideBar;
    }
    QObject *screenButton = sideBar->findChild<QObject *>("screenButton");
    QObject *networkButton = sideBar->findChild<QObject *>("networkButton");
    QObject *timeButton = sideBar->findChild<QObject *>("timeButton");
    QObject *storageButton = sideBar->findChild<QObject *>("storageButton");
    QObject *systemButton = sideBar->findChild<QObject *>("systemButton");
    QObject *securityButton = sideBar->findChild<QObject *>("securityButton");
    QObject *ftpButton = sideBar->findChild<QObject *>("ftpButton");
    QObject *logoButton = sideBar->findChild<QObject *>("logoButton");
    QObject *updateButton = sideBar->findChild<QObject *>("updateButton");
    QObject *operateButton = sideBar->findChild<QObject *>("operateButton");
    QObject *passwordButton = sideBar->findChild<QObject *>("passwordButton");
    QObject *aboutButton = sideBar->findChild<QObject *>("aboutButton");
    QObject::connect(screenButton, SIGNAL(pressed()),
                     this, SLOT(on_screen_toggled()));
    QObject::connect(networkButton, SIGNAL(pressed()),
                     this, SLOT(on_network_toggled()));
    QObject::connect(timeButton, SIGNAL(pressed()),
                     this, SLOT(on_time_toggled()));
    QObject::connect(storageButton, SIGNAL(pressed()),
                     this, SLOT(on_storage_toggled()));
    QObject::connect(systemButton, SIGNAL(pressed()),
                     this, SLOT(on_system_toggled()));
    QObject::connect(securityButton, SIGNAL(pressed()),
                     this, SLOT(on_security_toggled()));
    QObject::connect(ftpButton, SIGNAL(pressed()),
                     this, SLOT(on_ftp_toggled()));
    QObject::connect(logoButton, SIGNAL(pressed()),
                     this, SLOT(on_logo_toggled()));
    QObject::connect(updateButton, SIGNAL(pressed()),
                     this, SLOT(on_update_toggled()));
    QObject::connect(operateButton, SIGNAL(pressed()),
                     this, SLOT(on_operate_toggled()));
    QObject::connect(passwordButton, SIGNAL(pressed()),
                     this, SLOT(on_password_toggled()));
    QObject::connect(aboutButton, SIGNAL(pressed()),
                     this, SLOT(on_about_toggled()));
}

void QMLWindow::initCredentialsWindowHandler(QObject *rootObject)
{
    bool isRootPasswordRequired = this->m_configUtil->get_root_password_required();
    bool isWestonPasswordRequired = this->m_configUtil->get_weston_password_required();
    bool isWestonPasswordExists = is_user_password_exists(WESTON_USER);
    QObject *credentialsForm = rootObject->findChild<QObject *>("credentialsForm");
    QObject *nextButton = credentialsForm->findChild<QObject *>("nextButton");
    QObject *rootGroup = credentialsForm->findChild<QObject *>("rootGroup");
    QObject *westonGroup = credentialsForm->findChild<QObject *>("westonGroup");
    QObject *westonCurrentPasswordLabel = credentialsForm->findChild<QObject *>("westonCurrentPasswordLabel");
    QObject *westonCurrentPasswordTextField = credentialsForm->findChild<QObject *>("westonCurrentPasswordTextField");
    rootGroup->setProperty("visible", QVariant(isRootPasswordRequired));
    westonGroup->setProperty("visible", QVariant(isWestonPasswordRequired));
    westonCurrentPasswordLabel->setProperty("visible", QVariant(isWestonPasswordExists));
    westonCurrentPasswordTextField->setProperty("visible", QVariant(isWestonPasswordExists));
    QObject::connect(nextButton, SIGNAL(clicked()),
                     this, SLOT(on_credentialsWindow_nextButton_clicked()));
}

void QMLWindow::initWizardNetworkWindowHandler(QObject *rootObject)
{
    QObject *wiznetworkForm = rootObject->findChild<QObject *>("wiznetworkForm");
    QObject *nextButton1 = wiznetworkForm->findChild<QObject *>("wizNetworkPageNextButton1");
    QObject *nextButton2 = wiznetworkForm->findChild<QObject *>("wizNetworkPageNextButton2");
    QObject::connect(nextButton1, SIGNAL(clicked()),
                     this, SLOT(on_wizardNetworkWindow_nextButton1_clicked()));
    QObject::connect(nextButton2, SIGNAL(clicked()),
                     this, SLOT(on_wizardNetworkWindow_nextButton2_clicked()));
}

void QMLWindow::initWizardTimeWindowHandler(QObject *rootObject, QStringList &timezones)
{
    const auto retCurrentTimezone = this->m_timeUtil->get_current_timezone();
    QObject *wiztimeForm = rootObject->findChild<QObject *>("wiztimeForm");
    QObject *nextButton = wiztimeForm->findChild<QObject *>("wizTimePageNextButton");
    QObject::connect(nextButton, SIGNAL(clicked()),
                     this, SLOT(on_wizardTimeWindow_nextButton_clicked()));

    // initialize timezone model when first time
    if (timezones.size() == 0)
    {
        this->initTimezones(timezones);
        QMetaObject::invokeMethod(wiztimeForm, "initTimezoneModel",
                                  Q_ARG(QVariant, QVariant::fromValue(timezones)));
    }
    QMetaObject::invokeMethod(wiztimeForm, "initTimezoneComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(retCurrentTimezone.first))));
    QMetaObject::invokeMethod(wiztimeForm, "initTimeComboBox");
}

void QMLWindow::initWizardScreenWindowHandler(QObject *rootObject)
{
    QObject *wizscreenForm = rootObject->findChild<QObject *>("wizscreenForm");
    QObject *nextButton = wizscreenForm->findChild<QObject *>("wizScreenPageNextButton");
    QObject::connect(nextButton, SIGNAL(clicked()),
                     this, SLOT(on_wizardScreenWindow_nextButton_clicked()));
    QObject::connect(wizscreenForm, SIGNAL(brightChangedSignal(int)),
                     this, SLOT(on_screenWindow_brightChanged_signal(int)));
}

void QMLWindow::initWizardStartupWindowHandler(QObject *rootObject)
{
    QObject *wizstartupForm = rootObject->findChild<QObject *>("wizstartupForm");
    QObject *nextButton = wizstartupForm->findChild<QObject *>("startupPageNextButton");
    QObject::connect(nextButton, SIGNAL(clicked()),
                     this, SLOT(on_wizardStartupWindow_nextButton_clicked()));
}

void QMLWindow::initAboutWindowValue(QObject *rootObject)
{
    QObject *aboutForm = rootObject->findChild<QObject *>("aboutForm");
    QObject *imageVersionLabel = aboutForm->findChild<QObject *>("imageVersionLabel");
    QObject *kernelVersionLabel = aboutForm->findChild<QObject *>("kernelVersionLabel");
    QObject *ubootVersionLabel = aboutForm->findChild<QObject *>("ubootVersionLabel");
    QObject *appVersionLabel = aboutForm->findChild<QObject *>("appVersionLabel");
    QObject *qtVersionLabel = aboutForm->findChild<QObject *>("qtVersionLabel");
    QObject *opensslVersionLabel = aboutForm->findChild<QObject *>("opensslVersionLabel");
    QObject *javaVersionLabel = aboutForm->findChild<QObject *>("javaVersionLabel");
    QObject *chromiumVersionLabel = aboutForm->findChild<QObject *>("chromiumVersionLabel");
    QObject *temperatureLabel = aboutForm->findChild<QObject *>("temperatureLabel");

    const auto retImageVersion = this->m_versionUtil->get_image_version();
    const auto retKernelVersion = this->m_versionUtil->get_kernel_version();
    const auto retUBootVersion = this->m_versionUtil->get_uboot_version();
    const auto retAppVersion = this->m_versionUtil->get_app_version();
    const auto retQtVersion = this->m_versionUtil->get_qt_runtime_version();
    const auto retOpensslVersion = this->m_versionUtil->get_openssl_version();
    const auto retJavaVersion = this->m_versionUtil->get_java_runtime_version();
    const auto retChromiumVersion = this->m_versionUtil->get_chromium_version();
    float temperatureValue = this->m_deviceInfoUtil->get_temperature();
    char buff[BUFF_SIZE] = {0};
    snprintf(buff, BUFF_SIZE, "%.1f°C", temperatureValue);
    string temperatureStr = buff;
    imageVersionLabel->setProperty("text", QVariant(QString::fromStdString(retImageVersion.first)));
    kernelVersionLabel->setProperty("text", QVariant(QString::fromStdString(retKernelVersion.first)));
    ubootVersionLabel->setProperty("text", QVariant(QString::fromStdString(retUBootVersion.first)));
    appVersionLabel->setProperty("text", QVariant(QString::fromStdString(retAppVersion.first)));
    qtVersionLabel->setProperty("text", QVariant(QString::fromStdString(retQtVersion.first)));
    opensslVersionLabel->setProperty("text", QVariant(QString::fromStdString(retOpensslVersion.first)));
    javaVersionLabel->setProperty("text", QVariant(QString::fromStdString(retJavaVersion.first)));
    chromiumVersionLabel->setProperty("text", QVariant(QString::fromStdString(retChromiumVersion.first)));
    temperatureLabel->setProperty("text", QVariant(QString::fromStdString(temperatureStr)));
}

void QMLWindow::initAboutWindowHandler(QObject *rootObject)
{
    QObject *aboutForm = rootObject->findChild<QObject *>("aboutForm");
    QObject *licenseButton = aboutForm->findChild<QObject *>("licenseButton");
    QObject::connect(licenseButton, SIGNAL(clicked()),
                     this, SLOT(on_aboutWindow_licenseButton_clicked()));
}

void QMLWindow::initUpdateWindowValue(QObject *rootObject)
{
    bool isBackupEnabled = this->m_configUtil->get_backup_config_enable();
    bool isBackupUserEnabled = this->m_configUtil->get_backup_user_enable();
    QObject *updateForm = rootObject->findChild<QObject *>("updateForm");
    QObject *backupSettingsSwitch = updateForm->findChild<QObject *>("backupSettingsSwitch");
    QObject *backupUserSwitch = updateForm->findChild<QObject *>("backupUserSwitch");
    backupSettingsSwitch->setProperty("checked", QVariant(isBackupEnabled));
    backupUserSwitch->setProperty("checked", QVariant(isBackupUserEnabled));
}

void QMLWindow::initUpdateWindowHandler(QObject *rootObject)
{
    QObject *updateForm = rootObject->findChild<QObject *>("updateForm");
    QObject *updateButton = updateForm->findChild<QObject *>("updateButton");
    QObject::connect(updateButton, SIGNAL(clicked()),
                     this, SLOT(on_updateWindow_updateButton_clicked()));
}

void QMLWindow::initScreenWindowValue(QObject *rootObject)
{
    int brightValue = this->m_screenUtil->get_brightness();
    int idleTimeMinute = this->m_screenUtil->get_screensaver_idle_time() / MINUTE_OF_SECONDS;
    bool isHideCursor = this->m_screenUtil->get_hide_cursor();
    string topBarPosition = this->m_screenUtil->get_top_bar_position();
    string rotateScreen = this->m_screenUtil->get_rotate_screen();
    string gestureType = this->m_screenUtil->get_gesture_type();
    bool isGestureEnabled = this->m_configUtil->get_gesture_enable();
    bool isGestureSwipeDownEnabled = this->m_screenUtil->get_2_finger_gesture_swipe_down_enabled();
    bool isGestureSwipeUpEnabled = this->m_screenUtil->get_2_finger_gesture_swipe_up_enabled();
    bool isGestureSwipeRightEnabled = this->m_screenUtil->get_2_finger_gesture_swipe_right_enabled();
    QObject *screenForm = rootObject->findChild<QObject *>("screenForm");
    QObject *brightSlider = screenForm->findChild<QObject *>("brightSlider");
    QObject *brightSpinbox = screenForm->findChild<QObject *>("brightSpinbox");
    QObject *screensaverSwitch = screenForm->findChild<QObject *>("screensaverSwitch");
    QObject *screensaverSpinbox = screenForm->findChild<QObject *>("screensaverSpinbox");
    QObject *hidecursorSwitch = screenForm->findChild<QObject *>("hidecursorSwitch");
    QObject *gestureSwitch = screenForm->findChild<QObject *>("gestureSwitch");
    QObject *gestureLabel = screenForm->findChild<QObject *>("gestureLabel");
    QObject *gestureGroup = screenForm->findChild<QObject *>("gestureGroup");
    QObject *gestureSwipeDownSwitch = screenForm->findChild<QObject *>("gestureSwipeDownSwitch");
    QObject *gestureSwipeUpSwitch = screenForm->findChild<QObject *>("gestureSwipeUpSwitch");
    QObject *gestureSwipeRightSwitch = screenForm->findChild<QObject *>("gestureSwipeRightSwitch");

    brightSlider->setProperty("value", QVariant(brightValue));
    brightSpinbox->setProperty("value", QVariant(brightValue));
    if (idleTimeMinute > 0)
    {
        screensaverSwitch->setProperty("checked", QVariant(true));
        screensaverSpinbox->setProperty("enabled", QVariant(true));
        screensaverSpinbox->setProperty("value", QVariant(idleTimeMinute));
    }
    else
    {
        screensaverSwitch->setProperty("checked", QVariant(false));
        screensaverSpinbox->setProperty("enabled", QVariant(false));
    }
    hidecursorSwitch->setProperty("checked", QVariant(isHideCursor));
    gestureSwitch->setProperty("checked", QVariant(isGestureEnabled));
    gestureSwipeDownSwitch->setProperty("checked", QVariant(isGestureSwipeDownEnabled));
    gestureSwipeUpSwitch->setProperty("checked", QVariant(isGestureSwipeUpEnabled));
    gestureSwipeRightSwitch->setProperty("checked", QVariant(isGestureSwipeRightEnabled));

    gestureSwipeDownSwitch->setProperty("enabled", QVariant(isGestureEnabled));
    gestureSwipeUpSwitch->setProperty("enabled", QVariant(isGestureEnabled));
    gestureSwipeRightSwitch->setProperty("enabled", QVariant(isGestureEnabled));

    gestureSwitch->setProperty("visible", QVariant((gestureType.compare(GESTURE_TYPE_GENERAL) == 0)));
    gestureLabel->setProperty("visible", QVariant((gestureType.compare(GESTURE_TYPE_GENERAL) == 0)));
    gestureGroup->setProperty("visible", QVariant((gestureType.compare(GESTURE_TYPE_GENERAL) == 0)));

    QMetaObject::invokeMethod(screenForm, "initTopBarPositionComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(topBarPosition))));
    QMetaObject::invokeMethod(screenForm, "initRotateScreenComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(rotateScreen))));
}

void QMLWindow::initScreenWindowHandler(QObject *rootObject)
{
    QObject *screenForm = rootObject->findChild<QObject *>("screenForm");
    QObject *applyButton = screenForm->findChild<QObject *>("applyButton");
    QObject::connect(applyButton, SIGNAL(clicked()),
                     this, SLOT(on_screenWindow_applyButton_clicked()));
    QObject::connect(screenForm, SIGNAL(brightChangedSignal(int)),
                     this, SLOT(on_screenWindow_brightChanged_signal(int)));
}

void QMLWindow::initFTPWindowValue(QObject *rootObject)
{
    string address = this->m_configUtil->get_ftp_server_address();
    string port = this->m_configUtil->get_ftp_server_port();
    string username = this->m_configUtil->get_ftp_server_username();
    string password = this->m_configUtil->get_ftp_server_password();
    string remotePath = this->m_configUtil->get_ftp_server_remote_path();
    string localPath = this->m_configUtil->get_ftp_server_local_path();
    QObject *ftpForm = rootObject->findChild<QObject *>("ftpForm");
    QObject *serverTextField = ftpForm->findChild<QObject *>("serverTextField");
    QObject *portTextField = ftpForm->findChild<QObject *>("portTextField");
    QObject *usernameTextField = ftpForm->findChild<QObject *>("usernameTextField");
    QObject *passwordTextField = ftpForm->findChild<QObject *>("passwordTextField");
    QObject *remotePathTextField = ftpForm->findChild<QObject *>("remotePathTextField");
    QObject *localPathTextField = ftpForm->findChild<QObject *>("localPathTextField");

    serverTextField->setProperty("text", QVariant(QString::fromStdString(address)));
    portTextField->setProperty("text", QVariant(QString::fromStdString(port)));
    usernameTextField->setProperty("text", QVariant(QString::fromStdString(username)));
    passwordTextField->setProperty("text", QVariant(QString::fromStdString(password)));
    remotePathTextField->setProperty("text", QVariant(QString::fromStdString(remotePath)));
    localPathTextField->setProperty("text", QVariant(QString::fromStdString(localPath)));
}

void QMLWindow::initFTPWindowHandler(QObject *rootObject)
{
    QObject *ftpForm = rootObject->findChild<QObject *>("ftpForm");
    QObject *downloadButton = ftpForm->findChild<QObject *>("downloadButton");
    QObject::connect(downloadButton, SIGNAL(clicked()),
                     this, SLOT(on_ftpWindow_downloadButton_clicked()));
}

void QMLWindow::initLogoWindowHandler(QObject *rootObject)
{
    QObject *logoForm = rootObject->findChild<QObject *>("logoForm");
    QObject *applyButton = logoForm->findChild<QObject *>("applyButton");
    QObject::connect(applyButton, SIGNAL(clicked()),
                     this, SLOT(on_logoWindow_applyButton_clicked()));
}

void QMLWindow::initStorageWindowValue(QObject *rootObject)
{
    QObject *storageForm = rootObject->findChild<QObject *>("storageForm");
    QObject *emmcSizeLabel = storageForm->findChild<QObject *>("emmcSizeLabel");
    QObject *sdSizeLabel = storageForm->findChild<QObject *>("sdSizeLabel");
    // clear old data
    QMetaObject::invokeMethod(storageForm, "clearPratitionModelList");
    QMetaObject::invokeMethod(storageForm, "reset");

    // set emmc data
    const auto reteMMCSize = this->m_storageUtil->get_emmc_size();
    if (!reteMMCSize.first.empty())
    {
        emmcSizeLabel->setProperty("text", QVariant(QString::fromStdString(reteMMCSize.first)));
        const auto reteMMCParts = this->m_storageUtil->get_emmc_parts();
        for (int i = 0; i < (int)reteMMCParts.first.size(); i++)
        {
            BlockDeviceData retPart = reteMMCParts.first.at(i);
            QMetaObject::invokeMethod(storageForm, "addToEMMCPratitionModelList",
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getLabel()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getMountPoint()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getUUID()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getFSType()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getSize()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getUsedPercent()))));
        }
    }
    else
    {
        emmcSizeLabel->setProperty("text", QVariant(QString::fromStdString("0G")));
    }
    // init partition UI
    QMetaObject::invokeMethod(storageForm, "initEMMCStoragePratitionUI");

    // set sd data
    const auto retSDSize = this->m_storageUtil->get_sd_card_size();
    if (!retSDSize.first.empty())
    {
        sdSizeLabel->setProperty("text", QVariant(QString::fromStdString(retSDSize.first)));
        const auto retSDParts = this->m_storageUtil->get_sd_card_parts();
        for (int i = 0; i < (int)retSDParts.first.size(); i++)
        {
            BlockDeviceData retPart = retSDParts.first.at(i);
            QMetaObject::invokeMethod(storageForm, "addToSDPratitionModelList",
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getLabel()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getMountPoint()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getUUID()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getFSType()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getSize()))),
                                      Q_ARG(QVariant, QVariant(QString::fromStdString(retPart.getUsedPercent()))));
        }
    }
    else
    {
        sdSizeLabel->setProperty("text", QVariant(QString::fromStdString("0G")));
    }

    // init partition UI
    QMetaObject::invokeMethod(storageForm, "initSDStoragePratitionUI");
}

void QMLWindow::initNetworkWindowValue(QObject *rootObject)
{
    const static int WIRED1_INDEX = 0;
    const static int WIRED2_INDEX = 1;
    const static int FIREWALL_INDEX = 2;
    QObject *networkForm = rootObject->findChild<QObject *>("networkForm");
    QObject *networkSwipeView = networkForm->findChild<QObject *>("networkSwipeView");
    int currentIndex = networkSwipeView->property("currentIndex").toInt();
    if (currentIndex == WIRED1_INDEX)
        initNetworkWindowWired1Value(rootObject);
    else if (currentIndex == WIRED2_INDEX)
        initNetworkWindowWired2Value(rootObject);
    else if (currentIndex == FIREWALL_INDEX)
        initNetworkWindowFirewallValue(rootObject);
}

void QMLWindow::initNetworkWindowWired1Value(QObject *rootObject)
{
    const auto reteth0 = this->m_networkUtil->get_ethernet_status(ETHERNET_0);
    bool wired1_is_online = reteth0.second;
    const auto reteth0Mac = this->m_networkUtil->get_ethernet_mac_address(ETHERNET_0);
    QObject *networkForm = rootObject->findChild<QObject *>("networkForm");
    // page wired 1
    QObject *macAddressLabel = networkForm->findChild<QObject *>("macAddressLabel1");
    QObject *ipAddressLabel = networkForm->findChild<QObject *>("ipAddressLabel1");
    QObject *defaultGatewayLabel = networkForm->findChild<QObject *>("defaultGatewayLabel1");
    QObject *dns1Label = networkForm->findChild<QObject *>("dns1Label1");
    QObject *dhcpSwitch = networkForm->findChild<QObject *>("dhcpSwitch1");
    QObject *ipTextField = networkForm->findChild<QObject *>("ipTextField1");
    QObject *networkMaskTextField = networkForm->findChild<QObject *>("networkMaskTextField1");
    QObject *defaultGatewayTextField = networkForm->findChild<QObject *>("defaultGatewayTextField1");
    QObject *dns1TextField = networkForm->findChild<QObject *>("dns1TextField1");
    QObject *dns2TextField = networkForm->findChild<QObject *>("dns2TextField1");
    // clear values
    dhcpSwitch->setProperty("checked", QVariant(false));
    ipTextField->setProperty("text", QVariant(""));
    networkMaskTextField->setProperty("text", QVariant(""));
    defaultGatewayTextField->setProperty("text", QVariant(""));
    dns1TextField->setProperty("text", QVariant(""));
    dns2TextField->setProperty("text", QVariant(""));

    // set required properties
    QMetaObject::invokeMethod(networkForm, "setWired1LinkStatus",
                              Q_ARG(QVariant, QVariant::fromValue(wired1_is_online)));
    macAddressLabel->setProperty("text", QVariant(QString::fromStdString(reteth0Mac.first)));

    string method, ip, networkMask, defaultGateway;
    vector<string> nameservers;
    // init wired 1
    if (wired1_is_online)
    {
        // get value from connmanctl
        const auto retMode = this->m_networkUtil->get_network_mode(ETHERNET_0, true);
        const auto retIPv4 = this->m_networkUtil->get_ip_address(ETHERNET_0, true);
        const auto retMask = this->m_networkUtil->get_network_mask(ETHERNET_0, true);
        const auto retGateway = this->m_networkUtil->get_default_gateway(ETHERNET_0, true);
        const auto retNameServers = this->m_networkUtil->get_network_nameservers(ETHERNET_0);

        method = retMode.first;
        ip = retIPv4.first;
        networkMask = retMask.first;
        defaultGateway = retGateway.first;
        nameservers = retNameServers.first;
    }
    else
    {
        // get value from config
        method = this->m_configUtil->get_net_method(ETHERNET_0);
        ip = this->m_configUtil->get_net_ip_address(ETHERNET_0);
        networkMask = this->m_configUtil->get_net_subnet_mask(ETHERNET_0);
        defaultGateway = this->m_configUtil->get_net_gateway(ETHERNET_0);
        nameservers = this->m_configUtil->get_net_dns_servers(ETHERNET_0);
    }
    // set wired 1 value
    if (method.compare(MODE_DHCP) == 0)
    {
        dhcpSwitch->setProperty("checked", QVariant(true));
    }
    else
    {
        dhcpSwitch->setProperty("checked", QVariant(false));
    }
    QMetaObject::invokeMethod(dhcpSwitch, "clicked");
    ipAddressLabel->setProperty("text", QVariant(QString::fromStdString(ip)));
    defaultGatewayLabel->setProperty("text", QVariant(QString::fromStdString(defaultGateway)));
    ipTextField->setProperty("text", QVariant(QString::fromStdString(ip)));
    networkMaskTextField->setProperty("text", QVariant(QString::fromStdString(networkMask)));
    defaultGatewayTextField->setProperty("text", QVariant(QString::fromStdString(defaultGateway)));
    if (nameservers.size() > 0)
    {
        dns1Label->setProperty("text", QVariant(QString::fromStdString(nameservers.at(0))));
        dns1TextField->setProperty("text", QVariant(QString::fromStdString(nameservers.at(0))));
    }
    if (nameservers.size() > 1)
    {
        dns2TextField->setProperty("text", QVariant(QString::fromStdString(nameservers.at(1))));
    }
}
void QMLWindow::initNetworkWindowWired2Value(QObject *rootObject)
{
    const auto reteth1 = this->m_networkUtil->get_ethernet_status(ETHERNET_1);
    bool wired2_is_online = reteth1.second;
    const auto reteth1Mac = this->m_networkUtil->get_ethernet_mac_address(ETHERNET_1);
    QObject *networkForm = rootObject->findChild<QObject *>("networkForm");
    // page wired 2
    QObject *macAddressLabel2 = networkForm->findChild<QObject *>("macAddressLabel2");
    QObject *ipAddressLabel2 = networkForm->findChild<QObject *>("ipAddressLabel2");
    QObject *defaultGatewayLabel2 = networkForm->findChild<QObject *>("defaultGatewayLabel2");
    QObject *dns1Label2 = networkForm->findChild<QObject *>("dns1Label2");
    QObject *dhcpSwitch2 = networkForm->findChild<QObject *>("dhcpSwitch2");
    QObject *ipTextField2 = networkForm->findChild<QObject *>("ipTextField2");
    QObject *networkMaskTextField2 = networkForm->findChild<QObject *>("networkMaskTextField2");
    QObject *defaultGatewayTextField2 = networkForm->findChild<QObject *>("defaultGatewayTextField2");
    QObject *dns1TextField2 = networkForm->findChild<QObject *>("dns1TextField2");
    QObject *dns2TextField2 = networkForm->findChild<QObject *>("dns2TextField2");
    // clear values
    dhcpSwitch2->setProperty("checked", QVariant(false));
    ipTextField2->setProperty("text", QVariant(""));
    networkMaskTextField2->setProperty("text", QVariant(""));
    defaultGatewayTextField2->setProperty("text", QVariant(""));
    dns1TextField2->setProperty("text", QVariant(""));
    dns2TextField2->setProperty("text", QVariant(""));

    // set required properties
    QMetaObject::invokeMethod(networkForm, "setWired2LinkStatus",
                              Q_ARG(QVariant, QVariant::fromValue(wired2_is_online)));
    macAddressLabel2->setProperty("text", QVariant(QString::fromStdString(reteth1Mac.first)));

    string method, ip, networkMask, defaultGateway;
    vector<string> nameservers;
    // init wired 2
    if (wired2_is_online)
    {
        // get value from connmanctl
        const auto retMode = this->m_networkUtil->get_network_mode(ETHERNET_1, true);
        const auto retIPv4 = this->m_networkUtil->get_ip_address(ETHERNET_1, true);
        const auto retMask = this->m_networkUtil->get_network_mask(ETHERNET_1, true);
        const auto retGateway = this->m_networkUtil->get_default_gateway(ETHERNET_1, true);
        const auto retNameServers = this->m_networkUtil->get_network_nameservers(ETHERNET_1);

        method = retMode.first;
        ip = retIPv4.first;
        networkMask = retMask.first;
        defaultGateway = retGateway.first;
        nameservers = retNameServers.first;
    }
    else
    {
        // get value from config
        method = this->m_configUtil->get_net_method(ETHERNET_1);
        ip = this->m_configUtil->get_net_ip_address(ETHERNET_1);
        networkMask = this->m_configUtil->get_net_subnet_mask(ETHERNET_1);
        defaultGateway = this->m_configUtil->get_net_gateway(ETHERNET_1);
        nameservers = this->m_configUtil->get_net_dns_servers(ETHERNET_1);
    }
    // set wired 2 value
    if (method.compare(MODE_DHCP) == 0)
    {
        dhcpSwitch2->setProperty("checked", QVariant(true));
    }
    else
    {
        dhcpSwitch2->setProperty("checked", QVariant(false));
    }
    QMetaObject::invokeMethod(dhcpSwitch2, "clicked");
    ipAddressLabel2->setProperty("text", QVariant(QString::fromStdString(ip)));
    defaultGatewayLabel2->setProperty("text", QVariant(QString::fromStdString(defaultGateway)));
    ipTextField2->setProperty("text", QVariant(QString::fromStdString(ip)));
    networkMaskTextField2->setProperty("text", QVariant(QString::fromStdString(networkMask)));
    defaultGatewayTextField2->setProperty("text", QVariant(QString::fromStdString(defaultGateway)));
    if (nameservers.size() > 0)
    {
        dns1Label2->setProperty("text", QVariant(QString::fromStdString(nameservers.at(0))));
        dns1TextField2->setProperty("text", QVariant(QString::fromStdString(nameservers.at(0))));
    }
    if (nameservers.size() > 1)
    {
        dns2TextField2->setProperty("text", QVariant(QString::fromStdString(nameservers.at(1))));
    }
}
void QMLWindow::initNetworkWindowFirewallValue(QObject *rootObject)
{
    const auto retRules = this->m_networkUtil->get_firewall_accept_ports();
    // transform firewall rules to QVariantList for UI
    QVariantList rulelist;
    // transform std::map to QVariantMap
    for (const auto& retRule : retRules.first) {
        QVariantMap qRule;
        string protocol = retRule.find(PROTOCOL_STRING)->second;
        string port = retRule.find(PORT_STRING)->second;
        qRule.insert(QString::fromStdString(PROTOCOL_STRING), QString::fromStdString(protocol));
        qRule.insert(QString::fromStdString(PORT_STRING), QString::fromStdString(port));
        qRule.insert(QString::fromStdString(IS_ALLOWED_STRING), true);
        rulelist.append(qRule);
    }
    QObject *networkForm = rootObject->findChild<QObject *>("networkForm");
    // init firewall rules
    QMetaObject::invokeMethod(networkForm, "initFirewallRulesModel",
                              Q_ARG(QVariant, QVariant(QVariant::fromValue(rulelist))));
}

void QMLWindow::initNetworkWindowHandler(QObject *rootObject)
{
    QObject *networkForm = rootObject->findChild<QObject *>("networkForm");
    QObject *applyButton1 = networkForm->findChild<QObject *>("applyButton1");
    QObject::connect(applyButton1, SIGNAL(clicked()),
                     this, SLOT(on_networkWindow_applyButton1_clicked()));
    QObject *applyButton2 = networkForm->findChild<QObject *>("applyButton2");
    QObject::connect(applyButton2, SIGNAL(clicked()),
                     this, SLOT(on_networkWindow_applyButton2_clicked()));
    QObject *firewallApplyButton = networkForm->findChild<QObject *>("firewallApplyButton");
    QObject::connect(firewallApplyButton, SIGNAL(clicked()),
                     this, SLOT(on_networkWindow_firewallApplyButton_clicked()));
    QObject *networkSwipeView = networkForm->findChild<QObject *>("networkSwipeView");
    QObject::connect(networkSwipeView, SIGNAL(currentIndexChanged()),
                     this, SLOT(on_networkWindow_swipeView_changed()));
                     
}

void QMLWindow::initTimeWindowValue(QObject *rootObject, QStringList &timezones)
{
    const auto retCurrentTimezone = this->m_timeUtil->get_current_timezone();
    const auto retNTPServer = this->m_timeUtil->get_ntp_server();
    const auto retNTPEnabled = this->m_timeUtil->get_ntp_enabled();
    const auto retDate = this->m_timeUtil->get_current_date();
    const auto retTime = this->m_timeUtil->get_current_time();
    QObject *timeForm = rootObject->findChild<QObject *>("timeForm");
    QObject *currentDateText = timeForm->findChild<QObject *>("currentDateText");
    QObject *currentTimeText = timeForm->findChild<QObject *>("currentTimeText");
    QObject *ntpRadioButton = timeForm->findChild<QObject *>("ntpRadioButton");
    QObject *ntpServerTextField = timeForm->findChild<QObject *>("ntpServerTextField");
    QObject *manualRadioButton = timeForm->findChild<QObject *>("manualRadioButton");
    QObject *dateButton = timeForm->findChild<QObject *>("dateButton");
    QObject *hourComboBox = timeForm->findChild<QObject *>("hourComboBox");
    QObject *minuteComboBox = timeForm->findChild<QObject *>("minuteComboBox");
    QObject *secondComboBox = timeForm->findChild<QObject *>("secondComboBox");

    currentDateText->setProperty("text", QVariant(QString::fromStdString(retDate.first)));
    currentTimeText->setProperty("text", QVariant(QString::fromStdString(retTime.first)));
    ntpServerTextField->setProperty("text", QVariant(QString::fromStdString(retNTPServer.first)));
    ntpRadioButton->setProperty("checked", QVariant(retNTPEnabled.first));
    ntpServerTextField->setProperty("enabled", QVariant(retNTPEnabled.first));
    manualRadioButton->setProperty("checked", QVariant(!retNTPEnabled.first));
    dateButton->setProperty("enabled", QVariant(!retNTPEnabled.first));
    hourComboBox->setProperty("enabled", QVariant(!retNTPEnabled.first));
    minuteComboBox->setProperty("enabled", QVariant(!retNTPEnabled.first));
    secondComboBox->setProperty("enabled", QVariant(!retNTPEnabled.first));

    // initialize timezone model when first time
    if (timezones.size() == 0)
    {
        this->initTimezones(timezones);
        QMetaObject::invokeMethod(timeForm, "initTimezoneModel",
                                  Q_ARG(QVariant, QVariant::fromValue(timezones)));
    }

    QMetaObject::invokeMethod(timeForm, "initTimezoneComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(retCurrentTimezone.first))));
    QMetaObject::invokeMethod(timeForm, "initTimeComboBox");
}

void QMLWindow::initTimeWindowHandler(QObject *rootObject)
{
    QObject *timeForm = rootObject->findChild<QObject *>("timeForm");
    QObject *applyButton = timeForm->findChild<QObject *>("applyButton");
    QObject::connect(applyButton, SIGNAL(clicked()),
                     this, SLOT(on_timeWindow_applyButton_clicked()));
}

void QMLWindow::initSystemWindowValue(QObject *rootObject)
{
    const auto retStartup = this->m_configUtil->get_startup();
    int retStaticPageTimeout = this->m_configUtil->get_static_page_timeout();
    const auto retStaticPageUrl = this->m_configUtil->get_static_page_url();
    const auto retStaticPageFilePath = this->m_configUtil->get_static_page_file_path();
    bool retIsAutoRestart = this->m_configUtil->get_startup_auto_restart();
    bool retIsReadonly = this->m_systemUtil->get_readonly_mode();
    const auto retcom1Mode = this->m_configUtil->get_com1_mode();
    const auto retcom1BaudRate = this->m_configUtil->get_com1_baudrate();
    const auto retcom2BaudRate = this->m_configUtil->get_com2_baudrate();
    bool retIsUserLogin = this->m_systemUtil->get_system_user_login_desktop();
    bool retIsEthernetEnable = this->m_configUtil->get_ethernet_enable();
    bool retIsUSBEnable = this->m_systemUtil->get_usb_enable();
    bool retChromiumUseSysVKB = this->m_configUtil->get_chromium_use_sys_virtual_keyboard();
    bool retChromiumUseCustomVKB = this->m_configUtil->get_chromium_use_custom_virtual_keyboard();
    int retWebPagesCount = this->m_configUtil->get_web_pages_count();
    bool retIsRSCronEnable = this->m_configUtil->get_reboot_system_crontab_enabled();
    const auto retCronMode = this->m_configUtil->get_reboot_system_crontab_mode();
    int retMinute = this->m_configUtil->get_reboot_system_crontab_minute();
    int retHour = this->m_configUtil->get_reboot_system_crontab_hour();
    int retDayofweek = this->m_configUtil->get_reboot_system_crontab_dayofweek();
    bool retCOMShowed = this->m_configUtil->get_com_function_is_showed_for_user();
    const auto retVncAddress = this->m_configUtil->get_vnc_server_address();
    const auto retVncPort = this->m_configUtil->get_vnc_server_port();
    const auto retVncPassword = this->m_configUtil->get_vnc_server_password();
    bool retVncViewonly = this->m_configUtil->get_vnc_server_viewonly();
    int retVncImageQuality = this->m_configUtil->get_vnc_server_image_quality();
    bool retVncFullscreen = this->m_configUtil->get_vnc_server_fullscreen();
    bool retVncFitWindow = this->m_configUtil->get_vnc_server_fit_window();
    char vncServer[BUFF_SIZE] = {0};
    if (retVncAddress.length() > 0 && retVncPort.length() > 0)
        snprintf(vncServer, BUFF_SIZE, "%s:%s", retVncAddress.c_str(), retVncPort.c_str());
    else if (retVncAddress.length() > 0)
        snprintf(vncServer, BUFF_SIZE, "%s", retVncAddress.c_str());
    string vncServerStr = vncServer;
    // transform web pages to QVariantList for UI
    QVariantList webPagelist;
    for (int i = 0; i < retWebPagesCount; i++) {
        // transform std::map to QVariantMap
        QVariantMap qWebPage;
        map<string, string> webPage = this->m_configUtil->get_web_page_by_index(i);
        for (const auto& pair : webPage) {
            qWebPage.insert(QString::fromStdString(pair.first), QString::fromStdString(pair.second));
        }
        webPagelist.append(qWebPage);
    }
    QObject *systemForm = rootObject->findChild<QObject *>("systemForm");
    QObject *readonlySwitch = systemForm->findChild<QObject *>("readonlySwitch");
    QObject *timeoutTextField = systemForm->findChild<QObject *>("timeoutTextField");
    QObject *urlTextField = systemForm->findChild<QObject *>("urlTextField");
    QObject *fileTextField = systemForm->findChild<QObject *>("fileTextField");
    QObject *userLoginSwitch = systemForm->findChild<QObject *>("userLoginSwitch");
    QObject *autoRestartSwitch = systemForm->findChild<QObject *>("autoRestartSwitch");
    QObject *rebootSystemCronJobSwitch = systemForm->findChild<QObject *>("rebootSystemCronJobSwitch");
    QObject *ethernetSwitch = systemForm->findChild<QObject *>("ethernetSwitch");
    QObject *usbSwitch = systemForm->findChild<QObject *>("usbSwitch");
    QObject *chromiumSysVKBLabel = systemForm->findChild<QObject *>("chromiumSysVKBLabel");
    QObject *chromiumSysVKBSwitch = systemForm->findChild<QObject *>("chromiumSysVKBSwitch");
    QObject *vncServerTextField = systemForm->findChild<QObject *>("vncServerTextField");
    QObject *vncPasswordTextField = systemForm->findChild<QObject *>("vncPasswordTextField");
    QObject *vncViewonlySwitch = systemForm->findChild<QObject *>("vncViewonlySwitch");
    QObject *vncFullscreenSwitch = systemForm->findChild<QObject *>("vncFullscreenSwitch");
    QObject *vncFitWindowSwitch = systemForm->findChild<QObject *>("vncFitWindowSwitch");
    QObject *vncImageQualitySpinbox = systemForm->findChild<QObject *>("vncImageQualitySpinbox");

    timeoutTextField->setProperty("text", QVariant::fromValue(retStaticPageTimeout));
    urlTextField->setProperty("text", QVariant(QString::fromStdString(retStaticPageUrl)));
    fileTextField->setProperty("text", QVariant(QString::fromStdString(retStaticPageFilePath)));
    readonlySwitch->setProperty("checked", QVariant(retIsReadonly));
    userLoginSwitch->setProperty("checked", QVariant(retIsUserLogin));
    autoRestartSwitch->setProperty("checked", QVariant(retIsAutoRestart));
    rebootSystemCronJobSwitch->setProperty("checked", QVariant(retIsRSCronEnable));
    ethernetSwitch->setProperty("checked", QVariant(retIsEthernetEnable));
    usbSwitch->setProperty("checked", QVariant(retIsUSBEnable));
    chromiumSysVKBSwitch->setProperty("checked", QVariant(retChromiumUseSysVKB));
    chromiumSysVKBSwitch->setProperty("visible", QVariant(!retChromiumUseCustomVKB));
    chromiumSysVKBLabel->setProperty("visible", QVariant(!retChromiumUseCustomVKB));
    vncServerTextField->setProperty("text", QVariant(QString::fromStdString(vncServerStr)));
    vncPasswordTextField->setProperty("text", QVariant(QString::fromStdString(retVncPassword)));
    vncViewonlySwitch->setProperty("checked", QVariant(retVncViewonly));
    vncFullscreenSwitch->setProperty("checked", QVariant(retVncFullscreen));
    vncFitWindowSwitch->setProperty("checked", QVariant(retVncFitWindow));
    vncImageQualitySpinbox->setProperty("value", QVariant(retVncImageQuality));
    // disable page when login with non-root user
    if (this->m_loginType.compare(LOGIN_TYPE_SYS_USER) == 0 && 
        !this->m_loginIdentity.empty() && this->m_loginIdentity.compare(ROOT_USER) != 0) {
        if (!retCOMShowed)
            QMetaObject::invokeMethod(systemForm, "removeCOMPage");
    }
    QMetaObject::invokeMethod(systemForm, "initStartupComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(retStartup))));

    QMetaObject::invokeMethod(systemForm, "initCom1ModeComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(retcom1Mode))));
    QMetaObject::invokeMethod(systemForm, "initCom1BaudRateComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(retcom1BaudRate))));
    QMetaObject::invokeMethod(systemForm, "initCom2BaudRateComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(retcom2BaudRate))));
    QMetaObject::invokeMethod(systemForm, "initWebPageModel",
                              Q_ARG(QVariant, QVariant(QVariant::fromValue(webPagelist))));
    QMetaObject::invokeMethod(systemForm, "initCrontabGroup",
                              Q_ARG(QVariant, QVariant(retIsRSCronEnable)));
    QMetaObject::invokeMethod(systemForm, "initCronModeComboBox",
                              Q_ARG(QVariant, QVariant(QString::fromStdString(retCronMode))));
    QMetaObject::invokeMethod(systemForm, "initCronMinuteComboBox",
                              Q_ARG(QVariant, QVariant(retMinute)));
    QMetaObject::invokeMethod(systemForm, "initCronHourComboBox",
                              Q_ARG(QVariant, QVariant(retHour)));
    QMetaObject::invokeMethod(systemForm, "initCronDayofweekComboBox",
                              Q_ARG(QVariant, QVariant(retDayofweek)));
}

void QMLWindow::initSystemWindowHandler(QObject *rootObject)
{
    QObject *systemForm = rootObject->findChild<QObject *>("systemForm");
    QObject *startupApplyButton = systemForm->findChild<QObject *>("startupPageApplyButton");
    QObject *generalApplyButton = systemForm->findChild<QObject *>("generalPageApplyButton");
    QObject *comApplyButton = systemForm->findChild<QObject *>("comPageApplyButton");
    QObject *systemSwipeView = systemForm->findChild<QObject *>("systemSwipeView");
    QObject::connect(systemSwipeView, SIGNAL(currentIndexChanged()),
                     this, SLOT(on_systemWindow_swipeView_changed()));
    QObject::connect(startupApplyButton, SIGNAL(clicked()),
                     this, SLOT(on_systemWindow_startupApplyButton_clicked()));
    QObject::connect(generalApplyButton, SIGNAL(clicked()),
                     this, SLOT(on_systemWindow_generalApplyButton_clicked()));
    QObject::connect(comApplyButton, SIGNAL(clicked()),
                     this, SLOT(on_systemWindow_comApplyButton_clicked()));
}

void QMLWindow::initSecurityWindowValue(QObject *rootObject)
{
    bool retEnabled = this->m_configUtil->get_login_enable();
    string retPassword = this->m_configUtil->get_login_password();
    QObject *securityForm = rootObject->findChild<QObject *>("securityForm");
    QObject *enableSwitch = securityForm->findChild<QObject *>("enableSwitch");
    QObject *passwordTextField = securityForm->findChild<QObject *>("passwordTextField");
    QObject *showPasswordSwitch = securityForm->findChild<QObject *>("showPasswordSwitch");
    QObject *passwordRadioButton = securityForm->findChild<QObject *>("passwordRadioButton");
    QObject *systemUserRadioButton = securityForm->findChild<QObject *>("systemUserRadioButton");
    QObject *applyButton = securityForm->findChild<QObject *>("applyButton");

    enableSwitch->setProperty("checked", QVariant(retEnabled));
    QMetaObject::invokeMethod(enableSwitch, "clicked");
    passwordTextField->setProperty("text", QVariant(QString::fromStdString(retPassword)));
    if (this->m_loginType.compare(LOGIN_TYPE_SYS_USER) == 0) {
        systemUserRadioButton->setProperty("checked", QVariant(true));
    } else {
        passwordRadioButton->setProperty("checked", QVariant(true));
    }

    // disable operation when login with non-root user
    if (this->m_loginType.compare(LOGIN_TYPE_SYS_USER) == 0 && 
        !this->m_loginIdentity.empty() && this->m_loginIdentity.compare(ROOT_USER) != 0) {
        enableSwitch->setProperty("enabled", QVariant(false));
        passwordRadioButton->setProperty("enabled", QVariant(false));
        systemUserRadioButton->setProperty("enabled", QVariant(false));
        passwordTextField->setProperty("enabled", QVariant(false));
        showPasswordSwitch->setProperty("enabled", QVariant(false));
        applyButton->setProperty("enabled", QVariant(false));
    }
}

void QMLWindow::initSecurityWindowHandler(QObject *rootObject)
{
    QObject *securityForm = rootObject->findChild<QObject *>("securityForm");
    QObject *applyButton = securityForm->findChild<QObject *>("applyButton");
    QObject::connect(applyButton, SIGNAL(clicked()),
                     this, SLOT(on_securityWindow_applyButton_clicked()));
}

void QMLWindow::initOperateWindowValue(QObject *rootObject)
{
    bool exportSettingShowed = this->m_configUtil->get_export_setting_function_is_showed_for_user();
    bool exportScreenshotShowed = this->m_configUtil->get_export_screenshot_function_is_showed_for_user();
    bool importSettingShowed = this->m_configUtil->get_import_setting_function_is_showed_for_user();
    bool rebootShowed = this->m_configUtil->get_reboot_function_is_showed_for_user();
    bool shutdownShowed = this->m_configUtil->get_shutdown_function_is_showed_for_user();
    bool openTerminalShowed = this->m_configUtil->get_open_terminal_function_is_showed_for_user();
    bool factoryResetShowed = this->m_configUtil->get_factory_reset_function_is_showed_for_user();
    QObject *operateForm = rootObject->findChild<QObject *>("operateForm");
    QObject *exportSettingLayout = operateForm->findChild<QObject *>("exportSettingLayout");
    QObject *exportScreenshotLayout = operateForm->findChild<QObject *>("exportScreenshotLayout");
    QObject *importSettingLayout = operateForm->findChild<QObject *>("importSettingLayout");
    QObject *rebootLayout = operateForm->findChild<QObject *>("rebootLayout");
    QObject *shutdownLayout = operateForm->findChild<QObject *>("shutdownLayout");
    QObject *openTerminalLayout = operateForm->findChild<QObject *>("openTerminalLayout");
    QObject *factoryResetLayout = operateForm->findChild<QObject *>("factoryResetLayout");
    // disable operation when login with non-root user
    if (this->m_loginType.compare(LOGIN_TYPE_SYS_USER) == 0 && 
        !this->m_loginIdentity.empty() && this->m_loginIdentity.compare(ROOT_USER) != 0) {
        exportSettingLayout->setProperty("visible", QVariant(exportSettingShowed));
        exportScreenshotLayout->setProperty("visible", QVariant(exportScreenshotShowed));
        importSettingLayout->setProperty("visible", QVariant(importSettingShowed));
        rebootLayout->setProperty("visible", QVariant(rebootShowed));
        shutdownLayout->setProperty("visible", QVariant(shutdownShowed));
        openTerminalLayout->setProperty("visible", QVariant(openTerminalShowed));
        factoryResetLayout->setProperty("visible", QVariant(factoryResetShowed));
    }
}

void QMLWindow::initOperateWindowHandler(QObject *rootObject)
{
    QObject *operateForm = rootObject->findChild<QObject *>("operateForm");
    QObject *fileDialog = operateForm->findChild<QObject *>("fileDialog");
    QObject *folderDialog = operateForm->findChild<QObject *>("folderDialog");
    QObject *rebootButton = operateForm->findChild<QObject *>("rebootButton");
    QObject *shutdownButton = operateForm->findChild<QObject *>("shutdownButton");
    QObject *openTerminalButton = operateForm->findChild<QObject *>("openTerminalButton");
    QObject *factoryResetButton = operateForm->findChild<QObject *>("factoryResetButton");
    QObject *exportScreenshotsButton = operateForm->findChild<QObject *>("exportScreenshotsButton");
    QObject::connect(fileDialog, SIGNAL(accepted()),
                     this, SLOT(on_operateWindow_importFileDialog_accepted()));
    QObject::connect(folderDialog, SIGNAL(accepted()),
                     this, SLOT(on_operateWindow_exportFolderDialog_accepted()));
    QObject::connect(rebootButton, SIGNAL(clicked()),
                     this, SLOT(on_operateWindow_rebootButton_clicked()));
    QObject::connect(shutdownButton, SIGNAL(clicked()),
                     this, SLOT(on_operateWindow_shutdownButton_clicked()));
    QObject::connect(openTerminalButton, SIGNAL(clicked()),
                     this, SLOT(on_operateWindow_openTerminalButton_clicked()));
    QObject::connect(factoryResetButton, SIGNAL(clicked()),
                     this, SLOT(on_operateWindow_factoryResetButton_clicked()));
    QObject::connect(exportScreenshotsButton, SIGNAL(clicked()),
                     this, SLOT(on_operateWindow_exportScreenshotsButton_clicked()));
}

void QMLWindow::initPasswordWindowValue(QObject *rootObject)
{
    QObject *passwordForm = rootObject->findChild<QObject *>("passwordForm");
    QObject *changePasswordLabel = passwordForm->findChild<QObject *>("changePasswordLabel");
    QString label = QString("Change %1 password").arg(this->m_loginIdentity.c_str());
    if (!this->m_loginAliasName.empty()) {
        label = QString("Change %1 password").arg(this->m_loginAliasName.c_str());
    }
    changePasswordLabel->setProperty("text", QVariant(label));
}

void QMLWindow::initPasswordWindowHandler(QObject *rootObject)
{
    QObject *passwordForm = rootObject->findChild<QObject *>("passwordForm");
    QObject *applyButton = passwordForm->findChild<QObject *>("applyButton");
    QObject::connect(applyButton, SIGNAL(clicked()),
                     this, SLOT(on_passwordWindow_applyButton_clicked()));
}

void QMLWindow::waitNetworkSettingIsReady(QObject *rootObject, const char* ethernet)
{
    // start loading
    this->showLoadingIndicator(rootObject, true);
    // start polling thread
    this->m_pollingThread = new PollingThread(ethernet, DHCP_TIMEOUT, this->m_networkUtil);
    connect(this->m_pollingThread, SIGNAL(pollingFinishedSignal(bool, bool)),
            this, SLOT(pollingNetworkSettingIsReady(bool, bool)));
    connect(this->m_pollingThread, &PollingThread::finished,
            this->m_pollingThread, &QObject::deleteLater);
    this->m_pollingThread->start();
}

void QMLWindow::pollingNetworkSettingIsReady(bool isSuccess, bool isWiredOnline)
{
    this->showLoadingIndicator(this->m_rootObject, false);
    if (isSuccess)
    {
        this->showMessageDialog(this->m_rootObject, isSuccess, nullptr, NONE_HANDLER_INDEX);
        this->initNetworkWindowValue(this->m_rootObject);
    }
    else
    {
        string msg;
        if (isWiredOnline)
            msg = "Please check that your DHCP server is accessible and properly configured.";
        else
            msg = "Please check that your wired is connected.";
        this->showMessageDialog(this->m_rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
}

void QMLWindow::waitNetworkIPIsReady(QObject *rootObject, const char* ethernet)
{
    // start polling thread
    this->m_pollingThread = new PollingThread(ethernet, DHCP_TIMEOUT, this->m_networkUtil);
    connect(this->m_pollingThread, SIGNAL(pollingFinishedSignal(bool, bool)),
            this, SLOT(pollingNetworkIPIsReady(bool, bool)));
    connect(this->m_pollingThread, &PollingThread::finished,
            this->m_pollingThread, &QObject::deleteLater);
    this->m_pollingThread->start();
}

void QMLWindow::pollingNetworkIPIsReady(bool isSuccess, bool isWiredOnline)
{
    this->initNetworkWindowValue(this->m_rootObject);
}

void QMLWindow::importConfigIsFinished(QString customMessage, bool isSuccess)
{
    string msg = customMessage.toStdString();
    this->showLoadingIndicator(this->m_rootObject, false);
    this->showMessageDialog(this->m_rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
}

void QMLWindow::applyTimeSettingIsFinished(QString customMessage, bool isSuccess)
{
    string msg = customMessage.toStdString();
    this->showLoadingIndicator(this->m_rootObject, false);
    this->showMessageDialog(this->m_rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
}

void QMLWindow::applyNetworkSetting(QObject *rootObject, const char* ethernet)
{
    bool isSuccess = true;
    string msg;
    string empty;
    string ethernetTitle = ETHERNET_0_TITLE;
    QObject *networkForm = rootObject->findChild<QObject *>("networkForm");
    QObject *dhcpSwitch = networkForm->findChild<QObject *>("dhcpSwitch1");
    QObject *ipTextField = networkForm->findChild<QObject *>("ipTextField1");
    QObject *networkMaskTextField = networkForm->findChild<QObject *>("networkMaskTextField1");
    QObject *defaultGatewayTextField = networkForm->findChild<QObject *>("defaultGatewayTextField1");
    QObject *dns1TextField = networkForm->findChild<QObject *>("dns1TextField1");
    QObject *dns2TextField = networkForm->findChild<QObject *>("dns2TextField1");
    QObject *configButton = networkForm->findChild<QObject *>("configButton1");
    if (strcmp(ethernet, ETHERNET_1) == 0)
    {
        ethernetTitle = ETHERNET_1_TITLE;
        dhcpSwitch = networkForm->findChild<QObject *>("dhcpSwitch2");
        ipTextField = networkForm->findChild<QObject *>("ipTextField2");
        networkMaskTextField = networkForm->findChild<QObject *>("networkMaskTextField2");
        defaultGatewayTextField = networkForm->findChild<QObject *>("defaultGatewayTextField2");
        dns1TextField = networkForm->findChild<QObject *>("dns1TextField2");
        dns2TextField = networkForm->findChild<QObject *>("dns2TextField2");
        configButton = networkForm->findChild<QObject *>("configButton2");
    }
    bool setIsDHCP = dhcpSwitch->property("checked").toBool();
    QString ip = ipTextField->property("text").toString();
    QString networkMask = networkMaskTextField->property("text").toString();
    QString defaultGateway = defaultGatewayTextField->property("text").toString();
    QString dns1 = dns1TextField->property("text").toString();
    QString dns2 = dns2TextField->property("text").toString();
    // delete provisioning file first
    this->m_networkUtil->delete_offline_provisioning_file(ethernet);
    const auto retethStatus = this->m_networkUtil->get_ethernet_status(ethernet);
    bool isWiredOnline = retethStatus.second;
    if (!setIsDHCP && ip.isEmpty())
    {
        isSuccess = false;
        msg = QString("Please enter %1 IP address.").arg(ethernetTitle.c_str()).toStdString();
    }
    else if (!setIsDHCP && (!ip.isEmpty() && !is_valid_ip_address(ip.toStdString().c_str())) ||
        (!networkMask.isEmpty() && !is_valid_ip_address(networkMask.toStdString().c_str())) ||
        (!defaultGateway.isEmpty() && !is_valid_ip_address(defaultGateway.toStdString().c_str())) )
    {
        isSuccess = false;
        msg = QString("Please check that %1 IP address/network mask/default geteway are valid format.").arg(
            ethernetTitle.c_str()).toStdString();
    }
    else
    {
        if (isWiredOnline)
        {
            if (setIsDHCP)
            {
                // clear static dns server for getting from dhcp
                isSuccess &= this->m_networkUtil->set_dns_server(ethernet,
                                                                empty.c_str(),
                                                                empty.c_str());
                // set dhcp
                isSuccess &= this->m_networkUtil->set_dhcp(ethernet, true);
            }
            else
            {
                // set static ip
                isSuccess &= this->m_networkUtil->set_static_ip_address(ethernet,
                                                                        ip.toStdString().c_str(),
                                                                        nullptr,
                                                                        networkMask.toStdString().c_str(),
                                                                        defaultGateway.toStdString().c_str());
                isSuccess &= this->m_networkUtil->set_dns_server(ethernet,
                                                                dns1.toStdString().c_str(),
                                                                dns2.toStdString().c_str());
            }
            if (isSuccess)
            {
                this->waitNetworkIPIsReady(rootObject, ethernet);
            }
        }
        // save to config
        this->m_configUtil->set_net_has_configured(ethernet, isWiredOnline);
        if (setIsDHCP)
        {
            // set dhcp
            this->m_configUtil->set_net_dhcp(ethernet);
        }
        else
        {
            // set static ip
            this->m_configUtil->set_net_static_ip(ethernet,
                                                    ip.toStdString().c_str(),
                                                    nullptr,
                                                    networkMask.toStdString().c_str(),
                                                    defaultGateway.toStdString().c_str());
            this->m_configUtil->set_net_dns_server(ethernet,
                                                    dns1.toStdString().c_str(),
                                                    dns2.toStdString().c_str());
        }
        // return to info view
        QMetaObject::invokeMethod(configButton, "clicked");
    }

    this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
}

void QMLWindow::applyNetworkFirewallSetting(QObject *rootObject)
{
    bool isSuccess = true;
    QObject *networkForm = rootObject->findChild<QObject *>("networkForm");
    QObject *firewallRuleRepeater = networkForm->findChild<QObject *>("firewallRuleRepeater");
    int rulesCount = firewallRuleRepeater->property("count").toInt();
    int validRuleCount = 0;
    vector<map<string, string>> validRules;
    map<string, string> validRuleMap;
    QVariantMap retRuleMap;
    QVariant retRuleElement;

    // set firewall rule
    for (int i = 0; i < rulesCount; i++) {
        QMetaObject::invokeMethod(networkForm, "getFirewallRuleByIndex",
                                  Q_RETURN_ARG(QVariant, retRuleElement),
                                  Q_ARG(QVariant, QVariant::fromValue(i)));
        retRuleMap = retRuleElement.toMap();
        // skip empty element
        if (retRuleMap.value(PORT_STRING).toString().isEmpty()) {
            continue;
        }
        validRuleMap[PROTOCOL_STRING] = retRuleMap.value(PROTOCOL_STRING).toString().toStdString();
        validRuleMap[PORT_STRING] = retRuleMap.value(PORT_STRING).toString().toStdString();
        validRuleMap[IS_ALLOWED_STRING] = bool_cast(retRuleMap.value(IS_ALLOWED_STRING).toBool());
        this->m_configUtil->set_firewall_rule_by_index(validRuleMap, validRuleCount);
        validRules.push_back(validRuleMap);
        validRuleCount++;
    }
    this->m_configUtil->set_firewall_rules_count(validRuleCount);
    // set firewall rules
    isSuccess = this->m_networkUtil->set_firewall_accept_ports(validRules);
    this->showMessageDialog(rootObject, isSuccess, nullptr, NONE_HANDLER_INDEX);
}

void QMLWindow::applyTimeSetting(QObject *rootObject)
{
    // start loading
    this->showLoadingIndicator(rootObject, true);
    
    // get binded function pointer
    auto pApplyTimeSettingFunction = std::bind(&QMLWindow::bg_applyTimeSetting, this, 
        this->m_rootObject, this->m_timeUtil, this->m_configUtil);
    // run in worker thread prevent block UI thread
    this->m_workThread = new WorkerThread(pApplyTimeSettingFunction, 1);
    connect(this->m_workThread, SIGNAL(workFinishedWithResult(QString, bool)),
            this, SLOT(applyTimeSettingIsFinished(QString, bool)));
    connect(this->m_workThread, &WorkerThread::finished,
            this->m_workThread, &QObject::deleteLater);
    this->m_workThread->start();
}

pair<string, bool> QMLWindow::bg_applyTimeSetting(QObject *rootObject, ITimeUtility *pTimeUtil, 
    ConfigUtility* pConfigUtil)
{
    bool isSuccess = true;
    string msg;
    QObject *timeForm = rootObject->findChild<QObject *>("timeForm");
    QObject *timezoneTextField = timeForm->findChild<QObject *>("timezoneTextField");
    QObject *ntpRadioButton = timeForm->findChild<QObject *>("ntpRadioButton");
    QObject *ntpServerTextField = timeForm->findChild<QObject *>("ntpServerTextField");
    QObject *dateTextField = timeForm->findChild<QObject *>("dateTextField");
    QObject *hourComboBox = timeForm->findChild<QObject *>("hourComboBox");
    QObject *minuteComboBox = timeForm->findChild<QObject *>("minuteComboBox");
    QObject *secondComboBox = timeForm->findChild<QObject *>("secondComboBox");
    QString timezone = timezoneTextField->property("text").toString();
    QString ntpServer = ntpServerTextField->property("text").toString();
    QString date = dateTextField->property("text").toString();
    QString hour = hourComboBox->property("currentText").toString();
    QString minute = minuteComboBox->property("currentText").toString();
    QString second = secondComboBox->property("currentText").toString();
    QString datetime = QString("%1 %2:%3:%4").arg(date, hour, minute, second);
    bool setIsNTP = ntpRadioButton->property("checked").toBool();
    if (!ntpServer.isEmpty() && 
        !(is_valid_domain(ntpServer.toStdString().c_str()) || is_valid_ip_address(ntpServer.toStdString().c_str())) )
    {
        isSuccess = false;
        msg = "Please check that ntp server is valid domain/ip format.";
    }
    else
    {
        // set timezone
        auto result = pTimeUtil->set_timezone(timezone.toStdString().c_str());
        isSuccess &= result.second;
        // set sync with ntp
        result = pTimeUtil->set_sync_with_ntp_server(setIsNTP, ntpServer.toStdString().c_str());
        isSuccess &= result.second;
        if (!setIsNTP)
        {
            // set manual time
            result = pTimeUtil->set_manual_date_time(datetime.toStdString().c_str());
            isSuccess &= result.second;
        }
        msg = result.first;
        // save to config
        pConfigUtil->set_timezone(timezone.toStdString().c_str());
        pConfigUtil->set_ntp_enable(setIsNTP);
        pConfigUtil->set_ntp_server(ntpServer.toStdString().c_str());
        pConfigUtil->set_date(date.toStdString().c_str());
        pConfigUtil->set_hour(std::stoi(hour.toStdString()));
        pConfigUtil->set_minute(std::stoi(minute.toStdString()));
        pConfigUtil->set_second(std::stoi(second.toStdString()));
    }

    return make_pair(msg, isSuccess);
}

void QMLWindow::applyScreenSetting(QObject *rootObject)
{
    bool isSuccess = true;
    bool currentIsHideCursor = this->m_screenUtil->get_hide_cursor();
    string currentTopBarPosition = this->m_screenUtil->get_top_bar_position();
    string currentRotateScreen = this->m_screenUtil->get_rotate_screen();
    QObject *screenForm = rootObject->findChild<QObject *>("screenForm");
    QObject *brightSlider = screenForm->findChild<QObject *>("brightSlider");
    QObject *screensaverSwitch = screenForm->findChild<QObject *>("screensaverSwitch");
    QObject *screensaverSpinbox = screenForm->findChild<QObject *>("screensaverSpinbox");
    QObject *hidecursorSwitch = screenForm->findChild<QObject *>("hidecursorSwitch");
    QObject *gestureSwitch = screenForm->findChild<QObject *>("gestureSwitch");
    QObject *gestureSwipeDownSwitch = screenForm->findChild<QObject *>("gestureSwipeDownSwitch");
    QObject *gestureSwipeUpSwitch = screenForm->findChild<QObject *>("gestureSwipeUpSwitch");
    QObject *gestureSwipeRightSwitch = screenForm->findChild<QObject *>("gestureSwipeRightSwitch");
    QString idleTime = screensaverSpinbox->property("value").toString();
    bool setIsScreenSaver = screensaverSwitch->property("checked").toBool();
    bool setIsHideCursor = hidecursorSwitch->property("checked").toBool();
    bool IsGeneralGesture = gestureSwitch->property("visible").toBool();
    bool setIsGestureEnable = gestureSwitch->property("checked").toBool();
    bool setIsGestureSwipeDownEnable = gestureSwipeDownSwitch->property("checked").toBool();
    bool setIsGestureSwipeUpEnable = gestureSwipeUpSwitch->property("checked").toBool();
    bool setIsGestureSwipeRightEnable = gestureSwipeRightSwitch->property("checked").toBool();
    int setBrightness = brightSlider->property("value").toInt();
    QVariant setTopBarPosition;
    QVariant setRotateScreen;
    QMetaObject::invokeMethod(screenForm, "getCurrentTopBarPosition",
                              Q_RETURN_ARG(QVariant, setTopBarPosition));
    QMetaObject::invokeMethod(screenForm, "getCurrentRotateScreen",
                              Q_RETURN_ARG(QVariant, setRotateScreen));
    string msg = "If you want to take screensaver idle time effect immediately, click OK to restart desktop service. Do you want to continue?";
    int setMinutes = 0;
    if (setIsScreenSaver)
    {
        setMinutes = idleTime.toInt();
    }
    isSuccess &= this->m_screenUtil->set_screensaver_idle_time(setMinutes * MINUTE_OF_SECONDS);
    // hide cursor changes
    if (currentIsHideCursor != setIsHideCursor)
    {
        isSuccess &= this->m_screenUtil->set_hide_cursor(setIsHideCursor);
        msg = "If you want to take cursor effect immediately, click OK to restart desktop service. Do you want to continue?";
    }
    // top bar changes
    if (currentTopBarPosition.compare(setTopBarPosition.toString().toStdString()) != 0)
    {
        isSuccess &= this->m_screenUtil->set_top_bar_position(setTopBarPosition.toString().toStdString().c_str());
        msg = "If you want to take top bar position effect immediately, click OK to restart desktop service. Do you want to continue?";
    }
    // rotate screen changes
    if (currentRotateScreen.compare(setRotateScreen.toString().toStdString()) != 0)
    {
        isSuccess &= this->m_screenUtil->set_rotate_screen(setRotateScreen.toString().toStdString().c_str());
        msg = "If you want to take rotate screen effect immediately, click OK to restart desktop service. Do you want to continue?";
    }
    // gesture changes
    if (IsGeneralGesture)
    {
        isSuccess &= this->m_screenUtil->set_all_finger_gesture_enabled(setIsGestureEnable);
        isSuccess &= this->m_screenUtil->set_2_finger_gesture_swipe_down_enabled(setIsGestureSwipeDownEnable);
        isSuccess &= this->m_screenUtil->set_2_finger_gesture_swipe_up_enabled(setIsGestureSwipeUpEnable);
        isSuccess &= this->m_screenUtil->set_2_finger_gesture_swipe_right_enabled(setIsGestureSwipeRightEnable);
    }
    // restart gesture service if rotate screen or gesture changed
    isSuccess &= this->m_screenUtil->restart_gesture_service();
    // save to config
    this->m_configUtil->set_screensaver_enable(setIsScreenSaver);
    this->m_configUtil->set_blank_after(setMinutes);
    this->m_configUtil->set_hide_cursor_enable(setIsHideCursor);
    this->m_configUtil->set_top_bar_position(setTopBarPosition.toString().toStdString().c_str());
    this->m_configUtil->set_rotate_screen(setRotateScreen.toString().toStdString().c_str());
    this->m_configUtil->set_brightness(setBrightness);
    this->m_configUtil->set_gesture_enable(setIsGestureEnable);
    this->m_configUtil->set_gesture_swipe_down_enable(setIsGestureSwipeDownEnable);
    this->m_configUtil->set_gesture_swipe_up_enable(setIsGestureSwipeUpEnable);
    this->m_configUtil->set_gesture_swipe_right_enable(setIsGestureSwipeRightEnable);

    if (isSuccess)
    {
        this->showQuestionDialog(rootObject, msg, SCREEN_HANDLER_INDEX);
    }
    else
    {
        this->showMessageDialog(rootObject, isSuccess, nullptr, NONE_HANDLER_INDEX);
    }
}

void QMLWindow::applySystemStartupSetting(QObject *rootObject)
{
    bool isSuccess = true;
    string msg;
    const auto currentStartup = this->m_configUtil->get_startup();
    QObject *systemForm = rootObject->findChild<QObject *>("systemForm");
    QObject *timeoutTextField = systemForm->findChild<QObject *>("timeoutTextField");
    QObject *urlTextField = systemForm->findChild<QObject *>("urlTextField");
    QObject *fileTextField = systemForm->findChild<QObject *>("fileTextField");
    QObject *autoRestartSwitch = systemForm->findChild<QObject *>("autoRestartSwitch");
    QObject *webPageRepeater = systemForm->findChild<QObject *>("webPageRepeater");
    QObject *vncServerTextField = systemForm->findChild<QObject *>("vncServerTextField");
    QObject *vncPasswordTextField = systemForm->findChild<QObject *>("vncPasswordTextField");
    QObject *vncViewonlySwitch = systemForm->findChild<QObject *>("vncViewonlySwitch");
    QObject *vncFullscreenSwitch = systemForm->findChild<QObject *>("vncFullscreenSwitch");
    QObject *vncFitWindowSwitch = systemForm->findChild<QObject *>("vncFitWindowSwitch");
    QObject *vncImageQualitySpinbox = systemForm->findChild<QObject *>("vncImageQualitySpinbox");
    QString timeout = timeoutTextField->property("text").toString();
    QString url = urlTextField->property("text").toString();
    QString filepath = fileTextField->property("text").toString();
    QString vncFullServer = vncServerTextField->property("text").toString();
    QString vncPassword = vncPasswordTextField->property("text").toString();
    QString setVncImageQuality = vncImageQualitySpinbox->property("value").toString();
    bool setVncViewonly = vncViewonlySwitch->property("checked").toBool();
    bool setVncFullscreen = vncFullscreenSwitch->property("checked").toBool();
    bool setVncFitWindow = vncFitWindowSwitch->property("checked").toBool();
    bool setIsAutoRestart = autoRestartSwitch->property("checked").toBool();
    bool isNeedAutoRestart = autoRestartSwitch->property("visible").toBool();
    int webPageCount = webPageRepeater->property("count").toInt();
    int validWebPageCount = 0;
    map<string, string> webPageMap;
    QVariantMap retWebPageMap;
    QVariant retWebPageElement;
    QVariant retStartup;
    QMetaObject::invokeMethod(systemForm, "getCurrentStartup",
                              Q_RETURN_ARG(QVariant, retStartup));

    if (retStartup.toString().compare(STARTUP_NAME_VNC_VIEWER) == 0)
    {
        // split address and port
        string address, port;
        QStringList splitList = vncFullServer.split(QLatin1Char(':'), Qt::SkipEmptyParts);
        if (splitList.size() > 0)
            address = splitList.at(0).toStdString();
        if (splitList.size() > 1)
            port = splitList.at(1).toStdString();
        if (address.empty())
        {
            isSuccess = false;
            msg = "Please enter VNC server hostname or ip address.";
        }
        else if (port.empty())
        {
            isSuccess = false;
            msg = "Please enter VNC server port.";
        }
        else if (!address.empty() && is_contains_special_characters(address.c_str()))
        {
            isSuccess = false;
            msg = "Please check that hostname or ip address is valid format.";
        }
        else if (!port.empty() && !is_numbers(port.c_str()))
        {
            isSuccess = false;
            msg = "Please check that port is valid number.";
        }
        else
        {
            // set vnc
            this->m_configUtil->set_vnc_server_address(address.c_str());
            this->m_configUtil->set_vnc_server_port(port.c_str());
            this->m_configUtil->set_vnc_server_password(vncPassword.toStdString().c_str());
            this->m_configUtil->set_vnc_server_viewonly(setVncViewonly);
            this->m_configUtil->set_vnc_server_fullscreen(setVncFullscreen);
            this->m_configUtil->set_vnc_server_fit_window(setVncFitWindow);
            this->m_configUtil->set_vnc_server_image_quality(setVncImageQuality.toInt());
        }
    }

    // set web page
    for (int i = 0; i < webPageCount; i++) {
        QMetaObject::invokeMethod(systemForm, "getWebPageByIndex",
                                  Q_RETURN_ARG(QVariant, retWebPageElement),
                                  Q_ARG(QVariant, QVariant::fromValue(i)));
        retWebPageMap = retWebPageElement.toMap();
        // skip empty element
        if (retWebPageMap.value("page").toString().isEmpty()) {
            continue;
        }
        webPageMap["page"] = retWebPageMap.value("page").toString().toStdString();
        webPageMap["is_startup"] = retWebPageMap.value("is_startup").toString().toStdString();
        this->m_configUtil->set_web_page_by_index(webPageMap, validWebPageCount);
        validWebPageCount++;
    }
    this->m_configUtil->set_web_pages_count(validWebPageCount);
    // set startup
    this->m_configUtil->set_startup(retStartup.toString().toStdString().c_str());
    this->m_configUtil->set_static_page_timeout(timeout.toInt());
    this->m_configUtil->set_static_page_url(url.toStdString().c_str());
    this->m_configUtil->set_static_page_file_path(filepath.toStdString().c_str());
    this->m_configUtil->set_startup_auto_restart(setIsAutoRestart);

    if (isSuccess) {
        // auto restart is changed
        if (setIsAutoRestart && isNeedAutoRestart) {
            msg = "You need to reboot for the startup application and monitor to take effect.";
        }
    }

    this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
}

void QMLWindow::applySystemGeneralSetting(QObject *rootObject)
{
    bool isSuccess = true;
    string msg;
    bool currentIsReadonly = this->m_systemUtil->get_readonly_mode();
    bool currentIsUserLogin = this->m_systemUtil->get_system_user_login_desktop();
    QObject *systemForm = rootObject->findChild<QObject *>("systemForm");
    QObject *readonlySwitch = systemForm->findChild<QObject *>("readonlySwitch");
    QObject *userLoginSwitch = systemForm->findChild<QObject *>("userLoginSwitch");
    QObject *ethernetSwitch = systemForm->findChild<QObject *>("ethernetSwitch");
    QObject *usbSwitch = systemForm->findChild<QObject *>("usbSwitch");
    QObject *chromiumSysVKBSwitch = systemForm->findChild<QObject *>("chromiumSysVKBSwitch");
    QObject *rebootSystemCronJobSwitch = systemForm->findChild<QObject *>("rebootSystemCronJobSwitch");
    QObject *cronMinuteComboBox = systemForm->findChild<QObject *>("cronMinuteComboBox");
    QObject *cronHourComboBox = systemForm->findChild<QObject *>("cronHourComboBox");
    QObject *cronDayofweekComboBox = systemForm->findChild<QObject *>("cronDayofweekComboBox");
    QString minute = cronMinuteComboBox->property("currentText").toString();
    QString hour = cronHourComboBox->property("currentText").toString();
    QString dayofweek = cronDayofweekComboBox->property("currentText").toString();
    bool setIsReadonly = readonlySwitch->property("checked").toBool();
    bool setIsUserLogin = userLoginSwitch->property("checked").toBool();
    bool setEthernetEnable = ethernetSwitch->property("checked").toBool();
    bool setUSBEnable = usbSwitch->property("checked").toBool();
    bool setChromiumUseSysVKB = chromiumSysVKBSwitch->property("checked").toBool();
    bool setIsRestartSystemCronJob = rebootSystemCronJobSwitch->property("checked").toBool();
    QVariant retStartup;
    QMetaObject::invokeMethod(systemForm, "getCurrentStartup",
                              Q_RETURN_ARG(QVariant, retStartup));
    QVariant retCronMode;
    QMetaObject::invokeMethod(systemForm, "getCurrentCronMode",
                              Q_RETURN_ARG(QVariant, retCronMode));
    // set user login and user login is changed
    if (setIsUserLogin && setIsUserLogin != currentIsUserLogin) {
        bool retEnabled = this->m_configUtil->get_login_enable();
        // NOTE: must set password for settings tool when login as user
        if (!retEnabled) {
            msg = "If you want to enable login as user, please enable Settings tool security first!";
            this->showMessageDialog(rootObject, false, &msg, NONE_HANDLER_INDEX);
            return;
        }
        // NOTE: user can not use gesture to close/switch application
        this->m_screenUtil->set_2_finger_gesture_swipe_up_enabled(false);
        this->m_screenUtil->set_2_finger_gesture_swipe_right_enabled(false);
        this->m_configUtil->set_gesture_swipe_up_enable(false);
        this->m_configUtil->set_gesture_swipe_right_enable(false);
    }

    // set general
    this->m_configUtil->set_system_user_login_desktop(setIsUserLogin);
    this->m_configUtil->set_ethernet_enable(setEthernetEnable);
    this->m_configUtil->set_usb_enable(setUSBEnable);
    this->m_configUtil->set_chromium_use_sys_virtual_keyboard(setChromiumUseSysVKB);
    this->m_configUtil->set_reboot_system_crontab_enabled(setIsRestartSystemCronJob);
    this->m_configUtil->set_reboot_system_crontab_mode(retCronMode.toString().toStdString().c_str());
    this->m_configUtil->set_reboot_system_crontab_minute(minute.toInt());
    this->m_configUtil->set_reboot_system_crontab_hour(hour.toInt());
    this->m_configUtil->set_reboot_system_crontab_dayofweek(dayofweek.toInt());
    // set usb
    this->m_systemUtil->set_usb_enable(setUSBEnable);
    // set ethernet
    this->m_systemUtil->do_init_ethernet();
    
    // set restart system crontab
    this->m_systemUtil->set_reboot_system_crontab(setIsRestartSystemCronJob, retCronMode.toString().toStdString().c_str(),
                                                   minute.toInt(), hour.toInt(), dayofweek.toInt());
    // restart crond service
    this->m_systemUtil->do_restart_crond_service();

    // readonly mode is changed
    if (setIsReadonly != currentIsReadonly) 
    {
        msg = "If you want to enable/disable read-only mode, click OK to reboot system. Do you want to continue?";
        this->showQuestionDialog(rootObject, msg, SYSTEM_READONLY_HANDLER_INDEX);
    }
    else if (setIsUserLogin != currentIsUserLogin) {
        msg = "If you want to enable/disable login as user, click OK to reboot system. Do you want to continue?";
        this->showQuestionDialog(rootObject, msg, SYSTEM_USER_LOGIN_HANDLER_INDEX);
    }
    else 
    {
        this->showMessageDialog(rootObject, isSuccess, nullptr, NONE_HANDLER_INDEX);
    }
}

void QMLWindow::applySystemCOMSetting(QObject *rootObject)
{
    bool isSuccess = true;
    QObject *systemForm = rootObject->findChild<QObject *>("systemForm");
    QObject *com1BaudRateComboBox = systemForm->findChild<QObject *>("com1BaudRateComboBox");
    QObject *com2BaudRateComboBox = systemForm->findChild<QObject *>("com2BaudRateComboBox");
    QString com1BaudRate = com1BaudRateComboBox->property("currentText").toString();
    QString com2BaudRate = com2BaudRateComboBox->property("currentText").toString();
    QVariant retcom1Mode;
    QMetaObject::invokeMethod(systemForm, "getCurrentCom1Mode",
                              Q_RETURN_ARG(QVariant, retcom1Mode));
    this->m_configUtil->set_com1_mode(retcom1Mode.toString().toStdString().c_str());
    this->m_configUtil->set_com1_baudrate(com1BaudRate.toStdString().c_str());
    this->m_configUtil->set_com2_baudrate(com2BaudRate.toStdString().c_str());
    // set com port
    this->m_systemUtil->do_init_com_port();

    this->showMessageDialog(rootObject, isSuccess, nullptr, NONE_HANDLER_INDEX);
}

void QMLWindow::on_systemWindow_questionDialog_readonly_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();

    // start loading
    this->showLoadingIndicator(this->m_rootObject, true);

    // enable/disable read-only mode
    bool currentIsReadonly = this->m_systemUtil->get_readonly_mode();
    // get binded function pointer
    auto pSetReadonlyMode = std::bind(&ISystemUtility::set_readonly_mode, this->m_systemUtil, !currentIsReadonly);
    // run in worker thread prevent block UI thread
    this->m_workThread = new WorkerThread(pSetReadonlyMode, 1);
    connect(this->m_workThread, &WorkerThread::workFinished,
            this->m_workThread, &QObject::deleteLater);
    this->m_workThread->start();
}

void QMLWindow::on_systemWindow_questionDialog_user_login_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();

    // start loading
    this->showLoadingIndicator(this->m_rootObject, true);

    // enable/disable user login
    bool currentIsUserLogin = this->m_systemUtil->get_system_user_login_desktop();
    // get binded function pointer
    auto pSetUserLogin = std::bind(&ISystemUtility::set_system_user_login_desktop, this->m_systemUtil, !currentIsUserLogin, true);
    // run in worker thread prevent block UI thread
    this->m_workThread = new WorkerThread(pSetUserLogin, 1);
    connect(this->m_workThread, &WorkerThread::workFinished,
            this->m_workThread, &QObject::deleteLater);
    this->m_workThread->start();
}

void QMLWindow::applySecuritySetting(QObject *rootObject)
{
    bool isSuccess = true;
    string msg;
    QObject *securityForm = rootObject->findChild<QObject *>("securityForm");
    QObject *enableSwitch = securityForm->findChild<QObject *>("enableSwitch");
    QObject *passwordTextField = securityForm->findChild<QObject *>("passwordTextField");
    QObject *passwordRadioButton = securityForm->findChild<QObject *>("passwordRadioButton");
    QObject *systemUserRadioButton = securityForm->findChild<QObject *>("systemUserRadioButton");
    QString password = passwordTextField->property("text").toString();
    bool isEnabled = enableSwitch->property("checked").toBool();
    bool setIsPassword = passwordRadioButton->property("checked").toBool();
    bool setIsSysUser = systemUserRadioButton->property("checked").toBool();
    // enable security does not allowed empty password
    if (isEnabled && setIsPassword && password.isEmpty())
    {
        isSuccess = false;
        msg = "Please check that password is properly configured.";
    }
    else
    {
        // set security
        this->m_configUtil->set_login_enable(isEnabled);
        this->m_configUtil->set_login_password(password.toStdString().c_str());
        if (setIsSysUser) {
            this->m_configUtil->set_login_type(LOGIN_TYPE_SYS_USER);
        } else {
            this->m_configUtil->set_login_type(LOGIN_TYPE_PASSWORD);
        }
    }
    this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
}

void QMLWindow::applyLogoSetting(QObject *rootObject)
{
    bool isSuccess = true;
    static const string PRESERVE_COLOR_HEX = "0x000000";
    string msg;
    QObject *logoForm = rootObject->findChild<QObject *>("logoForm");
    QObject *logoPathTextField = logoForm->findChild<QObject *>("logoPathTextField");
    QObject *bgColorTextField = logoForm->findChild<QObject *>("bgColorTextField");
    QString logoPath = logoPathTextField->property("text").toString();
    QString bgColor = bgColorTextField->property("text").toString();
    if (logoPath.isEmpty())
    {
        isSuccess = false;
        msg = QString("Boot logo path is empty, please select boot logo file path.")
                        .toStdString();
    }
    else
    {
        isSuccess = cp_file(logoPath.toStdString().c_str(), BOOT_LOGO_PATH);
    }
    if (!bgColor.isEmpty())
    {
        if (!is_hex_color(bgColor.toStdString().c_str()))
        {
            isSuccess = false;
            msg = QString("Background color format is incorrect.")
                            .toStdString();
        }
        else if (bgColor.toStdString().compare(PRESERVE_COLOR_HEX) == 0)
        {
            isSuccess = false;
            msg = QString("Please set another background color.")
                            .toStdString();
        }
        else
        {
            isSuccess &= this->m_systemUtil->set_boot_logo_background_color(bgColor.toStdString().c_str());
        }
    }
    if (isSuccess)
    {
        msg = "Boot logo has changed. Do you want to reboot?";
        this->showQuestionDialog(rootObject, msg, OPERATE_REBOOT_HANDLER_INDEX);
    }
    else
    {
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
}

void QMLWindow::applyPasswordSetting(QObject *rootObject)
{
    bool isSuccess = true;
    string msg;
    string user = this->m_loginIdentity;
    QObject *passwordForm = rootObject->findChild<QObject *>("passwordForm");
    QObject *passwordTextField = passwordForm->findChild<QObject *>("passwordTextField");
    QObject *confirmPasswordTextField = passwordForm->findChild<QObject *>("confirmPasswordTextField");
    QObject *currentPasswordTextField = passwordForm->findChild<QObject *>("currentPasswordTextField");
    QString password = passwordTextField->property("text").toString();
    QString confirmPassword = confirmPasswordTextField->property("text").toString();
    if (currentPasswordTextField) {
        QString currentPassword = currentPasswordTextField->property("text").toString();
        if (currentPassword.isEmpty())
        {
            isSuccess = false;
            msg = QString("%1 current password is empty, please fill it.").arg(user.c_str()).toStdString();
        }
        else if (currentPassword.compare(password) == 0)
        {
            isSuccess = false;
            msg = QString("%1 new password is same as current password, please change it.").arg(user.c_str()).toStdString();
        }
        else
        {
            auto ret = sys_auth_user(user.c_str(), currentPassword.toStdString().c_str());
            if (ret.second != EXIT_SUCCESS)
            {
                isSuccess = false;
                msg = QString("Incorrect %1 password.").arg(user.c_str()).toStdString();
            }
        }
        if (!isSuccess)
        {
            this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
            return;
        }
    }
    if (password.isEmpty() || confirmPassword.isEmpty())
    {
        isSuccess = false;
        msg = QString("%1 password or confirm password is empty, please fill it.").arg(user.c_str()).toStdString();
    }
    else if (password.compare(confirmPassword) != 0)
    {
        isSuccess = false;
        msg = QString("Incorrect %1 confirm password, please key in correct %1 confirm password.")
                        .arg(user.c_str()).toStdString();
    }
    if (isSuccess)
    {
        const auto ret = sys_change_user_password(user.c_str(), password.toStdString().c_str());
        if (!ret.first.empty())
        {
            isSuccess = false;
            msg = QString("Change password failed! %1")
                        .arg(ret.first.c_str()).toStdString();
        }
    }

    this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
}

bool QMLWindow::applyCredentialsSetting(QObject *rootObject)
{
    bool isSuccess = false;
    bool isRootPasswordRequired = this->m_configUtil->get_root_password_required();
    bool isWestonPasswordRequired = this->m_configUtil->get_weston_password_required();
    if (isRootPasswordRequired)
    {
        isSuccess = this->applyUserCredentialsSetting(this->m_rootObject, ROOT_USER);
        if (!isSuccess)
            return isSuccess;
    }
    if (isWestonPasswordRequired)
    {
        isSuccess = this->applyUserCredentialsSetting(this->m_rootObject, WESTON_USER);
        if (!isSuccess)
            return isSuccess;
    }
    isSuccess = this->checkCredentialsRequirement(this->m_rootObject);
    this->m_wizardStatusMap[WIZARD_CREDENTIALS] = isSuccess;
    return isSuccess;
}

bool QMLWindow::applyUserCredentialsSetting(QObject *rootObject, const char *username)
{
    bool isSuccess = true;
    string msg;
    string user = username;
    string user_arg = username;
    QObject *credentialsForm = rootObject->findChild<QObject *>("credentialsForm");
    QObject *passwordTextField = credentialsForm->findChild<QObject *>("rootPasswordTextField");
    QObject *confirmPasswordTextField = credentialsForm->findChild<QObject *>("rootConfirmPasswordTextField");
    QObject *currentPasswordTextField = nullptr;
    if (user.compare(WESTON_USER) == 0) {
        user_arg = USER_JOINT_NAME;
        passwordTextField = credentialsForm->findChild<QObject *>("westonPasswordTextField");
        confirmPasswordTextField = credentialsForm->findChild<QObject *>("westonConfirmPasswordTextField");
        currentPasswordTextField = credentialsForm->findChild<QObject *>("westonCurrentPasswordTextField");
    }
    QString password = passwordTextField->property("text").toString();
    QString confirmPassword = confirmPasswordTextField->property("text").toString();
    if (currentPasswordTextField) {
        QString currentPassword = currentPasswordTextField->property("text").toString();
        if (currentPassword.isEmpty())
        {
            isSuccess = false;
            msg = QString("%1 current password is empty, please fill it.").arg(user_arg.c_str()).toStdString();
        }
        else if (currentPassword.compare(password) == 0)
        {
            isSuccess = false;
            msg = QString("%1 new password is same as current password, please change it.").arg(user_arg.c_str()).toStdString();
        }
        else
        {
            auto ret = sys_auth_user(username, currentPassword.toStdString().c_str());
            if (ret.second != EXIT_SUCCESS)
            {
                isSuccess = false;
                msg = QString("Incorrect %1 password.").arg(user_arg.c_str()).toStdString();
            }
        }
        if (!isSuccess)
        {
            this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
            return isSuccess;
        }
    }
    if (password.isEmpty() || confirmPassword.isEmpty())
    {
        isSuccess = false;
        msg = QString("%1 password or confirm password is empty, please fill it.").arg(user_arg.c_str()).toStdString();
    }
    else if (password.compare(confirmPassword) != 0)
    {
        isSuccess = false;
        msg = QString("Incorrect %1 confirm password, please key in correct %1 confirm password.")
                        .arg(user_arg.c_str()).toStdString();
    }
    if (isSuccess)
    {
        const auto ret = sys_change_user_password(user.c_str(), password.toStdString().c_str());
        if (!ret.first.empty())
        {
            isSuccess = false;
            msg = QString("Setup password failed! %1")
                        .arg(ret.first.c_str()).toStdString();
        }
    }

    if (!isSuccess)
    {
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
    return isSuccess;
}

bool QMLWindow::checkCredentialsFields(QObject *rootObject, const char *username)
{
    bool isSuccess = true;
    string msg;
    string user = username;
    string user_arg = username;
    QObject *credentialsForm = rootObject->findChild<QObject *>("credentialsForm");
    QObject *passwordTextField = credentialsForm->findChild<QObject *>("rootPasswordTextField");
    QObject *confirmPasswordTextField = credentialsForm->findChild<QObject *>("rootConfirmPasswordTextField");
    QObject *currentPasswordTextField = nullptr;
    if (user.compare(WESTON_USER) == 0) {
        user_arg = USER_JOINT_NAME;
        passwordTextField = credentialsForm->findChild<QObject *>("westonPasswordTextField");
        confirmPasswordTextField = credentialsForm->findChild<QObject *>("westonConfirmPasswordTextField");
        currentPasswordTextField = credentialsForm->findChild<QObject *>("westonCurrentPasswordTextField");
    }
    QString password = passwordTextField->property("text").toString();
    QString confirmPassword = confirmPasswordTextField->property("text").toString();
    if (currentPasswordTextField) {
        QString currentPassword = currentPasswordTextField->property("text").toString();
        if (currentPassword.isEmpty())
        {
            isSuccess = false;
            msg = QString("%1 current password is empty, please fill it.").arg(user_arg.c_str()).toStdString();
        }
        else if (currentPassword.compare(password) == 0)
        {
            isSuccess = false;
            msg = QString("%1 new password is same as current password, please change it.").arg(user_arg.c_str()).toStdString();
        }
        else
        {
            auto ret = sys_auth_user(username, currentPassword.toStdString().c_str());
            if (ret.second != EXIT_SUCCESS)
            {
                isSuccess = false;
                msg = QString("Incorrect %1 password.").arg(user_arg.c_str()).toStdString();
            }
        }
        if (!isSuccess)
        {
            this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
            return isSuccess;
        }
    }
    if (password.isEmpty() || confirmPassword.isEmpty())
    {
        isSuccess = false;
        msg = QString("%1 password or confirm password is empty, please fill it.").arg(user_arg.c_str()).toStdString();
    }
    else if (password.compare(confirmPassword) != 0)
    {
        isSuccess = false;
        msg = QString("Incorrect %1 confirm password, please key in correct %1 confirm password.")
                        .arg(user_arg.c_str()).toStdString();
    }

    if (!isSuccess)
    {
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
    return isSuccess;
}

bool QMLWindow::checkCredentialsRequirement(QObject *rootObject)
{
    bool isSuccess = true;
    string msg;
    // check password
    bool isRootPasswordRequired = this->m_configUtil->get_root_password_required();
    bool isWestonPasswordRequired = this->m_configUtil->get_weston_password_required();
    if (isRootPasswordRequired && !is_user_password_exists(ROOT_USER)) {
        isSuccess = false;
        msg = "Root user has no password. Please setup root password!";
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
        return isSuccess;
    }
    if (isWestonPasswordRequired && !is_user_password_exists(WESTON_USER)) {
        isSuccess = false;
        msg = "Weston user has no password. Please setup weston password!";
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
        return isSuccess;
    }
    return isSuccess;
}

bool QMLWindow::applyWizardNetworkSetting(QObject *rootObject)
{
    bool isSuccess = true;
    // skip if not showed
    if (!this->m_wizardPageShowedMap[WIZARD_NETWORK])
        return isSuccess;

    isSuccess &= this->applyWizardEthernetNetworkSetting(rootObject, ETHERNET_0);
    isSuccess &= this->applyWizardEthernetNetworkSetting(rootObject, ETHERNET_1);
    this->m_wizardStatusMap[WIZARD_NETWORK] = isSuccess;
    return isSuccess;
}

bool QMLWindow::applyWizardEthernetNetworkSetting(QObject *rootObject, const char *ethernet)
{
    bool isSuccess = true;
    string msg;
    string ethernetTitle = ETHERNET_0_TITLE;
    QObject *wiznetworkForm = rootObject->findChild<QObject *>("wiznetworkForm");
    QObject *dhcpSwitch = wiznetworkForm->findChild<QObject *>("dhcpSwitch1");
    QObject *ipTextField = wiznetworkForm->findChild<QObject *>("ipTextField1");
    QObject *networkMaskTextField = wiznetworkForm->findChild<QObject *>("networkMaskTextField1");
    QObject *defaultGatewayTextField = wiznetworkForm->findChild<QObject *>("defaultGatewayTextField1");
    QObject *dns1TextField = wiznetworkForm->findChild<QObject *>("dns1TextField1");
    QObject *dns2TextField = wiznetworkForm->findChild<QObject *>("dns2TextField1");
    if (strcmp(ethernet, ETHERNET_1) == 0)
    {
        ethernetTitle = ETHERNET_1_TITLE;
        dhcpSwitch = wiznetworkForm->findChild<QObject *>("dhcpSwitch2");
        ipTextField = wiznetworkForm->findChild<QObject *>("ipTextField2");
        networkMaskTextField = wiznetworkForm->findChild<QObject *>("networkMaskTextField2");
        defaultGatewayTextField = wiznetworkForm->findChild<QObject *>("defaultGatewayTextField2");
        dns1TextField = wiznetworkForm->findChild<QObject *>("dns1TextField2");
        dns2TextField = wiznetworkForm->findChild<QObject *>("dns2TextField2");
    }
    bool setIsDHCP = dhcpSwitch->property("checked").toBool();
    QString ip = ipTextField->property("text").toString();
    QString networkMask = networkMaskTextField->property("text").toString();
    QString defaultGateway = defaultGatewayTextField->property("text").toString();
    QString dns1 = dns1TextField->property("text").toString();
    QString dns2 = dns2TextField->property("text").toString();
    // delete provisioning file first
    this->m_networkUtil->delete_offline_provisioning_file(ethernet);
    const auto retethStatus = this->m_networkUtil->get_ethernet_status(ethernet);
    bool isWiredOnline = retethStatus.second;
    if (!setIsDHCP && ip.isEmpty())
    {
        isSuccess = false;
        msg = QString("Please enter %1 IP address.").arg(ethernetTitle.c_str()).toStdString();
    }
    else if (!setIsDHCP && (!ip.isEmpty() && !is_valid_ip_address(ip.toStdString().c_str())) ||
        (!networkMask.isEmpty() && !is_valid_ip_address(networkMask.toStdString().c_str())) ||
        (!defaultGateway.isEmpty() && !is_valid_ip_address(defaultGateway.toStdString().c_str())) )
    {
        isSuccess = false;
        msg = QString("Please check that %1 IP address/network mask/default geteway are valid format.").arg(
            ethernetTitle.c_str()).toStdString();
    }
    else
    {
        if (isWiredOnline)
        {
            if (setIsDHCP)
            {
                // set dhcp
                isSuccess &= this->m_networkUtil->set_dhcp(ethernet, true);
            }
            else
            {
                // set static ip
                isSuccess &= this->m_networkUtil->set_static_ip_address(ethernet,
                                                                        ip.toStdString().c_str(),
                                                                        nullptr,
                                                                        networkMask.toStdString().c_str(),
                                                                        defaultGateway.toStdString().c_str());
                isSuccess &= this->m_networkUtil->set_dns_server(ethernet,
                                                                    dns1.toStdString().c_str(),
                                                                    dns2.toStdString().c_str());
            }
        } 
        else 
        {
            if (setIsDHCP)
            {
                // set dhcp
                isSuccess &= this->m_networkUtil->set_dhcp_offline(ethernet, true);
            }
            else
            {
                // set static ip
                isSuccess &= this->m_networkUtil->set_static_ip_address_offline(ethernet,
                                                                                ip.toStdString().c_str(),
                                                                                nullptr,
                                                                                networkMask.toStdString().c_str(),
                                                                                defaultGateway.toStdString().c_str());
                isSuccess &= this->m_networkUtil->set_dns_server_offline(ethernet,
                                                                            dns1.toStdString().c_str(),
                                                                            dns2.toStdString().c_str());
            }
            // create provisioning file
            this->m_networkUtil->create_offline_provisioning_file(ethernet);
        }
        // save to config
        this->m_configUtil->set_net_has_configured(ethernet, isWiredOnline);
        if (setIsDHCP)
        {
            // set dhcp
            this->m_configUtil->set_net_dhcp(ethernet);
        }
        else
        {
            // set static ip
            this->m_configUtil->set_net_static_ip(ethernet,
                                                    ip.toStdString().c_str(),
                                                    nullptr,
                                                    networkMask.toStdString().c_str(),
                                                    defaultGateway.toStdString().c_str());
            this->m_configUtil->set_net_dns_server(ethernet,
                                                    dns1.toStdString().c_str(),
                                                    dns2.toStdString().c_str());
        }
    }
    if (!isSuccess)
    {
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
    return isSuccess;
}

bool QMLWindow::checkWizardEthernetNetworkFields(QObject *rootObject, const char *ethernet)
{
    bool isSuccess = true;
    string msg;
    string ethernetTitle = ETHERNET_0_TITLE;
    QObject *wiznetworkForm = rootObject->findChild<QObject *>("wiznetworkForm");
    QObject *dhcpSwitch = wiznetworkForm->findChild<QObject *>("dhcpSwitch1");
    QObject *ipTextField = wiznetworkForm->findChild<QObject *>("ipTextField1");
    QObject *networkMaskTextField = wiznetworkForm->findChild<QObject *>("networkMaskTextField1");
    QObject *defaultGatewayTextField = wiznetworkForm->findChild<QObject *>("defaultGatewayTextField1");
    QObject *dns1TextField = wiznetworkForm->findChild<QObject *>("dns1TextField1");
    QObject *dns2TextField = wiznetworkForm->findChild<QObject *>("dns2TextField1");
    if (strcmp(ethernet, ETHERNET_1) == 0)
    {
        ethernetTitle = ETHERNET_1_TITLE;
        dhcpSwitch = wiznetworkForm->findChild<QObject *>("dhcpSwitch2");
        ipTextField = wiznetworkForm->findChild<QObject *>("ipTextField2");
        networkMaskTextField = wiznetworkForm->findChild<QObject *>("networkMaskTextField2");
        defaultGatewayTextField = wiznetworkForm->findChild<QObject *>("defaultGatewayTextField2");
        dns1TextField = wiznetworkForm->findChild<QObject *>("dns1TextField2");
        dns2TextField = wiznetworkForm->findChild<QObject *>("dns2TextField2");
    }
    bool setIsDHCP = dhcpSwitch->property("checked").toBool();
    QString ip = ipTextField->property("text").toString();
    QString networkMask = networkMaskTextField->property("text").toString();
    QString defaultGateway = defaultGatewayTextField->property("text").toString();
    QString dns1 = dns1TextField->property("text").toString();
    QString dns2 = dns2TextField->property("text").toString();
    // delete provisioning file first
    this->m_networkUtil->delete_offline_provisioning_file(ethernet);
    const auto retethStatus = this->m_networkUtil->get_ethernet_status(ethernet);
    bool isWiredOnline = retethStatus.second;
    if (!setIsDHCP && ip.isEmpty())
    {
        isSuccess = false;
        msg = QString("Please enter %1 IP address.").arg(ethernetTitle.c_str()).toStdString();
    }
    else if (!setIsDHCP && (!ip.isEmpty() && !is_valid_ip_address(ip.toStdString().c_str())) ||
        (!networkMask.isEmpty() && !is_valid_ip_address(networkMask.toStdString().c_str())) ||
        (!defaultGateway.isEmpty() && !is_valid_ip_address(defaultGateway.toStdString().c_str())) )
    {
        isSuccess = false;
        msg = QString("Please check that %1 IP address/network mask/default geteway are valid format.").arg(
            ethernetTitle.c_str()).toStdString();
    }
    if (!isSuccess)
    {
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
    return isSuccess;
}

bool QMLWindow::applyWizardTimeSetting(QObject *rootObject)
{
    bool isSuccess = true;
    // skip if not showed
    if (!this->m_wizardPageShowedMap[WIZARD_TIME])
        return isSuccess;
    string msg;
    QObject *wiztimeForm = rootObject->findChild<QObject *>("wiztimeForm");
    QObject *timezoneTextField = wiztimeForm->findChild<QObject *>("timezoneTextField");
    QObject *ntpRadioButton = wiztimeForm->findChild<QObject *>("ntpRadioButton");
    QObject *ntpServerTextField = wiztimeForm->findChild<QObject *>("ntpServerTextField");
    QObject *dateTextField = wiztimeForm->findChild<QObject *>("dateTextField");
    QObject *hourComboBox = wiztimeForm->findChild<QObject *>("hourComboBox");
    QObject *minuteComboBox = wiztimeForm->findChild<QObject *>("minuteComboBox");
    QObject *secondComboBox = wiztimeForm->findChild<QObject *>("secondComboBox");
    QString timezone = timezoneTextField->property("text").toString();
    QString ntpServer = ntpServerTextField->property("text").toString();
    QString date = dateTextField->property("text").toString();
    QString hour = hourComboBox->property("currentText").toString();
    QString minute = minuteComboBox->property("currentText").toString();
    QString second = secondComboBox->property("currentText").toString();
    QString datetime = QString("%1 %2:%3:%4").arg(date, hour, minute, second);
    bool setIsNTP = ntpRadioButton->property("checked").toBool();
    if (!ntpServer.isEmpty() && 
        !(is_valid_domain(ntpServer.toStdString().c_str()) || is_valid_ip_address(ntpServer.toStdString().c_str())) )
    {
        isSuccess = false;
        msg = "Please check that ntp server is valid domain/ip format.";
    }
    else
    {
        // set timezone
        auto result = this->m_timeUtil->set_timezone(timezone.toStdString().c_str());
        isSuccess &= result.second;
        // set sync with ntp
        result = this->m_timeUtil->set_sync_with_ntp_server(setIsNTP, ntpServer.toStdString().c_str());
        isSuccess &= result.second;
        if (!setIsNTP)
        {
            // set manual time
            result = this->m_timeUtil->set_manual_date_time(datetime.toStdString().c_str());
            isSuccess &= result.second;
        }
        msg = result.first;
        // save to config
        this->m_configUtil->set_timezone(timezone.toStdString().c_str());
        this->m_configUtil->set_ntp_enable(setIsNTP);
        this->m_configUtil->set_ntp_server(ntpServer.toStdString().c_str());
        this->m_configUtil->set_date(date.toStdString().c_str());
        this->m_configUtil->set_hour(hour.toInt());
        this->m_configUtil->set_minute(minute.toInt());
        this->m_configUtil->set_second(second.toInt());
    }
    if (!isSuccess)
    {
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
    this->m_wizardStatusMap[WIZARD_TIME] = isSuccess;
    return isSuccess;
}

bool QMLWindow::applyWizardScreenSetting(QObject *rootObject)
{
    bool isSuccess = true;
    // skip if not showed
    if (!this->m_wizardPageShowedMap[WIZARD_SCREEN])
        return isSuccess;

    this->m_wizardStatusMap[WIZARD_SCREEN] = isSuccess;
    return isSuccess;
}

bool QMLWindow::applyWizardStartupSetting(QObject *rootObject)
{
    bool isSuccess = true;
    // skip if not showed
    if (!this->m_wizardPageShowedMap[WIZARD_STARTUP])
        return isSuccess;
    string msg;
    QObject *wizstartupForm = rootObject->findChild<QObject *>("wizstartupForm");
    QObject *timeoutTextField = wizstartupForm->findChild<QObject *>("timeoutTextField");
    QObject *urlTextField = wizstartupForm->findChild<QObject *>("urlTextField");
    QObject *fileTextField = wizstartupForm->findChild<QObject *>("fileTextField");
    QObject *autoRestartSwitch = wizstartupForm->findChild<QObject *>("autoRestartSwitch");
    QObject *webPageRepeater = wizstartupForm->findChild<QObject *>("webPageRepeater");
    QObject *vncServerTextField = wizstartupForm->findChild<QObject *>("vncServerTextField");
    QObject *vncPasswordTextField = wizstartupForm->findChild<QObject *>("vncPasswordTextField");
    QObject *vncViewonlySwitch = wizstartupForm->findChild<QObject *>("vncViewonlySwitch");
    QObject *vncFullscreenSwitch = wizstartupForm->findChild<QObject *>("vncFullscreenSwitch");
    QObject *vncFitWindowSwitch = wizstartupForm->findChild<QObject *>("vncFitWindowSwitch");
    QObject *vncImageQualitySpinbox = wizstartupForm->findChild<QObject *>("vncImageQualitySpinbox");
    QString timeout = timeoutTextField->property("text").toString();
    QString url = urlTextField->property("text").toString();
    QString filepath = fileTextField->property("text").toString();
    QString vncFullServer = vncServerTextField->property("text").toString();
    QString vncPassword = vncPasswordTextField->property("text").toString();
    QString setVncImageQuality = vncImageQualitySpinbox->property("value").toString();
    bool setVncViewonly = vncViewonlySwitch->property("checked").toBool();
    bool setVncFullscreen = vncFullscreenSwitch->property("checked").toBool();
    bool setVncFitWindow = vncFitWindowSwitch->property("checked").toBool();
    bool setIsAutoRestart = autoRestartSwitch->property("checked").toBool();
    int webPageCount = webPageRepeater->property("count").toInt();
    int validWebPageCount = 0;
    map<string, string> webPageMap;
    QVariantMap retWebPageMap;
    QVariant retWebPageElement;
    QVariant retStartup;
    QMetaObject::invokeMethod(wizstartupForm, "getCurrentStartup",
                              Q_RETURN_ARG(QVariant, retStartup));

    if (retStartup.toString().compare(STARTUP_NAME_VNC_VIEWER) == 0)
    {
        // split address and port
        string address, port;
        QStringList splitList = vncFullServer.split(QLatin1Char(':'), Qt::SkipEmptyParts);
        if (splitList.size() > 0)
            address = splitList.at(0).toStdString();
        if (splitList.size() > 1)
            port = splitList.at(1).toStdString();
        if (address.empty())
        {
            isSuccess = false;
            msg = "Please enter VNC server hostname or ip address.";
        }
        else if (port.empty())
        {
            isSuccess = false;
            msg = "Please enter VNC server port.";
        }
        else if (!address.empty() && is_contains_special_characters(address.c_str()))
        {
            isSuccess = false;
            msg = "Please check that hostname or ip address is valid format.";
        }
        else if (!port.empty() && !is_numbers(port.c_str()))
        {
            isSuccess = false;
            msg = "Please check that port is valid number.";
        }
        else
        {
            // set vnc
            this->m_configUtil->set_vnc_server_address(address.c_str());
            this->m_configUtil->set_vnc_server_port(port.c_str());
            this->m_configUtil->set_vnc_server_password(vncPassword.toStdString().c_str());
            this->m_configUtil->set_vnc_server_viewonly(setVncViewonly);
            this->m_configUtil->set_vnc_server_fullscreen(setVncFullscreen);
            this->m_configUtil->set_vnc_server_fit_window(setVncFitWindow);
            this->m_configUtil->set_vnc_server_image_quality(setVncImageQuality.toInt());
        }
    }

    // set web page
    for (int i = 0; i < webPageCount; i++) {
        QMetaObject::invokeMethod(wizstartupForm, "getWebPageByIndex",
                                  Q_RETURN_ARG(QVariant, retWebPageElement),
                                  Q_ARG(QVariant, QVariant::fromValue(i)));
        retWebPageMap = retWebPageElement.toMap();
        // skip empty element
        if (retWebPageMap.value("page").toString().isEmpty()) {
            continue;
        }
        webPageMap["page"] = retWebPageMap.value("page").toString().toStdString();
        webPageMap["is_startup"] = retWebPageMap.value("is_startup").toString().toStdString();
        this->m_configUtil->set_web_page_by_index(webPageMap, validWebPageCount);
        validWebPageCount++;
    }
    this->m_configUtil->set_web_pages_count(validWebPageCount);
    // set startup
    this->m_configUtil->set_startup(retStartup.toString().toStdString().c_str());
    this->m_configUtil->set_static_page_timeout(timeout.toInt());
    this->m_configUtil->set_static_page_url(url.toStdString().c_str());
    this->m_configUtil->set_static_page_file_path(filepath.toStdString().c_str());
    this->m_configUtil->set_startup_auto_restart(setIsAutoRestart);

    if (!isSuccess)
    {
        this->showMessageDialog(rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
    }
    this->m_wizardStatusMap[WIZARD_STARTUP] = isSuccess;
    return isSuccess;
}

bool QMLWindow::applyWizard(QObject *rootObject)
{
    bool isSuccess = false;
    isSuccess = this->applyWizardNetworkSetting(this->m_rootObject);
    if (!isSuccess)
        return isSuccess;
    isSuccess = this->applyWizardTimeSetting(this->m_rootObject);
    if (!isSuccess)
        return isSuccess;
    isSuccess = this->applyWizardScreenSetting(this->m_rootObject);
    if (!isSuccess)
        return isSuccess;
    isSuccess = this->applyWizardStartupSetting(this->m_rootObject);
    if (!isSuccess)
        return isSuccess;
    isSuccess = this->applyCredentialsSetting(this->m_rootObject);
    if (!isSuccess)
        return isSuccess;
    return isSuccess;
}

void QMLWindow::moveToNextPage(QObject *rootObject, const string &currentPageName)
{
    string pageName = this->m_configUtil->get_next_page_by_name(currentPageName.c_str());
    // move to next page
    while (!pageName.empty()) {
        bool isShowed = this->m_configUtil->get_page_is_showed_by_name(pageName.c_str());
        if (isShowed) {
            break;
        }
        // get next page if page is not able to showed
        pageName = this->m_configUtil->get_next_page_by_name(pageName.c_str());
    }
    // hide wizard pages if backup config is exist
    const char* backupFile = ConfigUtility::get_backup_config_path();
    if (is_file_exist(backupFile)) {
        pageName.clear();
    }
    // empty means last page
    if (pageName.empty()) {
        bool isSuccess = this->applyWizard(rootObject);
        // finished
        if (isSuccess) {
            emit closeWindow();
        }
        return;
    }
    // press page button
    string pageButtonName = pageName.append("Button");
    QObject *sideBar = rootObject->findChild<QObject *>("sideBar");
    QObject *pageButton = sideBar->findChild<QObject *>(pageButtonName.c_str());
    pageButton->setProperty("checked", QVariant(true));

    // show next page
    QMetaObject::invokeMethod(sideBar, "showPage");
}

bool QMLWindow::downloadFTPFile(QObject *rootObject)
{
    bool isSuccess = true;
    QObject *ftpForm = rootObject->findChild<QObject *>("ftpForm");
    QObject *serverTextField = ftpForm->findChild<QObject *>("serverTextField");
    QObject *portTextField = ftpForm->findChild<QObject *>("portTextField");
    QObject *usernameTextField = ftpForm->findChild<QObject *>("usernameTextField");
    QObject *passwordTextField = ftpForm->findChild<QObject *>("passwordTextField");
    QObject *remotePathTextField = ftpForm->findChild<QObject *>("remotePathTextField");
    QObject *localPathTextField = ftpForm->findChild<QObject *>("localPathTextField");
    QString address = serverTextField->property("text").toString();
    QString port = portTextField->property("text").toString();
    QString username = usernameTextField->property("text").toString();
    QString password = passwordTextField->property("text").toString();
    QString remotePath = remotePathTextField->property("text").toString();
    QString localPath = localPathTextField->property("text").toString();

    this->m_configUtil->set_ftp_server_address(address.toStdString().c_str());
    this->m_configUtil->set_ftp_server_port(port.toStdString().c_str());
    this->m_configUtil->set_ftp_server_username(username.toStdString().c_str());
    this->m_configUtil->set_ftp_server_password(password.toStdString().c_str());
    this->m_configUtil->set_ftp_server_remote_path(remotePath.toStdString().c_str());
    this->m_configUtil->set_ftp_server_local_path(localPath.toStdString().c_str());
    isSuccess = this->m_ftpUtil->download_from_remote(
        address.toStdString().c_str(),
        port.toStdString().c_str(),
        username.toStdString().c_str(),
        password.toStdString().c_str(),
        remotePath.toStdString().c_str(),
        localPath.toStdString().c_str()
    );

    return isSuccess;
}

void QMLWindow::downloadIsFinished(bool isSuccess)
{
    this->showLoadingIndicator(this->m_rootObject, false);
    this->showMessageDialog(this->m_rootObject, isSuccess, nullptr, NONE_HANDLER_INDEX);
}

void QMLWindow::on_screen_toggled()
{
    this->initScreenWindowValue(this->m_rootObject);
}

void QMLWindow::on_network_toggled()
{
    this->initNetworkWindowValue(this->m_rootObject);
}

void QMLWindow::on_time_toggled()
{
    this->initTimeWindowValue(this->m_rootObject, this->m_timezones);
}

void QMLWindow::on_storage_toggled()
{
    this->initStorageWindowValue(this->m_rootObject);
}

void QMLWindow::on_system_toggled()
{
    this->initSystemWindowValue(this->m_rootObject);
}

void QMLWindow::on_security_toggled()
{
    this->initSecurityWindowValue(this->m_rootObject);
}

void QMLWindow::on_ftp_toggled()
{
    this->initFTPWindowValue(this->m_rootObject);
}

void QMLWindow::on_logo_toggled()
{
}

void QMLWindow::on_update_toggled()
{
    this->initUpdateWindowValue(this->m_rootObject);
}

void QMLWindow::on_operate_toggled()
{
    this->initOperateWindowValue(this->m_rootObject);
}

void QMLWindow::on_password_toggled()
{
    this->initPasswordWindowValue(this->m_rootObject);
}

void QMLWindow::on_about_toggled()
{
    this->initAboutWindowValue(this->m_rootObject);
}

void QMLWindow::on_networkWindow_applyButton1_clicked()
{
    this->applyNetworkSetting(this->m_rootObject, ETHERNET_0);
}

void QMLWindow::on_networkWindow_applyButton2_clicked()
{
    this->applyNetworkSetting(this->m_rootObject, ETHERNET_1);
}

void QMLWindow::on_networkWindow_firewallApplyButton_clicked()
{
    this->applyNetworkFirewallSetting(this->m_rootObject);
}

void QMLWindow::on_networkWindow_swipeView_changed()
{
    this->initNetworkWindowValue(this->m_rootObject);
}

void QMLWindow::on_timeWindow_applyButton_clicked()
{
    this->applyTimeSetting(this->m_rootObject);
}

void QMLWindow::on_systemWindow_swipeView_changed()
{
    this->initSystemWindowValue(this->m_rootObject);
}

void QMLWindow::on_systemWindow_startupApplyButton_clicked()
{
    this->applySystemStartupSetting(this->m_rootObject);
}

void QMLWindow::on_systemWindow_generalApplyButton_clicked()
{
    this->applySystemGeneralSetting(this->m_rootObject);
}

void QMLWindow::on_systemWindow_comApplyButton_clicked()
{
    this->applySystemCOMSetting(this->m_rootObject);
}

void QMLWindow::on_securityWindow_applyButton_clicked()
{
    this->applySecuritySetting(this->m_rootObject);
}

void QMLWindow::on_ftpWindow_downloadButton_clicked()
{
    // start loading
    this->showLoadingIndicator(this->m_rootObject, true);
    
    // get binded function pointer
    auto pDownloadFunction = std::bind(&QMLWindow::downloadFTPFile, this, this->m_rootObject);
    // run in worker thread prevent block UI thread
    this->m_workThread = new WorkerThread(pDownloadFunction, 1);
    connect(this->m_workThread, SIGNAL(workFinished(bool)),
            this, SLOT(downloadIsFinished(bool)));
    connect(this->m_workThread, &WorkerThread::finished,
            this->m_workThread, &QObject::deleteLater);
    this->m_workThread->start();
}

void QMLWindow::on_logoWindow_applyButton_clicked()
{
    this->applyLogoSetting(this->m_rootObject);
}

void QMLWindow::on_passwordWindow_applyButton_clicked()
{
    this->applyPasswordSetting(this->m_rootObject);
}

void QMLWindow::on_credentialsWindow_nextButton_clicked()
{
    bool isSuccess;
    bool isRootPasswordRequired = this->m_configUtil->get_root_password_required();
    bool isWestonPasswordRequired = this->m_configUtil->get_weston_password_required();
    if (isRootPasswordRequired)
    {
        isSuccess = this->checkCredentialsFields(this->m_rootObject, ROOT_USER);
        if (!isSuccess)
            return;
    }
    if (isWestonPasswordRequired)
    {
        isSuccess = this->checkCredentialsFields(this->m_rootObject, WESTON_USER);
        if (!isSuccess)
            return;
    }
    this->moveToNextPage(this->m_rootObject, WIZARD_CREDENTIALS);
}

void QMLWindow::on_wizardNetworkWindow_nextButton1_clicked()
{
    bool isSuccess = this->checkWizardEthernetNetworkFields(this->m_rootObject, ETHERNET_0);
    if (!isSuccess)
        return;
    QObject *wiznetworkForm = this->m_rootObject->findChild<QObject *>("wiznetworkForm");
    QObject *networkSwipeView = wiznetworkForm->findChild<QObject *>("networkSwipeView");
    // show next view
    QMetaObject::invokeMethod(networkSwipeView, "incrementCurrentIndex");
}

void QMLWindow::on_wizardNetworkWindow_nextButton2_clicked()
{
    bool isSuccess = this->checkWizardEthernetNetworkFields(this->m_rootObject, ETHERNET_0);
    if (!isSuccess)
        return;
    isSuccess = this->checkWizardEthernetNetworkFields(this->m_rootObject, ETHERNET_1);
    if (!isSuccess)
        return;
    this->moveToNextPage(this->m_rootObject, WIZARD_NETWORK);
}

void QMLWindow::on_wizardTimeWindow_nextButton_clicked()
{
    this->moveToNextPage(this->m_rootObject, WIZARD_TIME);
}

void QMLWindow::on_wizardScreenWindow_nextButton_clicked()
{
    this->moveToNextPage(this->m_rootObject, WIZARD_SCREEN);
}

void QMLWindow::on_wizardStartupWindow_nextButton_clicked()
{
    this->moveToNextPage(this->m_rootObject, WIZARD_STARTUP);
}

void QMLWindow::on_screenWindow_brightChanged_signal(int value)
{
    this->m_screenUtil->set_brightness(value);
    this->m_configUtil->set_brightness(value);
}

void QMLWindow::on_screenWindow_applyButton_clicked()
{
    this->applyScreenSetting(this->m_rootObject);
}

void QMLWindow::on_screenWindow_questionDialog_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();

    this->m_screenUtil->restart_desktop_service();
}

void QMLWindow::on_updateWindow_updateButton_clicked()
{
    bool isReadonly = this->m_systemUtil->get_readonly_mode();
    bool isBootFromSD = this->m_systemUtil->is_boot_from_sd_card();
    string msg;
    if (isReadonly)
    {
        // readonly mode cannot do swupdate
        msg = "Please disable read-only system first.";
    }
    if (isBootFromSD)
    {
        // boot from SD card cannot do swupdate
        msg = "Please boot from eMMC to use this feature. If you want to update SD card, you should use burn image tool.";
    }
    if (!msg.empty())
    {
        this->showMessageDialog(this->m_rootObject, false, &msg, NONE_HANDLER_INDEX);
    }
    else
    {
        msg = "This operation would reboot device. Do you want to continue?";
        this->showQuestionDialog(this->m_rootObject, msg, UPDATE_HANDLER_INDEX);
    }
}

void QMLWindow::on_updateWindow_questionDialog_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();

    // start loading
    this->showLoadingIndicator(this->m_rootObject, true);

    QObject *updateForm = this->m_rootObject->findChild<QObject *>("updateForm");
    QObject *fileTextField = updateForm->findChild<QObject *>("fileTextField");
    QObject *backupSettingsSwitch = updateForm->findChild<QObject *>("backupSettingsSwitch");
    QObject *backupUserSwitch = updateForm->findChild<QObject *>("backupUserSwitch");
    QString filepath = fileTextField->property("text").toString();
    QObject *updateStatusInfo = updateForm->findChild<QObject *>("updateStatusInfo");
    QCoreApplication *app = (QCoreApplication *)this->parent();
    bool setIsBackupSettings = backupSettingsSwitch->property("checked").toBool();
    bool setIsBackupUser = backupUserSwitch->property("checked").toBool();

    char dst_file_path[] = TMP_SWUPDATE_FILE;
    bool ready = false;    
    int downgrading;    

    if (filepath.isEmpty())
    {
        updateStatusInfo->setProperty("text", "please choose an update file(.swu).");
    }
    else if (is_contains_special_characters(filepath.toStdString().c_str()))
    {
        qDebug() << "invalid file src=" << filepath.toStdString().c_str();
        updateStatusInfo->setProperty("text", "the file contains invalid characters(& | ; $ > < ` \\ !), please rename it.");
    }
    else
    {
        this->m_configUtil->set_backup_config_enable(setIsBackupSettings);
        this->m_configUtil->set_backup_user_enable(setIsBackupUser);
        // backup settings
        if (setIsBackupSettings)
        {
            this->m_restoreUtility->export_config(this->m_configUtil->get_backup_config_path(), this->m_configUtil);
        }
        if (setIsBackupUser)
        {
            this->m_configUtil->backup_user_config();
        }
        qDebug() << "file src=" << filepath.toStdString().c_str();

        updateStatusInfo->setProperty("text", "System now check file version ,please wait....");
        app->processEvents();

        downgrading = this->m_updateUtil->check_downgrading((const char*)filepath.toStdString().c_str()); 
        if (downgrading != 0)   
        {
            this->showLoadingIndicator(this->m_rootObject, false);
            if ( downgrading == OLDER_VERSION )            
                updateStatusInfo->setProperty("text", "The version is lower than \"1.6\" , No downgrading allowed, installation fail ! \nplease choose another file");                      
            
            if (downgrading == NO_SW_DESC_FILE)
                updateStatusInfo->setProperty("text", "read file config failed.");
            
            return;
        }      
        // if swu file is from /userdata/, we don't copy
        if (filepath.startsWith("/userdata/"))
        {
            this->m_updateUtil->start_local_update(filepath.toStdString().c_str());
        }
        else // if swu file is not from /userdata/, we copy to /userdata/tmp_update.swu before update
        {
            if (QFile::exists(dst_file_path))
            {
                QFile::remove(dst_file_path);
            }

            QFile src(filepath.toStdString().c_str());
            QFile dst(dst_file_path);
            if (!src.open(QIODevice::ReadOnly) || !dst.open(QIODevice::WriteOnly))
            {
                qDebug() << "file open failed. src=" << filepath.toStdString().c_str() << ", dst=" << dst_file_path;
                updateStatusInfo->setProperty("text", "open file failed.");
            }
            else
            {
                qint64 len = src.bytesAvailable();
                if (!src.seek(0))
                {
                    updateStatusInfo->setProperty("text", "Error: The source file cannot set the current position to 0.");
                }
                else if (!dst.seek(0))
                {
                    updateStatusInfo->setProperty("text", "Error: The destination file cannot set the current position to 0.");
                }
                else
                {
                    qint64 writedsize = 0, readed = 0, copyed = 0, buff_size = 1024 * 1024;
                    do
                    {
                        QByteArray data = src.read(buff_size);
                        if (data.size() == 0)
                        {
                            updateStatusInfo->setProperty("text", "Error: The source file cannot read data.");
                            break;
                        }
                        writedsize = dst.write(data);
                        // write data error
                        if (writedsize == -1)
                        {
                            updateStatusInfo->setProperty("text", "Error: Write data error. please check /userdata usage.");
                            break;
                        }
                        // write data size != read data size
                        if (data.size() != writedsize)
                        {
                            updateStatusInfo->setProperty("text", "Error: write data abnormal. please try angin later.");
                            break;
                        }

                        copyed += writedsize;

                        if (copyed < len)
                        {
                            updateStatusInfo->setProperty("text", QString("Copyed: %1 Bytes, Total: %2 Bytes")
                                                                      .arg(copyed)
                                                                      .arg(len)
                                                                      .toStdString()
                                                                      .c_str());

                            app->processEvents();
                        }
                        else if (copyed == len)
                        {
                            updateStatusInfo->setProperty("text", "System will update after reboot. Please wait.....");
                            app->processEvents();
                            ready = true;
                            break;
                        }
                        else
                        {
                            updateStatusInfo->setProperty("text", "Error: write data more then read data. Please try angin later.");
                            break;
                        }
                    } while (true);
                }
                src.close();
                dst.close();
                if (ready)
                    this->m_updateUtil->start_local_update(dst_file_path);
            }
        }
    }

    this->showLoadingIndicator(this->m_rootObject, false);
}

void QMLWindow::on_loginDialog_okButton_clicked()
{
    string msg = "Incorrect password.";
    QObject *loginPopup = this->m_rootObject->findChild<QObject *>("loginPopup");
    QObject *setupPasswordLabel = loginPopup->findChild<QObject *>("setupPasswordLabel");
    QObject *usernamePopupTextField = loginPopup->findChild<QObject *>("usernamePopupTextField");
    QObject *passwordPopupTextField = loginPopup->findChild<QObject *>("passwordPopupTextField");
    bool isSetupPassword = setupPasswordLabel->property("visible").toBool();
    string input_username = usernamePopupTextField->property("text").toString().toStdString();
    string input_password = passwordPopupTextField->property("text").toString().toStdString();
    string alias_identity = this->m_configUtil->get_login_name_alias(input_username.c_str());
    // disconnect button handler
    QObject *loginPopupOKButton = loginPopup->findChild<QObject *>("loginPopupOKButton");
    QObject *loginPopupCancelButton = loginPopup->findChild<QObject *>("loginPopupCancelButton");
    QObject::disconnect(loginPopupOKButton, SIGNAL(clicked()), 0, 0);
    QObject::disconnect(loginPopupCancelButton, SIGNAL(clicked()), 0, 0);
    // close login popup
    QMetaObject::invokeMethod(loginPopup, "close");
    // verify password
    if (isSetupPassword) {
        // setup password
        if (input_password.empty()) {
            msg = "Please enter a password.";
        } else {
            this->m_configUtil->set_login_password(input_password.c_str());
            this->startWorker();
            return;
        }
    }
    else {
        if (this->m_loginType.compare(LOGIN_TYPE_SYS_USER) == 0) {
            if (input_username.empty()) {
                msg = "Please enter a username.";
            } else {
                // use alias identity to login
                if (!alias_identity.empty()) {
                    this->m_loginAliasName = input_username;
                    input_username = alias_identity;
                }
                auto ret = sys_auth_user(input_username.c_str(), input_password.c_str());
                if (ret.second == EXIT_SUCCESS) {
                    this->m_loginIdentity = input_username;
                    this->startWorker();
                    return;
                }
            }
        } else {
            if (input_password.compare(this->m_loginPassword) == 0) {
                this->startWorker();
                return;
            }
        }
    }
    this->showMessageDialog(this->m_rootObject, false, &msg, SHOW_LOGIN_HANDLER_INDEX);
}

void QMLWindow::on_loginDialog_cancelButton_clicked()
{
    // Emit signal when canceled
    emit closeWindow();
}

void QMLWindow::on_questionDialog_cancelButton_clicked()
{
    // disconnect ok button handler
    QObject *msgbox = this->m_rootObject->findChild<QObject *>("questionPopup");
    QObject *questionPopupOKButton = msgbox->findChild<QObject *>("questionPopupOKButton");
    QObject::disconnect(questionPopupOKButton, SIGNAL(clicked()), 0, 0);
    QMetaObject::invokeMethod(msgbox, "close");
}

void QMLWindow::on_messageDialog_button_clicked()
{
    // disconnect button handler
    QObject *msgbox = this->m_rootObject->findChild<QObject *>("messagePopup");
    QObject *msgboxButton = msgbox->findChild<QObject *>("messagePopupButton");
    QObject::disconnect(msgboxButton, SIGNAL(clicked()), 0, 0);
    QMetaObject::invokeMethod(msgbox, "close");
}

void QMLWindow::on_messageDialog_button_clicked_relogin()
{
    this->on_messageDialog_button_clicked();
    this->initLoginFlow(this->m_rootObject);
}

void QMLWindow::on_operateWindow_importFileDialog_accepted()
{
    QString strFilePrefix = "file://";
    QObject *operateForm = this->m_rootObject->findChild<QObject *>("operateForm");
    QObject *fileDialog = operateForm->findChild<QObject *>("fileDialog");
    QString filePath = fileDialog->property("file").toString().replace(strFilePrefix, "");
    if (filePath.isEmpty()) {
        filePath = fileDialog->property("selectedFile").toString().replace(strFilePrefix, "");
    }
    m_threadParameter = filePath.toStdString();
    QMetaObject::invokeMethod(fileDialog, "close");
    // start loading
    this->showLoadingIndicator(this->m_rootObject, true);
    
    // get binded function pointer
    auto pImportFunction = std::bind(
        &RestoreUtility::import_config, this->m_restoreUtility, m_threadParameter.c_str(), this->m_configUtil);
    // run in worker thread prevent block UI thread
    this->m_workThread = new WorkerThread(pImportFunction, 1);
    connect(this->m_workThread, SIGNAL(workFinishedWithResult(QString, bool)),
            this, SLOT(importConfigIsFinished(QString, bool)));
    connect(this->m_workThread, &WorkerThread::finished,
            this->m_workThread, &QObject::deleteLater);
    this->m_workThread->start();
}

void QMLWindow::on_operateWindow_exportFolderDialog_accepted()
{
    bool isSuccess = true;
    string msg;
    char exportFile[BUFF_SIZE]= {0};
    QString strFilePrefix = "file://";
    QString strSlash = "/";
    QObject *operateForm = this->m_rootObject->findChild<QObject *>("operateForm");
    QObject *folderDialog = operateForm->findChild<QObject *>("folderDialog");
    QString folderPath = folderDialog->property("folder").toString().replace(strFilePrefix, "");
    if (folderPath.isEmpty()) {
        folderPath = folderDialog->property("selectedFolder").toString().replace(strFilePrefix, "");
    }
    // make sure folder slash exist
    if (folderPath.indexOf(strSlash) != 0) {
        folderPath.insert(0, strSlash);
    }
    bool isConfig = folderDialog->property("isConfig").toBool();
    QMetaObject::invokeMethod(folderDialog, "close");
    
    if (isConfig) {
        snprintf(exportFile, BUFF_SIZE, "%s/%s", 
            folderPath.toStdString().c_str(), this->m_configUtil->get_export_config_filename());
        auto result = this->m_restoreUtility->export_config(exportFile, this->m_configUtil);
        isSuccess = result.second;
        if (isSuccess) {
            msg = QString("Export settings to %1 is success.").arg(result.first.c_str()).toStdString();
        }
    } else {
        auto result = this->m_screenUtil->export_screenshots(folderPath.toStdString().c_str());
        isSuccess = result.second;
        msg = result.first;
        if (isSuccess) {
            msg = QString("Export screenshots to %1 is success. Do you want to delete screenshots?").arg(folderPath).toStdString();
            this->showQuestionDialog(this->m_rootObject, msg, OPERATE_DELETE_SCREENSHOTS_HANDLER_INDEX);
            return;
        }
    }
    this->showMessageDialog(this->m_rootObject, isSuccess, &msg, NONE_HANDLER_INDEX);
}

void QMLWindow::on_operateWindow_rebootButton_clicked()
{
    this->showQuestionDialog(this->m_rootObject, "Are you sure you want to reboot your device?", OPERATE_REBOOT_HANDLER_INDEX);
}

void QMLWindow::on_operateWindow_shutdownButton_clicked()
{
    this->showQuestionDialog(this->m_rootObject, "Are you sure you want to shutdown your device?", OPERATE_SHUTDOWN_HANDLER_INDEX);
}

void QMLWindow::on_operateWindow_openTerminalButton_clicked()
{
    this->m_systemUtil->open_terminal();
}

void QMLWindow::on_operateWindow_factoryResetButton_clicked()
{
    bool isReadonly = this->m_systemUtil->get_readonly_mode();
    bool isBootFromSD = this->m_systemUtil->is_boot_from_sd_card();
    
    string msg;
    if (isReadonly)
    {
        // readonly mode cannot do swupdate
        msg = "Please disable read-only system first.";
    }
    if (isBootFromSD)
    {
        // boot from SD card cannot do swupdate
        msg = "Please boot from eMMC to use this feature.";
    }

    if (!msg.empty())
    {
        this->showMessageDialog(this->m_rootObject, false, &msg, NONE_HANDLER_INDEX);
    }
    else
    {
        msg = "Are you sure you want to factory reset your device? Factory reset will take a few minutes, do not turn off the device during the process.";
        this->showQuestionDialog(this->m_rootObject, msg, OPERATE_FACTORY_RESET_HANDLER_INDEX);
    }
}

void QMLWindow::on_operateWindow_exportScreenshotsButton_clicked()
{
    string msg;
    int count = this->m_screenUtil->get_screenshots_count();
    if (count == 0)
    {
        msg = "No screenshot. Unable to export screenshots.";
    }

    if (!msg.empty())
    {
        this->showMessageDialog(this->m_rootObject, false, &msg, NONE_HANDLER_INDEX);
    }
    else
    {
        QObject *operateForm = this->m_rootObject->findChild<QObject *>("operateForm");
        QMetaObject::invokeMethod(operateForm, "openFolderDialogForScreenshot");
    }
}

void QMLWindow::on_operateWindow_questionDialog_reboot_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();

    this->m_systemUtil->do_reboot();
    // close
    emit closeWindow();
}

void QMLWindow::on_operateWindow_questionDialog_shutdown_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();

    this->m_systemUtil->do_shutdown();
    // close
    emit closeWindow();
}

void QMLWindow::on_operateWindow_questionDialog_deleteScreenshots_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();
    
    this->m_screenUtil->delete_screenshots();
}

void QMLWindow::on_operateWindow_questionDialog_factoryReset_okButton_clicked()
{
    this->on_questionDialog_cancelButton_clicked();

    // start loading
    this->showLoadingIndicator(this->m_rootObject, true);

    QCoreApplication *app = (QCoreApplication *)this->parent();
    app->processEvents();

    bool isProcessRunning = this->m_updateUtil->check_factory_status();
    bool check = this->m_updateUtil->start_factory_check();
    if (isProcessRunning)
    {
        // can not perform program at the same time 
        string msg = "The factory reset has been performed.";
        this->showLoadingIndicator(this->m_rootObject, false);
        this->showMessageDialog(this->m_rootObject, false, &msg, NONE_HANDLER_INDEX);
        return;
    }
    if (!check)
    {
        string msg = "There is no legal file!! Unable to start the factory reset";
        this->showLoadingIndicator(this->m_rootObject, false);
        this->showMessageDialog(this->m_rootObject, false, &msg, NONE_HANDLER_INDEX);
        return;
    }
    
    // get binded function pointer
    auto pFactoryResetFunction = std::bind(&IUpdateUtility::start_factory_reset, this->m_updateUtil);
    // run in worker thread prevent block UI thread
    this->m_workThread = new WorkerThread(pFactoryResetFunction, 1);
    connect(this->m_workThread, &WorkerThread::finished,
            this->m_workThread, &QObject::deleteLater);
    this->m_workThread->start();
}

void QMLWindow::on_aboutWindow_licenseButton_clicked()
{
    this->m_systemUtil->open_license_page();
}

void QMLWindow::showMessageDialog(QObject *rootObject, bool isSuccess, string *customMessage, int handlerIndex)
{
    string message;
    if (isSuccess)
    {
        message = "Success!";
    }
    else
    {
        message = "Failed! Please check log at " LOG_FILE_NAME;
    }
    // using custom message
    if (customMessage && customMessage->length() > 0)
    {
        message = *customMessage;
    }

    QObject *msgbox = rootObject->findChild<QObject *>("messagePopup");
    QObject *msgboxText = msgbox->findChild<QObject *>("messagePopupText");
    QObject *msgboxButton = msgbox->findChild<QObject *>("messagePopupButton");
    msgboxText->setProperty("text", QVariant(QString::fromStdString(message)));
    // connect ok button handler
    switch (handlerIndex) {
        case SHOW_LOGIN_HANDLER_INDEX:
            {
                QObject::connect(msgboxButton, SIGNAL(clicked()),
                                this, SLOT(on_messageDialog_button_clicked_relogin()));
            }
            break;
        case NONE_HANDLER_INDEX:
        default:
            // do nothing
            break;
    }
    QMetaObject::invokeMethod(msgbox, "open");
}

void QMLWindow::showQuestionDialog(QObject *rootObject, string message, int handlerIndex)
{
    QObject *msgbox = rootObject->findChild<QObject *>("questionPopup");
    QObject *msgboxText = msgbox->findChild<QObject *>("questionPopupText");
    QObject *msgboxOKButton = msgbox->findChild<QObject *>("questionPopupOKButton");
    msgboxText->setProperty("text", QVariant(QString::fromStdString(message)));
    // connect ok button handler
    switch (handlerIndex) {
        case SCREEN_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_screenWindow_questionDialog_okButton_clicked()));
            }
            break;
        case SYSTEM_READONLY_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_systemWindow_questionDialog_readonly_okButton_clicked()));
            }
            break;
        case SYSTEM_USER_LOGIN_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_systemWindow_questionDialog_user_login_okButton_clicked()));
            }
            break;
        case UPDATE_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_updateWindow_questionDialog_okButton_clicked()));
            }
            break;
        case OPERATE_REBOOT_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_operateWindow_questionDialog_reboot_okButton_clicked()));
            }
            break;
        case OPERATE_SHUTDOWN_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_operateWindow_questionDialog_shutdown_okButton_clicked()));
            }
            break;
        case OPERATE_DELETE_SCREENSHOTS_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_operateWindow_questionDialog_deleteScreenshots_okButton_clicked()));
            }
            break;
        case OPERATE_FACTORY_RESET_HANDLER_INDEX:
            {
                QObject::connect(msgboxOKButton, SIGNAL(clicked()),
                                this, SLOT(on_operateWindow_questionDialog_factoryReset_okButton_clicked()));
            }
        case NONE_HANDLER_INDEX:
        default:
            // do nothing
            break;
    }
    QMetaObject::invokeMethod(msgbox, "open");
}

void QMLWindow::showLoadingIndicator(QObject *rootObject, bool isShow)
{
    QObject *msgbox = rootObject->findChild<QObject *>("loadingIndicator");
    if (isShow)
        QMetaObject::invokeMethod(msgbox, "open");
    else
        QMetaObject::invokeMethod(msgbox, "close");
}

void QMLWindow::showLoginDialog(QObject *rootObject, bool isLogin)
{
    QObject *loginPopup = rootObject->findChild<QObject *>("loginPopup");
    QObject *setupPasswordLabel = loginPopup->findChild<QObject *>("setupPasswordLabel");
    QObject *usernameLabel = loginPopup->findChild<QObject *>("usernameLabel");
    QObject *usernamePopupTextField = loginPopup->findChild<QObject *>("usernamePopupTextField");
    setupPasswordLabel->setProperty("visible", QVariant(!isLogin));
    if (this->m_loginType.compare(LOGIN_TYPE_SYS_USER) != 0) {
        usernameLabel->setProperty("visible", QVariant(false));
        usernamePopupTextField->setProperty("visible", QVariant(false));
    }
    // connect button handler
    QObject *loginPopupOKButton = loginPopup->findChild<QObject *>("loginPopupOKButton");
    QObject *loginPopupCancelButton = loginPopup->findChild<QObject *>("loginPopupCancelButton");
    QObject::connect(loginPopupOKButton, SIGNAL(clicked()),
                     this, SLOT(on_loginDialog_okButton_clicked()));
    QObject::connect(loginPopupCancelButton, SIGNAL(clicked()),
                     this, SLOT(on_loginDialog_cancelButton_clicked()));
    QMetaObject::invokeMethod(loginPopup, "open");
}

void QMLWindow::startNetworkMonitor()
{
    // stop previous
    this->stopNetworkMonitor();

    this->m_networkUtil->start_ip_monitor();
    this->m_watcher = new QFileSystemWatcher(this);
    QObject::connect(this->m_watcher, SIGNAL(fileChanged(const QString &)), 
                     this, SLOT(ipMonitorFileChangedEvent(const QString &)));
    this->m_watcher->addPath(MONITOR_FILE);
}

void QMLWindow::stopNetworkMonitor()
{
    this->m_networkUtil->stop_ip_monitor();
    if (this->m_watcher) {
        QObject::disconnect(this->m_watcher, SIGNAL(fileChanged(const QString &)), 0, 0);
        delete this->m_watcher;
        this->m_watcher = nullptr;
    }
}

void QMLWindow::ipMonitorFileChangedEvent(const QString &path)
{
    const auto retUPeth = this->m_networkUtil->get_up_ethernet_from_monitor_file(path.toStdString().c_str());
    // skip down event
    if (retUPeth.first.empty()) {
        return;
    }

    bool has_configured = this->m_configUtil->get_net_has_configured(retUPeth.first.c_str());
    // make sure get connmanctl network name for ethernet first time UP
    auto reteth = this->m_networkUtil->get_ethernet_status_until_timeout(retUPeth.first.c_str());
    if (!reteth.second) {
        return;
    }
    // if not configured, auto set when ethernet is online
    if (!has_configured)
    {
        bool isSuccess = true;
        string empty;
        // delete provisioning file first
        this->m_networkUtil->delete_offline_provisioning_file(retUPeth.first.c_str());
        string method = this->m_configUtil->get_net_method(retUPeth.first.c_str());
        qDebug("%s auto configure method:%s connmanctl is online:%d", retUPeth.first.c_str(), method.c_str(), reteth.second);
        if (method.compare(MODE_DHCP) == 0)
        {
            // clear static dns server for getting from dhcp
            isSuccess &= this->m_networkUtil->set_dns_server(retUPeth.first.c_str(),
                                                             empty.c_str(),
                                                             empty.c_str());
            // set dhcp
            isSuccess &= this->m_networkUtil->set_dhcp(retUPeth.first.c_str(), true);

            if (isSuccess)
            {
                this->m_configUtil->set_net_has_configured(retUPeth.first.c_str(), true);
            }
        }
        else if (method.compare(MODE_MANUAL) == 0)
        {
            string ip = this->m_configUtil->get_net_ip_address(retUPeth.first.c_str());
            string networkMask = this->m_configUtil->get_net_subnet_mask(retUPeth.first.c_str());
            string defaultGateway = this->m_configUtil->get_net_gateway(retUPeth.first.c_str());
            vector<string> nameservers = this->m_configUtil->get_net_dns_servers(retUPeth.first.c_str());
            string dns1, dns2;
            if (nameservers.size() > 0)
                dns1 = nameservers.at(0);
            if (nameservers.size() > 1)
                dns2 = nameservers.at(1);
            // set static ip
            isSuccess &= this->m_networkUtil->set_static_ip_address(retUPeth.first.c_str(),
                                                                    ip.c_str(),
                                                                    nullptr,
                                                                    networkMask.c_str(),
                                                                    defaultGateway.c_str());
            isSuccess &= this->m_networkUtil->set_dns_server(retUPeth.first.c_str(),
                                                             dns1.c_str(),
                                                             dns2.c_str());
            if (isSuccess)
            {
                this->m_configUtil->set_net_has_configured(retUPeth.first.c_str(), true);
            }
        }
    }
}
