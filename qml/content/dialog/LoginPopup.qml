// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import "../controls"

Popup {
    id: control
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose

    contentItem: Pane {
        ColumnLayout {
            anchors.fill: parent

            ScreenLabel {
                id: setupPasswordLabel
                objectName: "setupPasswordLabel"
                text: qsTr("Please setup admin password for Settings tool")
                color: "black"
            }

            GridLayout {
                columns: 2
                rowSpacing: 10
                columnSpacing: 10
                Layout.alignment: Qt.AlignHCenter

                ScreenLabel {
                    id: usernameLabel
                    objectName: "usernameLabel"
                    text: qsTr("Username: ")
                    color: "black"
                }
                GeneralTextField {
                    id: usernamePopupTextField
                    objectName: "usernamePopupTextField"
                }

                ScreenLabel {
                    id: passwordLabel
                    objectName: "passwordLabel"
                    text: qsTr("Password: ")
                    color: "black"
                }
                PasswordTextField {
                    id: passwordPopupTextField
                    objectName: "passwordPopupTextField"
                }
            }

            RowLayout {
                spacing: 10
                Layout.alignment: Qt.AlignHCenter

                Button {
                    id: loginPopupOKButton
                    objectName: "loginPopupOKButton"
                    text: qsTr("OK")
                    Layout.alignment: Qt.AlignHCenter
                }

                Button {
                    id: loginPopupCancelButton
                    objectName: "loginPopupCancelButton"
                    text: qsTr("Cancel")
                    Layout.alignment: Qt.AlignHCenter
                    onClicked: {
                        control.close()
                    }
                }
            }
        }
    }
}
