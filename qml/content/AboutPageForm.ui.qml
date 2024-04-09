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

    GridLayout {
        width: Constants.aboutLayoutWidth
        anchors.margins: Constants.baseMargin
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        columns: 2
        rowSpacing: 10

        ScreenLabel {
            text: qsTr("Image Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: imageVersionLabel
            objectName: "imageVersionLabel"
            text: "TPC-100W-Yocto3.3.3-v1.0"
            Layout.maximumWidth: Constants.aboutColumnWidth
        }
        ScreenLabel {
            text: qsTr("Kernel Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: kernelVersionLabel
            objectName: "kernelVersionLabel"
            text: "Linux version 5.10.72"
            Layout.maximumWidth: Constants.aboutColumnWidth
        }
        ScreenLabel {
            text: qsTr("U-Boot Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: ubootVersionLabel
            objectName: "ubootVersionLabel"
            text: "U-Boot 2021.04"
            wrapMode: Text.Wrap
            Layout.maximumWidth: Constants.aboutColumnWidth
        }
        ScreenLabel {
            text: qsTr("App Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: appVersionLabel
            objectName: "appVersionLabel"
            text: "1.11.0"
        }
        ScreenLabel {
            text: qsTr("Qt Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: qtVersionLabel
            objectName: "qtVersionLabel"
            text: "5.15.2"
        }
        ScreenLabel {
            text: qsTr("OpenSSL Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: opensslVersionLabel
            objectName: "opensslVersionLabel"
            text: "1.1.1l"
        }
        ScreenLabel {
            text: qsTr("Java Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: javaVersionLabel
            objectName: "javaVersionLabel"
            text: "1.8.0_362"
        }
        ScreenLabel {
            text: qsTr("Chromium Version: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: chromiumVersionLabel
            objectName: "chromiumVersionLabel"
            text: "101.0.4951.54"
        }
        Item {
            height: Constants.splitMargin
        }
        Item {
            height: Constants.splitMargin
        }
        ScreenLabel {
            text: qsTr("CPU Temperature: ")
            Layout.alignment: Qt.AlignRight
        }
        ScreenLabel {
            id: temperatureLabel
            objectName: "temperatureLabel"
            text: "55.0°C"
        }
    }
}
