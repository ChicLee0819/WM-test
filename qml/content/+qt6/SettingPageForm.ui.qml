// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs
import QtCore
import SettingsGUI 1.0
import "./controls"
import "./dialog"
import "./wizard"

Item {
    id: root

    width: Constants.screenWidth
    height: Constants.screenHeight

    property alias loadingIndicator: loadingIndicator
    property alias loadingIndicatorTimer: loadingIndicatorTimer
    property alias fileDialog: fileDialog
    property alias folderDialog: folderDialog
    property alias calendarPopup: calendarPopup
    property alias dateCalendar: dateCalendar
    property alias arrowButton: arrowButton
    property alias sideBar: sideBar
    property alias drawerSideBar: drawerSideBar
    property alias credentialsForm: credentialsForm
    property alias wiznetworkForm: wiznetworkForm
    property alias wiztimeForm: wiztimeForm
    property alias wizscreenForm: wizscreenForm
    property alias wizstartupForm: wizstartupForm
    property alias screenForm: screenForm
    property alias networkForm: networkForm
    property alias timeForm: timeForm
    property alias storageForm: storageForm
    property alias systemForm: systemForm
    property alias securityForm: securityForm
    property alias opcuaForm: opcuaForm
    property alias ftpForm: ftpForm
    property alias logoForm: logoForm
    property alias updateForm: updateForm
    property alias operateForm: operateForm
    property alias passwordForm: passwordForm
    property alias aboutForm: aboutForm

    Rectangle {
        anchors.fill: parent
        color: appPalette.sideBGColor
    }

    SideBar {
        id: sideBar
        objectName: "sideBar"
    }

    DrawerSideBar {
        id: drawerSideBar
        objectName: "drawerSideBar"
    }

    ArrowButton {
        id: arrowButton
        z: 1
        anchors.left: root.left
        anchors.verticalCenter: root.verticalCenter
        visible: Constants.inPortrait
    }

    CredentialsPage {
        id: credentialsForm
        objectName: "credentialsForm"
        anchors.left: sideBar.right
        visible: false
    }

    WizNetworkPage {
        id: wiznetworkForm
        objectName: "wiznetworkForm"
        anchors.left: sideBar.right
        visible: false
    }

    WizTimePage {
        id: wiztimeForm
        objectName: "wiztimeForm"
        anchors.left: sideBar.right
        visible: false
    }

    WizScreenPage {
        id: wizscreenForm
        objectName: "wizscreenForm"
        anchors.left: sideBar.right
        visible: false
    }

    WizStartupPage {
        id: wizstartupForm
        objectName: "wizstartupForm"
        anchors.left: sideBar.right
        visible: false
    }

    ScreenPage {
        id: screenForm
        objectName: "screenForm"
        anchors.left: sideBar.right
        visible: true
    }

    NetworkPage {
        id: networkForm
        objectName: "networkForm"
        anchors.left: sideBar.right
        visible: false
    }

    TimePage {
        id: timeForm
        objectName: "timeForm"
        anchors.left: sideBar.right
        visible: false
    }

    StoragePage {
        id: storageForm
        objectName: "storageForm"
        anchors.left: sideBar.right
        visible: false
    }

    SystemPage {
        id: systemForm
        objectName: "systemForm"
        anchors.left: sideBar.right
        visible: false
    }

    SecurityPage {
        id: securityForm
        objectName: "securityForm"
        anchors.left: sideBar.right
        visible: false
    }

    OPCUAPage {
        id: opcuaForm
        objectName: "opcuaForm"
        anchors.left: sideBar.right
        visible: false
    }

    FTPPage {
        id: ftpForm
        objectName: "ftpForm"
        anchors.left: sideBar.right
        visible: false
    }

    LogoPage {
        id: logoForm
        objectName: "logoForm"
        anchors.left: sideBar.right
        visible: false
    }

    UpdatePage {
        id: updateForm
        objectName: "updateForm"
        anchors.left: sideBar.right
        visible: false
    }

    OperatePage {
        id: operateForm
        objectName: "operateForm"
        anchors.left: sideBar.right
        visible: false
    }

    PasswordPage {
        id: passwordForm
        objectName: "passwordForm"
        anchors.left: sideBar.right
        visible: false
    }

    AboutPage {
        id: aboutForm
        objectName: "aboutForm"
        anchors.left: sideBar.right
        visible: false
    }

    // global dialog
    FileDialog {
        id: fileDialog
        objectName: "fileDialog"
        title: "Please choose a file"
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        options: FileDialog.ReadOnly | FileDialog.DontResolveSymlinks
    }

    FolderDialog {
        id: folderDialog
        objectName: "folderDialog"
        title: "Please choose a folder"
        currentFolder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        options: FolderDialog.ReadOnly | FolderDialog.DontResolveSymlinks
    }

    // global message box
    MessagePopup {
        id: messagePopup
        objectName: "messagePopup"
        anchors.centerIn: parent
    }

    QuestionDialog {
        id: questionPopup
        objectName: "questionPopup"
        anchors.centerIn: parent
    }

    LoginPopup {
        id: loginPopup
        objectName: "loginPopup"
        anchors.centerIn: parent
    }

    Popup {
        id: loadingIndicator
        objectName: "loadingIndicator"
        anchors.centerIn: Overlay.overlay
        closePolicy: Popup.NoAutoClose
        modal: true

        background: Rectangle {
            color: "transparent"
        }

        BusyIndicator {
            palette.dark: appPalette.btnTextColor
            running: true
        }
    }
    Timer {
        id: loadingIndicatorTimer
    }

    // popup calendar
    Popup {
        id: calendarPopup
        anchors.centerIn: parent
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent

        contentItem: AdvantechCalendar {
            id: dateCalendar
            selectedDate: new Date()
        }
    }
}
