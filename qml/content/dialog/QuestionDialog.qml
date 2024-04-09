// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import SettingsGUI 1.0

// dialog position is random, might be Qt's bug. It works on windows, but it doesn't work on tpc100.
/*
import QtQuick.Dialogs 1.3
MessageDialog {
    id: control
    modality: Qt.ApplicationModal
    icon: StandardIcon.Question
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onRejected: {
        console.log("aborted")
        control.close()
    }
}
*/

Popup {
    id: control
    modal: true
    focus: true
    closePolicy: Popup.NoAutoClose
    contentWidth: view.implicitWidth

    ColumnLayout {
        id: view
        width: Constants.popupWidth

        Label {
            id: questionPopupText
            objectName: "questionPopupText"
            text: ""
            wrapMode: Text.WordWrap
            Layout.preferredWidth: parent.width
            Layout.alignment: Qt.AlignHCenter
            horizontalAlignment: Text.AlignHCenter
        }

        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignHCenter

            Button {
                id: questionPopupOKButton
                objectName: "questionPopupOKButton"
                text: qsTr("OK")
                Layout.alignment: Qt.AlignHCenter
            }

            Button {
                id: questionPopupCancelButton
                objectName: "questionPopupCancelButton"
                text: qsTr("Cancel")
                Layout.alignment: Qt.AlignHCenter
                onClicked: {
                    control.close()
                }
            }
        }
    }
}
