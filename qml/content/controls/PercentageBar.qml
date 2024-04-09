// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Item {
    id: root
    implicitWidth: Constants.percentageBarWidth
    implicitHeight: 40
    property int percentage: 0

    Rectangle {
        id: pb_back
        radius: 4
        anchors.right: parent.right
        anchors.rightMargin: 60
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        border.width: 1
        border.color: appPalette.borderColor

        Rectangle {
            id: pb_front
            color: appPalette.barColor
            width: 200
            anchors.bottomMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.topMargin: 5
        }
    }

    Text {
        id: text1
        color: appPalette.btnTextColor
        text: "%1%".arg(Math.round(root.percentage))
        font.pixelSize: 18
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 10
    }

    states: [
        State {
            name: "valueChanged"
            PropertyChanges { target: pb_front; width: Constants.percentageBarFrontWidth * root.percentage / 100 }
            PropertyChanges { target: text1; text: "%1%".arg(Math.round(root.percentage)) }
        }
    ]
}
