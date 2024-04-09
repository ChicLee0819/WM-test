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

    property alias currentDateText: currentDateText
    property alias currentTimeText: currentTimeText
    property alias timezoneComboBox: timezoneComboBox
    property alias timezoneModel: timezoneModel
    property alias timezoneTextField: timezoneTextField
    property alias ntpRadioButton: ntpRadioButton
    property alias manualRadioButton: manualRadioButton
    property alias ntpServerTextField: ntpServerTextField
    property alias dateTextField: dateTextField
    property alias dateButton: dateButton
    property alias hourComboBox: hourComboBox
    property alias minuteComboBox: minuteComboBox
    property alias secondComboBox: secondComboBox

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

        RowLayout {
            spacing: Constants.itemMargin

            ScreenLabel {
                text: qsTr("Current Time: ")
            }

            ScreenLabel {
                id: currentDateText
                objectName: "currentDateText"
                text: Qt.formatDateTime(new Date(), "yyyy-MM-dd")
            }

            ScreenLabel {
                id: currentTimeText
                objectName: "currentTimeText"
                text: Qt.formatDateTime(new Date(), "hh:mm:ss")
            }
        }

        Item {
            height: Constants.baseMargin
        }

        RowLayout {
            spacing: Constants.itemMargin

            ScreenLabel {
                text: qsTr("Timezone: ")
            }

            TimeComboBox {
                id: timezoneComboBox
                objectName: "timezoneComboBox"
                textRole: "text"
                model: ListModel {
                    id: timezoneModel
                }
            }

            GeneralTextField {
                id: timezoneTextField
                objectName: "timezoneTextField"
                visible: false
            }
        }

        Item {
            height: Constants.baseMargin
        }

        TimeRadioButton {
            id: ntpRadioButton
            objectName: "ntpRadioButton"
            text: qsTr("Synchronize with NTP Server")
            ButtonGroup.group: radioGroup
        }

        RowLayout {
            Layout.leftMargin: Constants.indentMargin
            spacing: Constants.itemMargin

            ScreenLabel {
                text: qsTr("NTP Server: ")
            }

            TimeTextField {
                id: ntpServerTextField
                objectName: "ntpServerTextField"
                placeholderText: qsTr("Optional")
            }
        }

        Item {
            height: Constants.baseMargin
        }

        TimeRadioButton {
            id: manualRadioButton
            objectName: "manualRadioButton"
            text: qsTr("Manual Setting")
            checked: true
            ButtonGroup.group: radioGroup
        }

        RowLayout {
            Layout.leftMargin: Constants.indentMargin
            spacing: Constants.itemMargin

            ScreenLabel {
                text: qsTr("Date: ")
            }

            TimeTextField {
                id: dateTextField
                objectName: "dateTextField"
                enabled: false
            }
            CalendarButton {
                id: dateButton
                objectName: "dateButton"
            }
        }

        RowLayout {
            Layout.leftMargin: Constants.indentMargin
            spacing: Constants.itemMargin

            ScreenLabel {
                text: qsTr("Time: ")
            }

            ComboBox {
                id: hourComboBox
                objectName: "hourComboBox"
                model: [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23]
                implicitWidth: Constants.smallComboBoxWidth
            }

            ScreenLabel {
                text: ":"
            }

            ComboBox {
                id: minuteComboBox
                objectName: "minuteComboBox"
                model: 60
                implicitWidth: Constants.smallComboBoxWidth
            }

            ScreenLabel {
                text: ":"
            }

            ComboBox {
                id: secondComboBox
                objectName: "secondComboBox"
                model: 60
                implicitWidth: Constants.smallComboBoxWidth
            }
        }

        Item {
            height: Constants.baseMargin
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight

            NetworkButton {
                id: wizTimePageNextButton
                objectName: "wizTimePageNextButton"
                text: qsTr("Next")
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
