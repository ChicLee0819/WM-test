// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15

OPCUAPageForm {
    id: opcuaForm

    opcuaEnableSwitch.onClicked: {
        opcuaPortTextField.enabled = opcuaEnableSwitch.checked
        identityTypeComboBox.enabled = opcuaEnableSwitch.checked
        usernameTextField.enabled = opcuaEnableSwitch.checked
        passwordTextField.enabled = opcuaEnableSwitch.checked
    }

    identityTypeComboBox.onCurrentIndexChanged: {
        if (identityTypeComboBox.currentIndex == 0) {
            // select Anonymous
            usernameGroup.visible = false;
        } else {
            // select Username/Password
            usernameGroup.visible = true;
        }
    }

    showPasswordSwitch.onClicked: {
        passwordTextField.echoMode = showPasswordSwitch.checked ? TextInput.Normal : TextInput.Password;
    }

    // these functions call from c++
    function initOPCUAComboBox(name) {
        let nIndex;

        for (let i = 0; i < identityTypeModel.count; i++)
        {
            if (identityTypeModel.get(i).value === name) {
                nIndex = i;
                break;
            }
        }
        identityTypeComboBox.currentIndex = nIndex;
    }

    function getCurrentIdentityType() {
        return identityTypeModel.get(identityTypeComboBox.currentIndex).value;
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
