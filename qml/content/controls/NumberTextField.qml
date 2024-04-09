// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

TextField {
    implicitWidth: Constants.textFieldWidth
    font.pointSize: Constants.baseLabelFontSize
    // only number
    validator: IntValidator {bottom: 1}
    // move to next item
    Keys.onReturnPressed: nextItemInFocusChain().forceActiveFocus()
}
