// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Button {
    implicitWidth: 50
    implicitHeight: 50

    contentItem: Image {
        anchors.fill: parent
        source: "../images/arrow_icon.png"
    }

    background: Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }
}
