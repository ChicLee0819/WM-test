// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.platform 1.1
import SettingsGUI 1.0
import "./controls"

Item {
    id: root
    width: Constants.pageWidth
    height: Constants.pageHeight

    property alias exportButton: exportButton
    property alias exportScreenshotsButton: exportScreenshotsButton
    property alias importButton: importButton
    property alias exitButton: exitButton
    property alias opfileDialog: opfileDialog
    property alias opfolderDialog: opfolderDialog

    Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }

    GridLayout {
        anchors.fill: parent
        anchors.margins: Constants.baseMargin
        columns: 2
        rowSpacing: 10
        columnSpacing: 10
        Layout.alignment: Qt.AlignTop

        ColumnLayout {
            objectName: "exportSettingLayout"
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Export Settings: ")
            }
            NetworkButton {
                id: exportButton
                text: qsTr("Export")
                objectName: "exportButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        ColumnLayout {
            objectName: "exportScreenshotLayout"
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Export Screenshots: ")
            }
            NetworkButton {
                id: exportScreenshotsButton
                text: qsTr("Export")
                objectName: "exportScreenshotsButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        ColumnLayout {
            objectName: "importSettingLayout"
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Import Settings: ")
            }
            NetworkButton {
                id: importButton
                text: qsTr("Import")
                objectName: "importButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        ColumnLayout {
            objectName: "rebootLayout"
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Reboot System: ")
            }
            NetworkButton {
                id: rebootButton
                text: qsTr("Reboot")
                objectName: "rebootButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        ColumnLayout {
            objectName: "shutdownLayout"
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Shutdown System: ")
            }
            NetworkButton {
                id: shutdownButton
                text: qsTr("Shutdown")
                objectName: "shutdownButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        ColumnLayout {
            objectName: "openTerminalLayout"
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Open Terminal: ")
            }
            NetworkButton {
                id: openTerminalButton
                text: qsTr("Open")
                objectName: "openTerminalButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        ColumnLayout {
            objectName: "factoryResetLayout"
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Factory Reset: ")
            }
            NetworkButton {
                id: factoryResetButton
                text: qsTr("Reset")
                objectName: "factoryResetButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        ColumnLayout {
            spacing: 10
            Layout.rowSpan: 2
            Layout.columnSpan: 1

            ScreenLabel {
                text: qsTr("Exit Application: ")
            }
            NetworkButton {
                id: exitButton
                text: qsTr("Exit")
                objectName: "exitButton"
                Layout.leftMargin: Constants.baseMargin
            }
        }

        Item {
            Layout.fillHeight: true
            Layout.columnSpan: 2
        }
    }

    FileDialog {
        id: opfileDialog
        objectName: "fileDialog"
        title: "Please choose a file"
        folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        nameFilters: ["Settings export file (*.tpc)"]
        options: FileDialog.ReadOnly
    }

    FolderDialog {
        id: opfolderDialog
        objectName: "folderDialog"
        title: "Please choose a folder"
        folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        options: FolderDialog.ReadOnly
        property bool isConfig: true
        property bool isScreenshot: false
    }
}
