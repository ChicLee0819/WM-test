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

    property alias enableSwitch: enableSwitch
    property alias showPasswordSwitch: showPasswordSwitch
    property alias passwordTextField: passwordTextField
    property alias passwordRadioButton: passwordRadioButton
    property alias systemUserRadioButton: systemUserRadioButton

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    // radio button group
    ButtonGroup {
        id: radioGroup
    }

    ColumnLayout {
        spacing: Constants.splitMargin
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        GridLayout {
            id: grid1
            columns: 2
            rowSpacing: Constants.itemMargin
            columnSpacing: Constants.itemMargin
            Layout.alignment: Qt.AlignTop

            ScreenLabel {
                text: qsTr("Enable ")
            }
            NetworkSwitch {
                id: enableSwitch
                objectName: "enableSwitch"
            }
        }

        TimeRadioButton {
            id: passwordRadioButton
            objectName: "passwordRadioButton"
            text: qsTr("Authentication with Password")
            checked: true
            ButtonGroup.group: radioGroup
        }

        GridLayout {
            id: grid2
            columns: 2
            rowSpacing: Constants.itemMargin
            columnSpacing: Constants.itemMargin
            Layout.alignment: Qt.AlignTop
            Layout.leftMargin: Constants.indentMargin

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
        }

        TimeRadioButton {
            id: systemUserRadioButton
            objectName: "systemUserRadioButton"
            text: qsTr("Authentication with system user")
            ButtonGroup.group: radioGroup
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight

            NetworkButton {
                id: applyButton
                text: qsTr("Apply")
                objectName: "applyButton"
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
