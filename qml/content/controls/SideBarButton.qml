// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Button {
    id: control
    implicitWidth: parent.width
    implicitHeight: Constants.sideBarButtonHeight
    font.bold: true
    font.pointSize: Constants.baseFontSize
    palette {
        buttonText: appPalette.btnTextColor
    }
    checkable: true

    contentItem: Text {
        text: control.text
        font: control.font
        color: control.checked || control.highlighted ? control.palette.brightText : control.palette.buttonText
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        color: appPalette.btnBGColor

        // make bottom border
        Rectangle {
            implicitWidth: parent.width
            implicitHeight: Constants.borderWidth
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }

        // make right border
        Rectangle {
            implicitWidth: Constants.borderWidth
            implicitHeight: parent.height
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.right: parent.right
        }
    }
}
