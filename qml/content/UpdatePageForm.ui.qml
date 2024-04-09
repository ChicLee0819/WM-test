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

    property alias selectButton: selectButton
    property alias fileTextField: fileTextField
    property alias backupSettingsSwitch: backupSettingsSwitch
    property alias backupUserSwitch: backupUserSwitch

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    ColumnLayout {
        spacing: Constants.splitMargin
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        ScreenLabel {
            text: qsTr("Update steps:")
        }

        ScreenLabel {
            text: qsTr("1. Upload image file to device or plug USB.")
            Layout.preferredWidth: parent.width
        }

        ScreenLabel {
            text: qsTr("2. Click select button to select image file.")
            Layout.preferredWidth: parent.width
        }

        RowLayout {
            spacing: Constants.splitMargin

            GeneralTextField {
                id: fileTextField
                objectName: "fileTextField"
                readOnly: true
            }
            NetworkButton {
                id: selectButton
                objectName: "selectButton"
                text: qsTr("Select")
            }
        }

        ScreenLabel {
            text: qsTr("3. Enable backup settings if you want to keep settings after update.")
            Layout.preferredWidth: parent.width
        }
        RowLayout {
            spacing: Constants.splitMargin

            ScreenLabel {
                text: qsTr("Backup Settings")
            }
            NetworkSwitch {
                id: backupSettingsSwitch
                objectName: "backupSettingsSwitch"
            }
        }
        RowLayout {
            spacing: Constants.splitMargin

            ScreenLabel {
                text: qsTr("Backup System User")
            }
            NetworkSwitch {
                id: backupUserSwitch
                objectName: "backupUserSwitch"
            }
        }

        ScreenLabel {
            text: qsTr("4. Click update button and wait until it finishes.")
            Layout.preferredWidth: parent.width
        }

        Item {
            height: Constants.baseMargin
        }

        NetworkButton {
            id: updateButton
            objectName: "updateButton"
            text: qsTr("Update")
        }

        ScreenLabel {
            id: updateStatusInfo
            objectName: "updateStatusInfo"
            text: qsTr("")
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
