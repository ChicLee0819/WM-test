// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15

WizNetworkPageForm {
    dhcpSwitch1.onClicked: {
        editGridLayout1.visible = !dhcpSwitch1.checked
    }

    dhcpSwitch2.onClicked: {
        editGridLayout2.visible = !dhcpSwitch2.checked
    }
}
