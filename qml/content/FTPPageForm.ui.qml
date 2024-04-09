// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SettingsGUI 1.0
import "./controls"

Item {
    id: root
    width: Constants.pageWidth
    height: Constants.pageHeight

    property alias showPasswordSwitch: showPasswordSwitch
    property alias passwordTextField: passwordTextField
    property alias localPathTextField: localPathTextField
    property alias selectButton: selectButton

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    ColumnLayout {
        spacing: Constants.splitMargin
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        GridLayout {
            id: grid1
            columns: 2
            rowSpacing: 10
            columnSpacing: 10
            Layout.alignment: Qt.AlignTop

            ScreenLabel {
                text: qsTr("FTP Server: ")
            }
            GeneralTextField {
                id: serverTextField
                objectName: "serverTextField"
            }

            ScreenLabel {
                text: qsTr("FTP Port: ")
            }
            GeneralTextField {
                id: portTextField
                objectName: "portTextField"
            }

            ScreenLabel {
                text: qsTr("Username: ")
            }
            GeneralTextField {
                id: usernameTextField
                objectName: "usernameTextField"
            }

            ScreenLabel {
                text: qsTr("Password: ")
            }
            PasswordTextField {
                id: passwordTextField
                objectName: "passwordTextField"
            }

            ScreenLabel {
                text: qsTr("Show Password ")
            }
            NetworkSwitch {
                id: showPasswordSwitch
                objectName: "showPasswordSwitch"
            }

            ScreenLabel {
                text: qsTr("Remote File Path: ")
            }
            ScrollTextField {
                id: remotePathTextField
                objectName: "remotePathTextField"
            }

            ScreenLabel {
                text: qsTr("Local Path: ")
            }
            GeneralTextField {
                id: localPathTextField
                objectName: "localPathTextField"
            }

            Item {
                height: Constants.baseMargin
            }
            NetworkButton {
                id: selectButton
                objectName: "selectButton"
                implicitWidth: Constants.largeButtonWidth
                text: qsTr("Select Local Path")
            }
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight

            NetworkButton {
                id: downloadButton
                text: qsTr("Get")
                objectName: "downloadButton"
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
