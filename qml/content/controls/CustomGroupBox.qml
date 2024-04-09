// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

GroupBox {
    id: control

    label: Rectangle {
        width: title.width * 1.2
        height: title.font.pixelSize
        anchors.bottom: parent.top
        anchors.bottomMargin: -height / 2
        anchors.left: parent.left
        anchors.leftMargin: 50
        color: appPalette.pageBGColor

        Text {
            id: title
            text: control.title
            anchors.centerIn: parent
            color: appPalette.labelTextColor
            font.family: control.font.family
            font.pointSize: Constants.baseLabelFontSize
        }
    }
}
