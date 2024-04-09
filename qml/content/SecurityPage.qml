// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15

SecurityPageForm {

    enableSwitch.onClicked: {
        passwordRadioButton.enabled = enableSwitch.checked
        systemUserRadioButton.enabled = enableSwitch.checked
        passwordTextField.enabled = enableSwitch.checked & passwordRadioButton.checked
        showPasswordSwitch.enabled = enableSwitch.checked & passwordRadioButton.checked
    }

    showPasswordSwitch.onClicked: {
        passwordTextField.echoMode = showPasswordSwitch.checked ? TextInput.Normal : TextInput.Password
    }

    passwordRadioButton.onClicked: {
        showPasswordSwitch.enabled = passwordRadioButton.checked
        passwordTextField.enabled = passwordRadioButton.checked
    }

    systemUserRadioButton.onClicked: {
        showPasswordSwitch.enabled = passwordRadioButton.checked
        passwordTextField.enabled = passwordRadioButton.checked
    }
}
