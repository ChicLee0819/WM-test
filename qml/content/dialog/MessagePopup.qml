// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SettingsGUI 1.0

Popup {
    id: control
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose
    contentWidth: view.implicitWidth

    ColumnLayout {
        id: view
        width: Constants.popupWidth

        Text {
            id: messagePopupText
            objectName: "messagePopupText"
            text: ""
            wrapMode: Text.WordWrap
            Layout.preferredWidth: parent.width
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Button {
            id: messagePopupButton
            objectName: "messagePopupButton"
            text: qsTr("Close")
            Layout.alignment: Qt.AlignHCenter
            onClicked: {
                control.close()
            }
        }
    }
}

