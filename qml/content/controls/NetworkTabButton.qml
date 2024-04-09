// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

TabButton {
    id: control
    implicitWidth: Constants.tabBarWidth
    implicitHeight: Constants.tabBarHeight
    font.pointSize: Constants.baseLabelFontSize
    palette {
        buttonText: appPalette.btnTextColor
        brightText: appPalette.btnBrightTextColor
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.down || control.checked ? control.palette.brightText : control.palette.buttonText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        color: control.down || control.checked ? appPalette.pressedBtnBGColor : appPalette.tabBtnBGColor
        border.width: 1
        border.color: appPalette.borderColor

        // overwrite bottom border
        Rectangle {
            id: bottomRect
            color: parent.color
            height: control.down || control.checked ? 1 : 0
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
        }
    }
}
