// Copyright (C) 2022 The Advantech Company Ltd.
// SPDX-License-Identifier: GPL-3.0-only

import QtQuick 2.15

WizStartupPageForm {
    startupComboBox.onCurrentIndexChanged: {
        let autoRestartShow = false;
        let vncViewerShow = false;
        let staticPageShow = false;
        let staticPageCustomShow = false;
        let webPageShow = false;
        if (startupComboBox.currentIndex == 1) {
            // select VNC
            vncViewerShow = true;
        } else if (startupComboBox.currentIndex == 2) {
            // select Chromium
            webPageShow = true;
        } else if (startupComboBox.currentIndex == 3) {
            // select Chromium Kiosk
            autoRestartShow = true;
            webPageShow = true;
        } else if (startupComboBox.currentIndex == 4) {
            // select Static Page
            staticPageShow = true;
            autoRestartShow = true;
        } else if (startupComboBox.currentIndex == 5) {
            // select custom Static Page
            staticPageCustomShow = true;
            autoRestartShow = true;
        } else {
            // select others
        }
        autoRestartGrid.visible = autoRestartShow;
        vncViewerGroup.visible = vncViewerShow;
        staticPageGroup.visible = staticPageShow;
        staticPageCustomGroup.visible = staticPageCustomShow;
        webPageGroup.visible = webPageShow;
    }

    vncShowPasswordSwitch.onClicked: {
        vncPasswordTextField.echoMode = vncShowPasswordSwitch.checked ? TextInput.Normal : TextInput.Password
    }

    selectButton.onClicked: {
        fileDialog.nameFilters = ["HTML files (*.html)"];
        fileDialog.open();
    }

    addWebPageButton.onClicked: {
        let empty = {
            "page": "",
            "is_startup": false,
        };
        webPageModel.append(empty);
    }

    function removeWebPageRow(index) {
        webPageModel.remove(index);
    }

    // these functions call from c++
    function getWebPageByIndex(index) {
        let page = {
            "page": webPageRepeater.itemAt(index).children[1].text,
            "is_startup": webPageRepeater.itemAt(index).children[0].checked,
        };
        return page;
    }

    function getCurrentStartup() {
        return startupModel.get(startupComboBox.currentIndex).value;
    }
}
