// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15
import SettingsGUI 1.0

TextField {
    id: control
    implicitWidth: Constants.textFieldWidth
    font.pointSize: Constants.baseLabelFontSize
    // move to next item
    Keys.onReturnPressed: nextItemInFocusChain().forceActiveFocus()
    // ip address regExp
    validator: RegularExpressionValidator { regularExpression: /^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$/ }
}

