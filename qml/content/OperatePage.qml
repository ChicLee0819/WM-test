// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15
import QtQuick.Controls 2.15

OperatePageForm {
    exportButton.onClicked: {
        opfolderDialog.isConfig = true;
        opfolderDialog.isScreenshot = false;
        opfolderDialog.open();
    }

    importButton.onClicked: {
        opfileDialog.open();
    }

    exitButton.onClicked: {
        Qt.quit();
    }

    // these functions call from c++
    function openFolderDialogForScreenshot() {
        opfolderDialog.isConfig = false;
        opfolderDialog.isScreenshot = true;
        opfolderDialog.open();
    }
}
