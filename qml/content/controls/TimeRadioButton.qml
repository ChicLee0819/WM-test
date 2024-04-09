// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

RadioButton {
    id: control
    font.pointSize: Constants.baseLabelFontSize

    indicator: Rectangle {
        implicitWidth: Constants.radioCircle
        implicitHeight: Constants.radioCircle
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: width / 2
        border.width: 1
        border.color: appPalette.radioBtnBGColor

        Rectangle {
            width: parent.width * 0.7
            height: parent.height * 0.7
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
            radius: width / 2
            color: control.enabled ? appPalette.itemColor : appPalette.itemDisableColor
            visible: control.checked
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: appPalette.labelTextColor
        leftPadding: control.indicator.width + control.spacing
        verticalAlignment: Text.AlignVCenter
    }
}
