// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Button {
    id: root
    implicitWidth: 32
    implicitHeight: 32
    property int rowIndex: 0

    signal clickIndexSignal(var index)
    onClickIndexSignal: function (index) {
        if (systemForm.visible || wizstartupForm.visible)
            removeWebPageRow(index);
        if (networkForm.visible)
            removeFirewallRuleRow(index);
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            // show loading indicator
            loadingIndicator.open();
            // delay 1s to close loading indicator
            // this is prevent click too fast cause problem
            loadingIndicatorDelayClose(1000);
            // this will delete itself, so do it at last
            root.clickIndexSignal(root.rowIndex);
        }
    }

    contentItem: Image {
        anchors.fill: parent
        source: "../images/unchecked.png"
    }

    background: Rectangle {
        anchors.fill: parent
        color: appPalette.pageBGColor
    }
}
