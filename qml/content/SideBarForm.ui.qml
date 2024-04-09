// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SettingsGUI 1.0
import "./controls"

Item {
    width: Constants.inPortrait ? 0 : Constants.leftSideBarWidth
    height: Constants.inPortrait ? 0 : Constants.screenHeight

    visible: !Constants.inPortrait

    property alias credentialsButton: credentialsButton
    property alias wiznetworkButton: wiznetworkButton
    property alias wiztimeButton: wiztimeButton
    property alias wizscreenButton: wizscreenButton
    property alias wizstartupButton: wizstartupButton
    property alias screenButton: screenButton
    property alias networkButton: networkButton
    property alias timeButton: timeButton
    property alias storageButton: storageButton
    property alias systemButton: systemButton
    property alias securityButton: securityButton
    property alias opcuaButton: opcuaButton
    property alias ftpButton: ftpButton
    property alias logoButton: logoButton
    property alias updateButton: updateButton
    property alias operateButton: operateButton
    property alias passwordButton: passwordButton
    property alias aboutButton: aboutButton
    property alias exitButton: exitButton

    Rectangle {
        anchors.fill: parent
        color: appPalette.sideBGColor
    }

    Flickable {
        width: parent.width
        height: parent.height
        contentHeight: Constants.sideBarButtonHeight * Constants.sideBarButtonCount
        clip: true

        ColumnLayout {
            spacing: 0
            anchors.fill: parent

            ButtonGroup {
                id: btnGroup
            }

            SideBarButton {
                id: credentialsButton
                objectName: "credentialsButton"
                text: qsTr("User")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: wiznetworkButton
                objectName: "wiznetworkButton"
                text: qsTr("Network")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: wiztimeButton
                objectName: "wiztimeButton"
                text: qsTr("Time")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: wizscreenButton
                objectName: "wizscreenButton"
                text: qsTr("Screen")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: wizstartupButton
                objectName: "wizstartupButton"
                text: qsTr("Startup")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: screenButton
                objectName: "screenButton"
                text: qsTr("Screen")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: networkButton
                objectName: "networkButton"
                text: qsTr("Network")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: timeButton
                objectName: "timeButton"
                text: qsTr("Time")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: storageButton
                objectName: "storageButton"
                text: qsTr("Storage")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: systemButton
                objectName: "systemButton"
                text: qsTr("System")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: securityButton
                objectName: "securityButton"
                text: qsTr("Security")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: opcuaButton
                objectName: "opcuaButton"
                text: qsTr("OPC UA")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: ftpButton
                objectName: "ftpButton"
                text: qsTr("FTP")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: logoButton
                objectName: "logoButton"
                text: qsTr("Logo")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: updateButton
                objectName: "updateButton"
                text: qsTr("Update")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: operateButton
                objectName: "operateButton"
                text: qsTr("Operate")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: passwordButton
                objectName: "passwordButton"
                text: qsTr("Password")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: aboutButton
                objectName: "aboutButton"
                text: qsTr("About")
                ButtonGroup.group: btnGroup
            }

            SideBarButton {
                id: exitButton
                objectName: "exitButton"
                text: qsTr("Exit")
                ButtonGroup.group: btnGroup
            }

            Item {
                Layout.fillHeight: parent.height

                // make right border
                Rectangle {
                    implicitWidth: Constants.borderWidth
                    implicitHeight: parent.height
                    anchors.bottom: parent.bottom
                    anchors.top: parent.top
                    anchors.right: parent.right
                }
            }
        }
    }
}
