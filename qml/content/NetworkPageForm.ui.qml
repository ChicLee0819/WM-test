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

    // wire1 page
    property alias linkImage1: linkImage1
    property alias ipLabel1: ipLabel1
    property alias ipAddressLabel1: ipAddressLabel1
    property alias gatewayLabel1: gatewayLabel1
    property alias defaultGatewayLabel1: defaultGatewayLabel1
    property alias dnsLabel1: dnsLabel1
    property alias dns1Label1: dns1Label1
    property alias configButton1: configButton1
    property alias configGroup1: configGroup1
    property alias editGridLayout1: editGridLayout1
    property alias dhcpSwitch1: dhcpSwitch1
    property alias ipTextField1: ipTextField1
    property alias networkMaskTextField1: networkMaskTextField1
    property alias defaultGatewayTextField1: defaultGatewayTextField1
    property alias dns1TextField1: dns1TextField1
    property alias dns2TextField1: dns2TextField1
    property alias cancelButton1: cancelButton1
    // wire2 page
    property alias linkImage2: linkImage2
    property alias ipLabel2: ipLabel2
    property alias ipAddressLabel2: ipAddressLabel2
    property alias gatewayLabel2: gatewayLabel2
    property alias defaultGatewayLabel2: defaultGatewayLabel2
    property alias dnsLabel2: dnsLabel2
    property alias dns1Label2: dns1Label2
    property alias configButton2: configButton2
    property alias configGroup2: configGroup2
    property alias editGridLayout2: editGridLayout2
    property alias dhcpSwitch2: dhcpSwitch2
    property alias ipTextField2: ipTextField2
    property alias networkMaskTextField2: networkMaskTextField2
    property alias defaultGatewayTextField2: defaultGatewayTextField2
    property alias dns1TextField2: dns1TextField2
    property alias dns2TextField2: dns2TextField2
    property alias cancelButton2: cancelButton2
    // firewall page
    property alias emptyFirewallLabel: emptyFirewallLabel
    property alias protocolModel: protocolModel
    property alias firewallRuleModel: firewallRuleModel
    property alias firewallRuleRepeater: firewallRuleRepeater
    property alias addFirewallRuleButton: addFirewallRuleButton

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
        id: firewallRuleModel
    }

    ListModel {
        id: protocolModel

        ListElement {
            text: "tcp"
        }
        ListElement {
            text: "udp"
        }
    }

    ColumnLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: Constants.baseMargin

        TabBar {
            id: tabBar
            currentIndex: swipeView.currentIndex

            NetworkTabButton {
                text: qsTr("Wired 1")
            }
            NetworkTabButton {
                text: qsTr("Wired 2")
            }
            NetworkTabButton {
                text: qsTr("Firewall")
            }
        }

        SwipeView {
            id: swipeView
            objectName: "networkSwipeView"
            width: mainLayout.width
            height: mainLayout.height - tabBar.height
            currentIndex: tabBar.currentIndex
            interactive: false

            Item {
                id: firstPage
                width: swipeView.width
                height: swipeView.height

                ColumnLayout {
                    spacing: Constants.splitMargin
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    GridLayout {
                        id: gridStatus
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10
                        Layout.alignment: Qt.AlignTop

                        ScreenLabel {
                            text: qsTr("Link: ")
                        }
                        Image {
                            id: linkImage1
                            Layout.preferredWidth: Constants.imageSquare
                            Layout.preferredHeight: Constants.imageSquare
                            source: "images/checked.png"
                        }

                        ScreenLabel {
                            text: qsTr("MAC Address: ")
                        }
                        ScreenLabel {
                            id: macAddressLabel1
                            objectName: "macAddressLabel1"
                            //text: "c4:00:ad:97:1b:0a"
                        }

                        ScreenLabel {
                            id: ipLabel1
                            text: qsTr("IP Address: ")
                        }
                        ScreenLabel {
                            id: ipAddressLabel1
                            objectName: "ipAddressLabel1"
                            //text: "172.16.12.50"
                        }

                        ScreenLabel {
                            id: gatewayLabel1
                            text: qsTr("Default Gateway: ")
                        }
                        ScreenLabel {
                            id: defaultGatewayLabel1
                            objectName: "defaultGatewayLabel1"
                        }

                        ScreenLabel {
                            id: dnsLabel1
                            text: qsTr("DNS Server: ")
                        }
                        ScreenLabel {
                            id: dns1Label1
                            objectName: "dns1Label1"
                        }

                        NetworkButton {
                            id: configButton1
                            text: qsTr("Config")
                            objectName: "configButton1"
                        }
                    }

                    GroupBox {
                        id: configGroup1
                        Layout.fillWidth: true
                        visible: false

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
                                }
                            }

                            GridLayout {
                                id: editGridLayout1
                                columns: 2
                                rowSpacing: 10
                                columnSpacing: 10
                                Layout.alignment: Qt.AlignTop

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
                                    id: applyButton1
                                    text: qsTr("Apply")
                                    objectName: "applyButton1"
                                }
                                NetworkButton {
                                    id: cancelButton1
                                    text: qsTr("Cancel")
                                    objectName: "cancelButton1"
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
                width: swipeView.width
                height: swipeView.height

                ColumnLayout {
                    spacing: Constants.splitMargin
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    GridLayout {
                        id: gridStatus2
                        columns: 2
                        rowSpacing: 10
                        columnSpacing: 10
                        Layout.alignment: Qt.AlignTop

                        ScreenLabel {
                            text: qsTr("Link: ")
                        }
                        Image {
                            id: linkImage2
                            Layout.preferredWidth: Constants.imageSquare
                            Layout.preferredHeight: Constants.imageSquare
                            source: "images/unchecked.png"
                        }

                        ScreenLabel {
                            text: qsTr("MAC Address: ")
                        }
                        ScreenLabel {
                            id: macAddressLabel2
                            objectName: "macAddressLabel2"
                        }

                        ScreenLabel {
                            id: ipLabel2
                            text: qsTr("IP Address: ")
                        }
                        ScreenLabel {
                            id: ipAddressLabel2
                            objectName: "ipAddressLabel2"
                        }

                        ScreenLabel {
                            id: gatewayLabel2
                            text: qsTr("Default Gateway: ")
                        }
                        ScreenLabel {
                            id: defaultGatewayLabel2
                            objectName: "defaultGatewayLabel2"
                        }

                        ScreenLabel {
                            id: dnsLabel2
                            text: qsTr("DNS Server: ")
                        }
                        ScreenLabel {
                            id: dns1Label2
                            objectName: "dns1Label2"
                        }

                        NetworkButton {
                            id: configButton2
                            text: qsTr("Config")
                            objectName: "configButton2"
                        }
                    }

                    GroupBox {
                        id: configGroup2
                        Layout.fillWidth: true
                        visible: false

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
                                }
                            }

                            GridLayout {
                                id: editGridLayout2
                                columns: 2
                                rowSpacing: 10
                                columnSpacing: 10
                                Layout.alignment: Qt.AlignTop

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
                                    id: applyButton2
                                    text: qsTr("Apply")
                                    objectName: "applyButton2"
                                }
                                NetworkButton {
                                    id: cancelButton2
                                    text: qsTr("Cancel")
                                    objectName: "cancelButton2"
                                }
                            }
                        }
                    }

                    Item {
                        Layout.fillHeight: true
                    }
                }
            }

            Flickable {
                id: thirdPage
                contentWidth: swipeView.width
                contentHeight: firstContent.implicitHeight
                clip: true
                ScrollBar.vertical: ScrollBar {}

                ColumnLayout {
                    id: firstContent
                    spacing: Constants.splitMargin
                    anchors.fill: parent
                    anchors.margins: Constants.baseMargin
                    Layout.alignment: Qt.AlignTop

                    Repeater {
                        id: firewallRuleRepeater
                        objectName: "firewallRuleRepeater"
                        model: firewallRuleModel

                        RowLayout {
                            ScreenLabel {
                                text: qsTr("Accept: ")
                            }
                            ComboBox {
                                implicitWidth: Constants.smallComboBoxWidth
                                model: protocolModel
                            }
                            NumberTextField {
                                text: model.port
                            }
                            DeleteRowButton {
                                rowIndex: model.index
                            }
                        }
                    }
                    ScreenLabel {
                        id: emptyFirewallLabel
                        text: qsTr("Please click Add button to add accept port")
                        visible: false
                    }
                    NetworkButton {
                        id: addFirewallRuleButton
                        objectName: "addFirewallRuleButton"
                        text: qsTr("Add")
                    }

                    RowLayout {
                        spacing: 2
                        Layout.alignment: Qt.AlignTop | Qt.AlignRight

                        NetworkButton {
                            id: firewallApplyButton
                            objectName: "firewallApplyButton"
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
        }
    }
}
