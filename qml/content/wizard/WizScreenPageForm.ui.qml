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

    property alias brightSlider: brightSlider
    property alias brightSpinbox: brightSpinbox

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
        }

        RowLayout {
            spacing: 2
            Layout.alignment: Qt.AlignTop | Qt.AlignRight

            NetworkButton {
                id: wizScreenPageNextButton
                objectName: "wizScreenPageNextButton"
                text: qsTr("Next")
            }
        }

        Item {
            Layout.fillHeight: true
        }
    }
}
