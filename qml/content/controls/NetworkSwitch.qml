// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

Switch {
    id: control

    indicator: Rectangle {
        implicitWidth: Constants.switchWidth
        implicitHeight: Constants.switchHeight
        x: control.leftPadding
        y: parent.height / 2 - height / 2
        radius: 13
        color: {
            if (control.enabled) {
                if (control.checked)
                    return appPalette.itemColor
                else
                    return appPalette.baseBGColor
            }
            else {
                return appPalette.disableColor
            }
        }
        border.color: control.checked ? appPalette.itemColor : appPalette.itemDisableColor

        Rectangle {
            x: control.checked ? parent.width - width : 0
            width: Constants.switchHeight
            height: Constants.switchHeight
            radius: 13
            color: {
                if (control.enabled) {
                    if (control.down)
                        return appPalette.itemDisableColor
                    else
                        return appPalette.baseBGColor
                }
                else {
                    return appPalette.disableColor
                }
            }
            border.color: appPalette.itemDisableColor
        }
    }
}


