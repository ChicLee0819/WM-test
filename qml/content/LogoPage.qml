// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15

LogoPageForm {
    selectLogoButton.onClicked: {
        fileDialog.nameFilters = ["Image files (*.bmp)"];
        fileDialog.open();
    }
}
