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

    property alias selectLogoButton: selectLogoButton
    property alias logoPathTextField: logoPathTextField
    property alias bgColorTextField: bgColorTextField

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
                text: qsTr("Boot logo Path: ")
            }
            GeneralTextField {
                id: logoPathTextField
                objectName: "logoPathTextField"
                readOnly: true
            }

            Item {
                height: Constants.baseMargin
            }
            NetworkButton {
                id: selectLogoButton
                objectName: "selectLogoButton"
                text: qsTr("Select")
            }

            ScreenLabel {
                text: qsTr("Background color: ")
            }
            GeneralTextField {
                id: bgColorTextField
                objectName: "bgColorTextField"
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
