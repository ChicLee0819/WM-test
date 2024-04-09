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
    clip: true

    property alias networkSwipeView: networkSwipeView
    property alias editGridLayout1: editGridLayout1
    property alias dhcpSwitch1: dhcpSwitch1
    property alias ipTextField1: ipTextField1
    property alias networkMaskTextField1: networkMaskTextField1
    property alias defaultGatewayTextField1: defaultGatewayTextField1
    property alias dns1TextField1: dns1TextField1
    property alias dns2TextField1: dns2TextField1
    property alias wizNetworkPageNextButton1: wizNetworkPageNextButton1
    property alias editGridLayout2: editGridLayout2
    property alias dhcpSwitch2: dhcpSwitch2
    property alias ipTextField2: ipTextField2
    property alias networkMaskTextField2: networkMaskTextField2
    property alias defaultGatewayTextField2: defaultGatewayTextField2
    property alias dns1TextField2: dns1TextField2
    property alias dns2TextField2: dns2TextField2
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

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        TabBar {
            id: tabBar
            currentIndex: networkSwipeView.currentIndex

            NetworkTabButton {
                text: qsTr("Wired 1")
            }
            NetworkTabButton {
                text: qsTr("Wired 2")
            }
        }

        SwipeView {
            id: networkSwipeView
            objectName: "networkSwipeView"
            width: mainLayout.width
            height: mainLayout.height - tabBar.height
            currentIndex: tabBar.currentIndex
            interactive: false

            Item {
                id: firstPage
                width: networkSwipeView.width
                height: networkSwipeView.height

                ColumnLayout {
                    spacing: Constants.splitMargin
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    GroupBox {
                        Layout.fillWidth: true

                        ColumnLayout {
                            width: parent.width
                            spacing: Constants.splitMargin

                            GridLayout {
                                columns: 2
                                rowSpacing: 10
                                columnSpacing: 10
                                Layout.alignment: Qt.AlignTop

                                ScreenLabel {
                                    text: qsTr("Enable DHCP ")
                                }
                                NetworkSwitch {
                                    id: dhcpSwitch1
                                    objectName: "dhcpSwitch1"
                                    checked: true
                                }
                            }

                            GridLayout {
                                id: editGridLayout1
                                columns: 2
                                rowSpacing: 10
                                columnSpacing: 10
                                Layout.alignment: Qt.AlignTop
                                visible: false

                                ScreenLabel {
                                    text: qsTr("IP Address: ")
                                }
                                NetworkTextField {
                                    id: ipTextField1
                                    objectName: "ipTextField1"
                                    placeholderText: qsTr("Required")
                                }

                                ScreenLabel {
                                    text: qsTr("Network Mask: ")
                                }
                                NetworkTextField {
                                    id: networkMaskTextField1
                                    objectName: "networkMaskTextField1"
                                }

                                ScreenLabel {
                                    text: qsTr("Default Gateway: ")
                                }
                                NetworkTextField {
                                    id: defaultGatewayTextField1
                                    objectName: "defaultGatewayTextField1"
                                }

                                ScreenLabel {
                                    text: qsTr("DNS Server 1: ")
                                }
                                NetworkTextField {
                                    id: dns1TextField1
                                    objectName: "dns1TextField1"
                                }

                                ScreenLabel {
                                    text: qsTr("DNS Server 2: ")
                                }
                                NetworkTextField {
                                    id: dns2TextField1
                                    objectName: "dns2TextField1"
                                }
                            }

                            RowLayout {
                                spacing: Constants.splitMargin
                                Layout.alignment: Qt.AlignTop | Qt.AlignRight

                                NetworkButton {
                                    id: wizNetworkPageNextButton1
                                    objectName: "wizNetworkPageNextButton1"
                                    text: qsTr("Next")
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            Item {
                id: secondPage
                width: networkSwipeView.width
                height: networkSwipeView.height

                ColumnLayout {
                    spacing: Constants.splitMargin
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    GroupBox {
                        Layout.fillWidth: true

                        ColumnLayout {
                            width: parent.width
                            spacing: Constants.splitMargin

                            GridLayout {
                                columns: 2
                                rowSpacing: 10
                                columnSpacing: 10
                                Layout.alignment: Qt.AlignTop

                                ScreenLabel {
                                    text: qsTr("Enable DHCP ")
                                }
                                NetworkSwitch {
                                    id: dhcpSwitch2
                                    objectName: "dhcpSwitch2"
                                    checked: true
                                }
                            }

                            GridLayout {
                                id: editGridLayout2
                                columns: 2
                                rowSpacing: 10
                                columnSpacing: 10
                                Layout.alignment: Qt.AlignTop
                                visible: false

                                ScreenLabel {
                                    text: qsTr("IP Address: ")
                                }
                                NetworkTextField {
                                    id: ipTextField2
                                    objectName: "ipTextField2"
                                    placeholderText: qsTr("Required")
                                }

                                ScreenLabel {
                                    text: qsTr("Network Mask: ")
                                }
                                NetworkTextField {
                                    id: networkMaskTextField2
                                    objectName: "networkMaskTextField2"
                                }

                                ScreenLabel {
                                    text: qsTr("Default Gateway: ")
                                }
                                NetworkTextField {
                                    id: defaultGatewayTextField2
                                    objectName: "defaultGatewayTextField2"
                                }

                                ScreenLabel {
                                    text: qsTr("DNS Server 1: ")
                                }
                                NetworkTextField {
                                    id: dns1TextField2
                                    objectName: "dns1TextField2"
                                }

                                ScreenLabel {
                                    text: qsTr("DNS Server 2: ")
                                }
                                NetworkTextField {
                                    id: dns2TextField2
                                    objectName: "dns2TextField2"
                                }
                            }

                            RowLayout {
                                spacing: Constants.splitMargin
                                Layout.alignment: Qt.AlignTop | Qt.AlignRight

                                NetworkButton {
                                    id: wizNetworkPageNextButton2
                                    objectName: "wizNetworkPageNextButton2"
                                    text: qsTr("Next")
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
