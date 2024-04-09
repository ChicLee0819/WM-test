// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Slider {
    id: control
    implicitWidth: Constants.sliderWidth

    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        width: control.availableWidth
        height: 4
        radius: 2
        color: appPalette.itemDisableColor

        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            color: appPalette.itemColor
            radius: 2
        }
    }

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 26
        implicitHeight: 26
        radius: 13
        color: control.pressed ? "#f0f0f0" : "#f6f6f6"
        border.color: appPalette.itemDisableColor
    }
}


