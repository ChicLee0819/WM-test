// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SettingsGUI 1.0
import "../controls"

Item {
    id: root
    width: Constants.pageWidth
    height: Constants.pageHeight

    property alias rootPasswordTextField: rootPasswordTextField
    property alias rootConfirmPasswordTextField: rootConfirmPasswordTextField

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    ColumnLayout {
        spacing: Constants.splitMargin
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        GroupBox {
            id: rootGroup
            objectName: "rootGroup"
            GridLayout {
                columns: 2
                rowSpacing: Constants.itemMargin
                columnSpacing: Constants.itemMargin
                Layout.alignment: Qt.AlignTop

                ScreenLabel {
                    text: qsTr("Setup root password")
                    Layout.columnSpan: 2
                }

                ScreenLabel {
                    text: qsTr("New Password: ")
                }
                PasswordTextField {
                    id: rootPasswordTextField
                    objectName: "rootPasswordTextField"
                }

                ScreenLabel {
                    text: qsTr("Confirm Password: ")
                }
                PasswordTextField {
                    id: rootConfirmPasswordTextField
                    objectName: "rootConfirmPasswordTextField"
                }
            }
        }

        GroupBox {
            id: westonGroup
            objectName: "westonGroup"

            GridLayout {
                columns: 2
                rowSpacing: Constants.itemMargin
                columnSpacing: Constants.itemMargin
                Layout.alignment: Qt.AlignTop

                ScreenLabel {
                    text: qsTr("Setup user password")
                    Layout.columnSpan: 2
                }

                ScreenLabel {
                    id: westonCurrentPasswordLabel
                    objectName: "westonCurrentPasswordLabel"
                    text: qsTr("Current Password: ")
                }
                PasswordTextField {
                    id: westonCurrentPasswordTextField
                    objectName: "westonCurrentPasswordTextField"
                }

                ScreenLabel {
                    text: qsTr("New Password: ")
                }
                PasswordTextField {
                    id: westonPasswordTextField
                    objectName: "westonPasswordTextField"
                }

                ScreenLabel {
                    text: qsTr("Confirm Password: ")
                }
                PasswordTextField {
                    id: westonConfirmPasswordTextField
                    objectName: "westonConfirmPasswordTextField"
                }
            }
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight

            NetworkButton {
                id: nextButton
                objectName: "nextButton"
                text: qsTr("Next")
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
