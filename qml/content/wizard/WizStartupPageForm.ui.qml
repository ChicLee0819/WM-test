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

    property alias startupComboBox: startupComboBox
    property alias startupModel: startupModel
    property alias vncViewerGroup: vncViewerGroup
    property alias vncShowPasswordSwitch: vncShowPasswordSwitch
    property alias vncPasswordTextField: vncPasswordTextField
    property alias staticPageGroup: staticPageGroup
    property alias staticPageCustomGroup: staticPageCustomGroup
    property alias autoRestartGrid: autoRestartGrid
    property alias selectButton: selectButton
    property alias fileTextField: fileTextField
    property alias webPageGroup: webPageGroup
    property alias webPageModel: webPageModel
    property alias webPageRepeater: webPageRepeater
    property alias addWebPageButton: addWebPageButton

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    // radio button group
    ButtonGroup {
        id: radioGroup
    }

    Flickable {
        id: firstPage
        anchors.fill: parent
        contentWidth: parent.width
        contentHeight: firstContent.implicitHeight
        clip: true
        ScrollBar.vertical: ScrollBar {}

        ColumnLayout {
            id: firstContent
            spacing: Constants.splitMargin
            anchors.fill: parent
            anchors.leftMargin: Constants.baseMargin
            anchors.rightMargin: Constants.baseMargin
            Layout.alignment: Qt.AlignTop

            Item {
                height: Constants.baseMargin
            }

            GridLayout {
                columns: 2
                rowSpacing: Constants.itemMargin
                columnSpacing: Constants.itemMargin
                Layout.alignment: Qt.AlignTop

                ScreenLabel {
                    text: qsTr("Startup Application: ")
                }
                TimeComboBox {
                    id: startupComboBox
                    objectName: "startupComboBox"
                    implicitWidth: Constants.largeComboBoxWidth
                    textRole: "text"
                    model: startupModel
                }
            }

            GroupBox {
                id: vncViewerGroup
                title: ""

                GridLayout {
                    columns: 2
                    rowSpacing: Constants.itemMargin
                    columnSpacing: Constants.itemMargin
                    Layout.alignment: Qt.AlignTop

                    ScreenLabel {
                        text: qsTr("VNC Server: ")
                    }
                    GeneralTextField {
                        id: vncServerTextField
                        objectName: "vncServerTextField"
                    }

                    ScreenLabel {
                        text: qsTr("Password: ")
                    }
                    PasswordTextField {
                        id: vncPasswordTextField
                        objectName: "vncPasswordTextField"
                    }

                    ScreenLabel {
                        text: qsTr("Show Password ")
                    }
                    NetworkSwitch {
                        id: vncShowPasswordSwitch
                    }

                    ScreenLabel {
                        text: qsTr("View only (inputs ignored) ")
                    }
                    NetworkSwitch {
                        id: vncViewonlySwitch
                        objectName: "vncViewonlySwitch"
                    }

                    ScreenLabel {
                        text: qsTr("Full screen mode ")
                    }
                    NetworkSwitch {
                        id: vncFullscreenSwitch
                        objectName: "vncFullscreenSwitch"
                    }

                    ScreenLabel {
                        text: qsTr("Fit window mode ")
                    }
                    NetworkSwitch {
                        id: vncFitWindowSwitch
                        objectName: "vncFitWindowSwitch"
                    }

                    ScreenLabel {
                        text: qsTr("Image quality: ")
                    }
                    SpinBox {
                        id: vncImageQualitySpinbox
                        objectName: "vncImageQualitySpinbox"
                        from: 1
                        value: 6
                        to: 9
                    }
                }
            }

            GroupBox {
                id: staticPageGroup
                title: ""

                GridLayout {
                    columns: 2
                    rowSpacing: Constants.itemMargin
                    columnSpacing: Constants.itemMargin
                    Layout.alignment: Qt.AlignTop

                    ScreenLabel {
                        text: qsTr("Timeout(sec): ")
                    }
                    GeneralTextField {
                        id: timeoutTextField
                        objectName: "timeoutTextField"
                        validator: IntValidator {
                            bottom: 1
                        }
                        inputMethodHints: Qt.ImhPreferNumbers
                        text: "180"
                    }

                    ScreenLabel {
                        text: qsTr("URL: ")
                    }
                    ScrollTextField {
                        id: urlTextField
                        objectName: "urlTextField"
                        text: "http://"
                    }
                }
            }

            GroupBox {
                id: staticPageCustomGroup
                title: ""

                GridLayout {
                    columns: 2
                    rowSpacing: Constants.itemMargin
                    columnSpacing: Constants.itemMargin
                    Layout.alignment: Qt.AlignTop

                    ScreenLabel {
                        text: qsTr("Static Page File: ")
                    }
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
            }

            GroupBox {
                id: webPageGroup
                title: ""

                ColumnLayout {
                    Repeater {
                        id: webPageRepeater
                        objectName: "webPageRepeater"
                        model: webPageModel

                        RowLayout {
                            TimeRadioButton {
                                ButtonGroup.group: radioGroup
                                checked: model.is_startup
                            }
                            ScrollTextField {
                                text: model.page
                            }
                            DeleteRowButton {
                                rowIndex: model.index
                                visible: (model.index > 0)
                            }
                        }
                    }
                    NetworkButton {
                        id: addWebPageButton
                        objectName: "addWebPageButton"
                        text: qsTr("Add")
                    }
                }
            }

            GridLayout {
                id: autoRestartGrid
                columns: 2
                rowSpacing: Constants.itemMargin
                columnSpacing: Constants.itemMargin
                Layout.alignment: Qt.AlignTop

                ScreenLabel {
                    text: qsTr("Enable monitor startup application")
                }
                NetworkSwitch {
                    id: autoRestartSwitch
                    objectName: "autoRestartSwitch"
                }
            }

            RowLayout {
                spacing: 2
                Layout.alignment: Qt.AlignTop | Qt.AlignRight

                NetworkButton {
                    id: startupPageNextButton
                    objectName: "startupPageNextButton"
                    text: qsTr("Next")
                }
            }

            Item {
                height: Constants.baseMargin
            }

            Item {
                Layout.fillHeight: true
            }
        }
    }

    ListModel {
        id: startupModel

        ListElement {
            text: "Settings Tool"
            value: "Settings"
        }

        ListElement {
            text: "VNC Viewer"
            value: "VNCViewer"
        }

        ListElement {
            text: "Chromium"
            value: "Chromium"
        }

        ListElement {
            text: "Chromium (kiosk)"
            value: "Chromium_kiosk"
        }
    }

    ListModel {
        id: webPageModel

        ListElement {
            page: ""
            is_startup: true
        }
    }
}
