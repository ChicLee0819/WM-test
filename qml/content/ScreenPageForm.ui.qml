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

    property alias brightSlider: brightSlider
    property alias brightSpinbox: brightSpinbox
    property alias screensaverSwitch: screensaverSwitch
    property alias screensaverSpinbox: screensaverSpinbox
    property alias topbarPositionModel: topbarPositionModel
    property alias topbarPositionComboBox: topbarPositionComboBox
    property alias rotateScreenModel: rotateScreenModel
    property alias rotateScreenComboBox: rotateScreenComboBox
    property alias gestureSwitch: gestureSwitch
    property alias gestureSwipeDownSwitch: gestureSwipeDownSwitch
    property alias gestureSwipeUpSwitch: gestureSwipeUpSwitch
    property alias gestureSwipeRightSwitch: gestureSwipeRightSwitch

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
                id: brightLabel
                text: qsTr("Brightness: ")
            }
            RowLayout {
                spacing: Constants.itemMargin

                ScreenSlider {
                    id: brightSlider
                    objectName: "brightSlider"
                    from: 5
                    value: 50
                    to: 100
                }

                SpinBox {
                    id: brightSpinbox
                    objectName: "brightSpinbox"
                    from: 5
                    value: 50
                    to: 100
                }
            }

            ScreenLabel {
                text: qsTr("Enable Screensaver ")
            }
            NetworkSwitch {
                id: screensaverSwitch
                objectName: "screensaverSwitch"
            }

            ScreenLabel {
                Layout.leftMargin: Constants.indentMargin
                text: qsTr("Blank After(minutes): ")
            }
            SpinBox {
                id: screensaverSpinbox
                objectName: "screensaverSpinbox"
                from: 1
                value: 5
                to: 60
            }

            ScreenLabel {
                text: qsTr("Hide Cursor ")
            }
            NetworkSwitch {
                id: hidecursorSwitch
                objectName: "hidecursorSwitch"
            }

            ScreenLabel {
                text: qsTr("Top Bar Position ")
            }
            TimeComboBox {
                id: topbarPositionComboBox
                objectName: "topbarPositionComboBox"
                implicitWidth: Constants.generalComboBoxWidth
                textRole: "text"
                model: ListModel {
                    id: topbarPositionModel

                    ListElement {
                        text: "Top"
                        value: "top"
                    }

                    ListElement {
                        text: "Bottom"
                        value: "bottom"
                    }

                    ListElement {
                        text: "None"
                        value: "none"
                    }
                }
            }

            ScreenLabel {
                text: qsTr("Rotate Screen ")
            }
            TimeComboBox {
                id: rotateScreenComboBox
                objectName: "rotateScreenComboBox"
                implicitWidth: Constants.generalComboBoxWidth
                textRole: "text"
                model: ListModel {
                    id: rotateScreenModel

                    ListElement {
                        text: "0"
                        value: "normal"
                    }

                    ListElement {
                        text: "90"
                        value: "rotate-90"
                    }

                    ListElement {
                        text: "180"
                        value: "rotate-180"
                    }

                    ListElement {
                        text: "270"
                        value: "rotate-270"
                    }
                }
            }

            ScreenLabel {
                text: qsTr("Enable Gesture ")
                objectName: "gestureLabel"
            }
            NetworkSwitch {
                id: gestureSwitch
                objectName: "gestureSwitch"
            }
        }

        GroupBox {
            id: gestureGroup
            objectName: "gestureGroup"
            title: ""

            GridLayout {
                columns: 2
                rowSpacing: 10
                columnSpacing: 10
                Layout.alignment: Qt.AlignTop

                ScreenLabel {
                    text: qsTr("Enable Swipe Down ")
                }
                NetworkSwitch {
                    id: gestureSwipeDownSwitch
                    objectName: "gestureSwipeDownSwitch"
                }

                ScreenLabel {
                    text: qsTr("Enable Swipe Up ")
                }
                NetworkSwitch {
                    id: gestureSwipeUpSwitch
                    objectName: "gestureSwipeUpSwitch"
                }

                ScreenLabel {
                    text: qsTr("Enable Swipe Right ")
                }
                NetworkSwitch {
                    id: gestureSwipeRightSwitch
                    objectName: "gestureSwipeRightSwitch"
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
