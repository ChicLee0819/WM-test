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

    property alias storageSwipeView: storageSwipeView
    property alias emmcPartitionModel: emmcPartitionModel
    property alias emmcStorageGrid: emmcStorageGrid
    property alias sdPartitionModel: sdPartitionModel
    property alias sdStorageGrid: sdStorageGrid
    property alias deviceLabel1: deviceLabel1
    property alias mountPointLabel1: mountPointLabel1
    property alias uuidLabel1: uuidLabel1
    property alias fsTypeLabel1: fsTypeLabel1
    property alias sizeLabel1: sizeLabel1
    property alias usedPercentBar1: usedPercentBar1
    property alias sdColumnLayout: sdColumnLayout
    property alias sdNotExistColumnLayout: sdNotExistColumnLayout
    property alias deviceLabel2: deviceLabel2
    property alias mountPointLabel2: mountPointLabel2
    property alias uuidLabel2: uuidLabel2
    property alias fsTypeLabel2: fsTypeLabel2
    property alias sizeLabel2: sizeLabel2
    property alias usedPercentBar2: usedPercentBar2
    property int tabbarHeight: tabBar.height

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

    ListModel {
        id: emmcPartitionModel
    }

    ListModel {
        id: sdPartitionModel
    }

    ColumnLayout {
        id: mainLayout
        width: parent.width
        height: parent.height
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        TabBar {
            id: tabBar
            currentIndex: storageSwipeView.currentIndex

            NetworkTabButton {
                text: qsTr("eMMC")
            }
            NetworkTabButton {
                text: qsTr("SD Card")
            }
        }

        SwipeView {
            id: storageSwipeView
            objectName: "storageSwipeView"
            width: mainLayout.width
            height: mainLayout.height - tabBar.height
            currentIndex: tabBar.currentIndex
            interactive: false

            Item {
                id: firstPage
                width: storageSwipeView.width
                height: storageSwipeView.height

                ButtonGroup {
                    id: firstBtnGroup
                }

                ColumnLayout {
                    width: parent.width
                    height: parent.height
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    Component {
                        id: emmcStorageDelegate
                        Item {
                            property variant storageData: model
                            width: emmcStorageGrid.cellWidth
                            height: emmcStorageGrid.cellHeight
                            Column {
                                anchors.fill: parent
                                StorageButton {
                                    text: label
                                    ButtonGroup.group: firstBtnGroup
                                }
                            }
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: Constants.itemMargin

                        ScreenLabel {
                            text: qsTr("Total Size: ")
                        }
                        ScreenLabel {
                            id: emmcSizeLabel
                            objectName: "emmcSizeLabel"
                        }
                    }

                    ScreenLabel {
                        text: qsTr("Volumes: ")
                    }

                    GridView {
                        id: emmcStorageGrid
                        width: parent.width
                        height: Constants.gridCellHeight
                        cellWidth: Constants.gridCellWidth
                        cellHeight: Constants.gridCellHeight

                        model: emmcPartitionModel
                        delegate: emmcStorageDelegate
                        focus: true
                    }

                    GridLayout {
                        id: emmcDataGrid
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10
                        Layout.alignment: Qt.AlignTop

                        ScreenLabel {
                            text: qsTr("Label: ")
                        }
                        ScreenLabel {
                            id: deviceLabel1
                        }
                        ScreenLabel {
                            text: qsTr("Mount Point: ")
                        }
                        ScreenLabel {
                            id: mountPointLabel1
                            Layout.maximumWidth: Constants.maximumLabelWidth
                        }
                        ScreenLabel {
                            text: qsTr("UUID: ")
                        }
                        ScreenLabel {
                            id: uuidLabel1
                            Layout.maximumWidth: Constants.maximumLabelWidth
                        }
                        ScreenLabel {
                            text: qsTr("File System Type: ")
                        }
                        ScreenLabel {
                            id: fsTypeLabel1
                        }
                        ScreenLabel {
                            text: qsTr("Size: ")
                        }
                        ScreenLabel {
                            id: sizeLabel1
                        }
                    }

                    ScreenLabel {
                        text: qsTr("Used: ")
                    }
                    PercentageBar {
                        id: usedPercentBar1
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            Item {
                id: secondPage
                width: storageSwipeView.width
                height: storageSwipeView.height

                ButtonGroup {
                    id: secondBtnGroup
                }

                ColumnLayout {
                    id: sdNotExistColumnLayout
                    width: parent.width
                    height: parent.height
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    ScreenLabel {
                        text: qsTr("Not exists")
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }

                ColumnLayout {
                    id: sdColumnLayout
                    width: parent.width
                    height: parent.height
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    Component {
                        id: sdStorageDelegate
                        Item {
                            property variant storageData: model
                            width: sdStorageGrid.cellWidth
                            height: sdStorageGrid.cellHeight
                            Column {
                                anchors.fill: parent
                                StorageButton {
                                    text: label
                                    ButtonGroup.group: secondBtnGroup
                                }
                            }
                        }
                    }

                    RowLayout {
                        width: parent.width
                        spacing: Constants.itemMargin

                        ScreenLabel {
                            text: qsTr("Total Size: ")
                        }
                        ScreenLabel {
                            id: sdSizeLabel
                            objectName: "sdSizeLabel"
                        }
                    }

                    ScreenLabel {
                        text: qsTr("Volumes: ")
                    }

                    GridView {
                        id: sdStorageGrid
                        width: parent.width
                        height: Constants.gridCellHeight
                        cellWidth: Constants.gridCellWidth
                        cellHeight: Constants.gridCellHeight

                        model: sdPartitionModel
                        delegate: sdStorageDelegate
                        focus: true
                    }

                    GridLayout {
                        id: sdDataGrid
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10
                        Layout.alignment: Qt.AlignTop

                        ScreenLabel {
                            text: qsTr("Label: ")
                        }
                        ScreenLabel {
                            id: deviceLabel2
                        }
                        ScreenLabel {
                            text: qsTr("Mount Point: ")
                        }
                        ScreenLabel {
                            id: mountPointLabel2
                            Layout.maximumWidth: Constants.maximumLabelWidth
                        }
                        ScreenLabel {
                            text: qsTr("UUID: ")
                        }
                        ScreenLabel {
                            id: uuidLabel2
                            Layout.maximumWidth: Constants.maximumLabelWidth
                        }
                        ScreenLabel {
                            text: qsTr("File System Type: ")
                        }
                        ScreenLabel {
                            id: fsTypeLabel2
                        }
                        ScreenLabel {
                            text: qsTr("Size: ")
                        }
                        ScreenLabel {
                            id: sizeLabel2
                        }
                    }

                    ScreenLabel {
                        text: qsTr("Used: ")
                    }
                    PercentageBar {
                        id: usedPercentBar2
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
