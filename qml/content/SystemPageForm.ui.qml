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
    clip: true

    property alias systemTabBar: systemTabBar
    property alias startupComboBox: startupComboBox
    property alias startupModel: startupModel
    property alias vncViewerGroup: vncViewerGroup
    property alias staticPageGroup: staticPageGroup
    property alias staticPageCustomGroup: staticPageCustomGroup
    property alias autoRestartGrid: autoRestartGrid
    property alias selectButton: selectButton
    property alias fileTextField: fileTextField
    property alias vncShowPasswordSwitch: vncShowPasswordSwitch
    property alias vncPasswordTextField: vncPasswordTextField
    property alias com1ModeComboBox: com1ModeComboBox
    property alias com1ModeModel: com1ModeModel
    property alias com1BaudRateComboBox: com1BaudRateComboBox
    property alias com2BaudRateComboBox: com2BaudRateComboBox
    property alias comBaudRateModel: comBaudRateModel
    property alias webPageGroup: webPageGroup
    property alias webPageModel: webPageModel
    property alias webPageRepeater: webPageRepeater
    property alias addWebPageButton: addWebPageButton
    property alias rebootSystemCronJobSwitch: rebootSystemCronJobSwitch
    property alias crontabGroup: crontabGroup
    property alias cronModeModel: cronModeModel
    property alias cronModeComboBox: cronModeComboBox
    property alias cronMinuteComboBox: cronMinuteComboBox
    property alias cronHourComboBox: cronHourComboBox
    property alias cronDayofweekLabel: cronDayofweekLabel
    property alias cronDayofweekComboBox: cronDayofweekComboBox
    property int tabbarHeight: systemTabBar.height

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    // swipeView border
    Rectangle {
        width: mainLayout.width
        height: mainLayout.height - tabbarHeight
        color: appPalette.pageBGColor
        border.width: 1
        border.color: appPalette.borderColor
        anchors.top: mainLayout.top
        anchors.left: mainLayout.left
        anchors.topMargin: tabbarHeight - 1
    }

    // radio button group
    ButtonGroup {
        id: radioGroup
    }

    ListModel {
        id: webPageModel
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        TabBar {
            id: systemTabBar
            currentIndex: swipeView.currentIndex

            NetworkTabButton {
                text: qsTr("Startup")
            }
            NetworkTabButton {
                text: qsTr("General")
            }
            NetworkTabButton {
                text: qsTr("COM")
            }
        }

        SwipeView {
            id: swipeView
            objectName: "systemSwipeView"
            width: mainLayout.width
            height: mainLayout.height - systemTabBar.height
            currentIndex: systemTabBar.currentIndex
            interactive: false

            Flickable {
                id: firstPage
                contentWidth: swipeView.width
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
                                implicitWidth: Constants.smallTextFieldWidth
                            }

                            ScreenLabel {
                                text: qsTr("Password: ")
                            }
                            PasswordTextField {
                                id: vncPasswordTextField
                                objectName: "vncPasswordTextField"
                                implicitWidth: Constants.smallTextFieldWidth
                            }

                            ScreenLabel {
                                text: qsTr("Show Password ")
                            }
                            NetworkSwitch {
                                id: vncShowPasswordSwitch
                                objectName: "vncShowPasswordSwitch"
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

                    RowLayout {
                        spacing: 2
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight

                        NetworkButton {
                            id: startupPageApplyButton
                            objectName: "startupPageApplyButton"
                            text: qsTr("Apply")
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

            Item {
                width: swipeView.width
                height: swipeView.height

                ColumnLayout {
                    spacing: Constants.splitMargin
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    GridLayout {
                        columns: 2
                        rowSpacing: Constants.itemMargin
                        columnSpacing: Constants.itemMargin
                        Layout.alignment: Qt.AlignTop

                        ScreenLabel {
                            text: qsTr("Default login desktop as user")
                        }
                        NetworkSwitch {
                            id: userLoginSwitch
                            objectName: "userLoginSwitch"
                        }
                        ScreenLabel {
                            text: qsTr("Enable read-only system")
                        }
                        NetworkSwitch {
                            id: readonlySwitch
                            objectName: "readonlySwitch"
                        }
                        ScreenLabel {
                            text: qsTr("Enable ethernet")
                        }
                        NetworkSwitch {
                            id: ethernetSwitch
                            objectName: "ethernetSwitch"
                        }
                        ScreenLabel {
                            text: qsTr("Enable USB")
                        }
                        NetworkSwitch {
                            id: usbSwitch
                            objectName: "usbSwitch"
                        }
                        ScreenLabel {
                            text: qsTr("Enable system virtual keyboard for Chromium")
                            objectName: "chromiumSysVKBLabel"
                            Layout.maximumWidth: Constants.maximumLabelWidth
                        }
                        NetworkSwitch {
                            id: chromiumSysVKBSwitch
                            objectName: "chromiumSysVKBSwitch"
                        }
                        ScreenLabel {
                            text: qsTr("Enable reboot system automatically")
                        }
                        NetworkSwitch {
                            id: rebootSystemCronJobSwitch
                            objectName: "rebootSystemCronJobSwitch"
                        }
                    }

                    GroupBox {
                        id: crontabGroup
                        title: ""

                        GridLayout {
                            columns: 2
                            rowSpacing: Constants.itemMargin
                            columnSpacing: Constants.itemMargin
                            Layout.alignment: Qt.AlignTop
                            Layout.leftMargin: Constants.indentMargin

                            ScreenLabel {
                                text: qsTr("Mode: ")
                            }
                            TimeComboBox {
                                id: cronModeComboBox
                                objectName: "cronModeComboBox"
                                implicitWidth: Constants.generalComboBoxWidth
                                textRole: "text"
                                model: cronModeModel
                            }

                            ScreenLabel {
                                text: qsTr("Minute: ")
                            }
                            ComboBox {
                                id: cronMinuteComboBox
                                objectName: "cronMinuteComboBox"
                                model: 60
                                implicitWidth: Constants.smallComboBoxWidth
                            }

                            ScreenLabel {
                                text: qsTr("Hour: ")
                            }
                            ComboBox {
                                id: cronHourComboBox
                                objectName: "cronHourComboBox"
                                model: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23]
                                implicitWidth: Constants.smallComboBoxWidth
                            }

                            ScreenLabel {
                                id: cronDayofweekLabel
                                text: qsTr("Weekday: ")
                            }
                            ComboBox {
                                id: cronDayofweekComboBox
                                objectName: "cronDayofweekComboBox"
                                model: [0, 1, 2, 3, 4, 5, 6, 7]
                                implicitWidth: Constants.smallComboBoxWidth
                            }
                        }
                    }

                    RowLayout {
                        spacing: 2
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight

                        NetworkButton {
                            id: generalPageApplyButton
                            objectName: "generalPageApplyButton"
                            text: qsTr("Apply")
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            Item {
                width: swipeView.width
                height: swipeView.height

                ColumnLayout {
                    spacing: Constants.splitMargin
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    GridLayout {
                        columns: 2
                        rowSpacing: Constants.itemMargin
                        columnSpacing: Constants.itemMargin
                        Layout.alignment: Qt.AlignTop

                        ScreenLabel {
                            text: qsTr("COM1 Mode: ")
                        }
                        TimeComboBox {
                            id: com1ModeComboBox
                            objectName: "com1ModeComboBox"
                            implicitWidth: Constants.generalComboBoxWidth
                            textRole: "text"
                            model: com1ModeModel
                        }
                        ScreenLabel {
                            text: qsTr("COM1 Baud Rate: ")
                        }
                        TimeComboBox {
                            id: com1BaudRateComboBox
                            objectName: "com1BaudRateComboBox"
                            implicitWidth: Constants.generalComboBoxWidth
                            textRole: "text"
                            model: comBaudRateModel
                        }

                        ScreenLabel {
                            text: qsTr("COM2 Mode: ")
                        }
                        ScreenLabel {
                            text: qsTr("RS485")
                        }
                        ScreenLabel {
                            text: qsTr("COM2 Baud Rate: ")
                        }
                        TimeComboBox {
                            id: com2BaudRateComboBox
                            objectName: "com2BaudRateComboBox"
                            implicitWidth: Constants.generalComboBoxWidth
                            textRole: "text"
                            model: comBaudRateModel
                        }
                    }

                    RowLayout {
                        spacing: 2
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight

                        NetworkButton {
                            id: comPageApplyButton
                            objectName: "comPageApplyButton"
                            text: qsTr("Apply")
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
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

        ListElement {
            text: "Static Page"
            value: "StaticPage"
        }

        ListElement {
            text: "Static Page (custom)"
            value: "StaticPage_custom"
        }

        ListElement {
            text: "None"
            value: "None"
        }
    }

    ListModel {
        id: com1ModeModel

        ListElement {
            text: "RS232"
            value: "rs232"
        }
        ListElement {
            text: "RS422"
            value: "rs422"
        }
        ListElement {
            text: "RS485"
            value: "rs485"
        }
    }

    ListModel {
        id: comBaudRateModel

        ListElement {
            text: "9600"
            value: "9600"
        }
        ListElement {
            text: "19200"
            value: "19200"
        }
        ListElement {
            text: "38400"
            value: "38400"
        }
        ListElement {
            text: "57600"
            value: "57600"
        }
        ListElement {
            text: "115200"
            value: "115200"
        }
    }

    ListModel {
        id: cronModeModel

        ListElement {
            text: "Daily"
            value: "daily"
        }
        ListElement {
            text: "Weekly"
            value: "weekly"
        }
    }
}
