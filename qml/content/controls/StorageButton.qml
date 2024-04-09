// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Button {
    id: control
    implicitWidth: parent.width
    implicitHeight: parent.height
    font.bold: true
    font.pointSize: Constants.baseListBtnFontSize
    palette {
        buttonText: appPalette.storagebtnTextColor
        brightText: appPalette.storagebtnBrightTextColor
    }
    checkable: true

    onClicked: {
        showPratition(storageData.label, storageData.mountPoint, storageData.uuid, storageData.fstype, storageData.size, storageData.usedPercent);
    }

    contentItem: Text {
        text: control.text
        font: control.font
        color: control.down || control.checked ? control.palette.brightText : control.palette.buttonText
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Rectangle {
        border.width: control.activeFocus ? 2 : 1
        border.color: "#888"
        radius: 4
        gradient: Gradient {
            GradientStop { position: 0 ; color: control.pressed ? "#ccc" : "#eee" }
            GradientStop { position: 1 ; color: control.pressed ? "#aaa" : "#ccc" }
        }
    }
}
