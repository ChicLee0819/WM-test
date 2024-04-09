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

    property alias opcuaEnableSwitch: opcuaEnableSwitch
    property alias opcuaPortTextField: opcuaPortTextField
    property alias identityTypeComboBox: identityTypeComboBox
    property alias identityTypeModel: identityTypeModel
    property alias usernameGroup: usernameGroup
    property alias showPasswordSwitch: showPasswordSwitch
    property alias usernameTextField: usernameTextField
    property alias passwordTextField: passwordTextField

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
                text: qsTr("OPC UA Server Enable ")
            }
            NetworkSwitch {
                id: opcuaEnableSwitch
                objectName: "opcuaEnableSwitch"
            }

            ScreenLabel {
                text: qsTr("OPC UA Server Port: ")
            }
            NumberTextField {
                id: opcuaPortTextField
                objectName: "opcuaPortTextField"
                text: "4840"
            }

            ScreenLabel {
                text: qsTr("Identity Token: ")
            }
            TimeComboBox {
                id: identityTypeComboBox
                objectName: "identityTypeComboBox"
                textRole: "text"
                implicitWidth: Constants.middleComboBoxWidth
                model: ListModel {
                    id: identityTypeModel

                    ListElement {
                        text: "Anonymous"
                        value: "Anonymous"
                    }

                    ListElement {
                        text: "Username/Password"
                        value: "Username"
                    }
                }
            }
        }

        GroupBox {
            id: usernameGroup
            title: ""

            GridLayout {
                columns: 2
                rowSpacing: 10
                columnSpacing: 10
                Layout.alignment: Qt.AlignTop

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
            }
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
