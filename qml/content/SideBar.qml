// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15

SideBarForm {
    id: root
    property string currentTab: qsTr("Screen")
    signal tabSelected

    function showPage() {
        credentialsForm.visible = credentialsButton.checked
        wiznetworkForm.visible = wiznetworkButton.checked
        wiztimeForm.visible = wiztimeButton.checked
        wizscreenForm.visible = wizscreenButton.checked
        wizstartupForm.visible = wizstartupButton.checked
        screenForm.visible = screenButton.checked
        networkForm.visible = networkButton.checked
        timeForm.visible = timeButton.checked
        storageForm.visible = storageButton.checked
        systemForm.visible = systemButton.checked
        securityForm.visible = securityButton.checked
        opcuaForm.visible = opcuaButton.checked
        ftpForm.visible = ftpButton.checked
        logoForm.visible = logoButton.checked
        updateForm.visible = updateButton.checked
        operateForm.visible = operateButton.checked
        passwordForm.visible = passwordButton.checked
        aboutForm.visible = aboutButton.checked
    }

    credentialsButton.onClicked: {
        root.currentTab = qsTr("Credentials")
        root.tabSelected()

        showPage()
    }

    wiznetworkButton.onClicked: {
        root.currentTab = qsTr("Network")
        root.tabSelected()

        showPage()
    }

    wiztimeButton.onClicked: {
        root.currentTab = qsTr("Time")
        root.tabSelected()

        showPage()
    }

    wizscreenButton.onClicked: {
        root.currentTab = qsTr("Screen")
        root.tabSelected()

        showPage()
    }

    wizstartupButton.onClicked: {
        root.currentTab = qsTr("Startup")
        root.tabSelected()

        showPage()
    }

    screenButton.onClicked: {
        root.currentTab = qsTr("Screen")
        root.tabSelected()

        showPage()
    }

    networkButton.onClicked: {
        root.currentTab = qsTr("Network")
        root.tabSelected()

        showPage()
    }

    timeButton.onClicked: {
        root.currentTab = qsTr("Time")
        root.tabSelected()

        showPage()
    }

    storageButton.onClicked: {
        root.currentTab = qsTr("Storage")
        root.tabSelected()

        showPage()
    }

    systemButton.onClicked: {
        root.currentTab = qsTr("System")
        root.tabSelected()

        showPage()
    }

    securityButton.onClicked: {
        root.currentTab = qsTr("Security")
        root.tabSelected()

        showPage()
    }

    ftpButton.onClicked: {
        root.currentTab = qsTr("FTP")
        root.tabSelected()

        showPage()
    }

    logoButton.onClicked: {
        root.currentTab = qsTr("Logo")
        root.tabSelected()

        showPage()
    }

    opcuaButton.onClicked: {
        root.currentTab = qsTr("OPC UA")
        root.tabSelected()

        showPage()
    }

    updateButton.onClicked: {
        root.currentTab = qsTr("Update")
        root.tabSelected()

        showPage()
    }

    operateButton.onClicked: {
        root.currentTab = qsTr("Operate")
        root.tabSelected()

        showPage()
    }

    passwordButton.onClicked: {
        root.currentTab = qsTr("Password")
        root.tabSelected()

        showPage()
    }

    aboutButton.onClicked: {
        root.currentTab = qsTr("About")
        root.tabSelected()

        showPage()
    }

    exitButton.onClicked: {
        Qt.quit();
    }

    // these functions call from c++
    function initAppModeUI() {
        screenButton.checked = true
        screenButton.clicked()
    }

    function initModeUI() {
        credentialsButton.checked = true
        credentialsButton.clicked()
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
