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

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    ColumnLayout {
        spacing: Constants.splitMargin
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        GroupBox {
            GridLayout {
                columns: 2
                rowSpacing: Constants.itemMargin
                columnSpacing: Constants.itemMargin
                Layout.alignment: Qt.AlignTop

                ScreenLabel {
                    id: changePasswordLabel
                    objectName: "changePasswordLabel"
                    text: qsTr("Change password")
                    Layout.columnSpan: 2
                }

                ScreenLabel {
                    id: currentPasswordLabel
                    objectName: "currentPasswordLabel"
                    text: qsTr("Current Password: ")
                }
                PasswordTextField {
                    id: currentPasswordTextField
                    objectName: "currentPasswordTextField"
                }

                ScreenLabel {
                    text: qsTr("New Password: ")
                }
                PasswordTextField {
                    id: passwordTextField
                    objectName: "passwordTextField"
                }

                ScreenLabel {
                    text: qsTr("Confirm Password: ")
                }
                PasswordTextField {
                    id: confirmPasswordTextField
                    objectName: "confirmPasswordTextField"
                }
            }
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight

            NetworkButton {
                id: applyButton
                objectName: "applyButton"
                text: qsTr("Apply")
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
