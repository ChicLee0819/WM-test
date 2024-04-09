// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Button {
    id: control
    implicitWidth: Constants.buttonWidth
    implicitHeight: Constants.buttonHeight
    font.pointSize: Constants.baseBtnFontSize
    palette {
        buttonText: control.enabled ? appPalette.btnTextColor : appPalette.disableColor
    }

    background: Rectangle {
        border.width: 1
        border.color: "#888"
        radius: 50
        gradient: Gradient {
            GradientStop { position: 0 ; color: control.pressed ? appPalette.btnGD0PressedColor : appPalette.btnGD0Color }
            GradientStop { position: 1 ; color: control.pressed ? appPalette.btnGD1PressedColor : appPalette.btnGD1Color }
        }
    }
}
